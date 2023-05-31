#include "common.h"
#include "string.h"
#include "debug.h"

#include "rle.h"

//#define RLE_DEBUG
#ifdef RLE_DEBUG
	#define pr_debug(fmt, args...) dbg_printf(fmt, ## args)
#else
	#define pr_debug(fmt, args...)
#endif

#define RLE_HEADER 0x2E454C52 // The header is 'RLE.'

#define REPET_ALPHA

/* 0xaabbccdd */
#define TO_WORD(a, b, c, d) (((a) & 0xFF) << 24 | ((b) & 0xFF) << 16 | ((c) & 0xFF) << 8 | ((d) & 0xFF))

#define OFF_HEADER 0
#define OFF_LENGTH 4
#define OFF_DATA   8

#define F_EXT  0x80 // Length extension flag
#define F_REPE 0x40 // Data duplication flag
#define F_WORD 0x20 // Word data flag, default is byte

#define FLAG_NON_MASK  0xC0
#define FLAG_REPE_MASK 0xF0
#define LEN_NON_MASK  0x3F
#define LEN_REPE_MASK 0xF
#define EXT_MASK 0x7F // bit 7 used to store F_EXT flag

#define MAX_LEN_NON  0x7FFFFFF
#define MAX_LEN_REPE 0x3FFFFFF

#ifdef REPET_ALPHA
#define F_ALPH 0x10 // Alpha data duplication flag
#define ALPHA_MASK 0x000000FF // RGBA8888 format

#undef MAX_LEN_REPE
#define MAX_LEN_REPE 0x1FFFFFF
#endif

static int extract(unsigned char *pflag, unsigned int *plen, unsigned int token)
{
	unsigned char mask;
	int ret = ERROR_UNKNOWN;	
	
	if (token & F_REPE) {
		*pflag = token & FLAG_REPE_MASK;
		mask = LEN_REPE_MASK;
	} else {
		*pflag = token & FLAG_NON_MASK;
		mask = LEN_NON_MASK;
	}

	if (!(token & F_EXT)) {
		*plen = token & mask;
		ret = 1;
	} else if (!(token & (F_EXT << 8))) {
		*plen = (token & mask) << 7 | ((token >> 8) & EXT_MASK);
		ret = 2;
	} else if (!(token & (F_EXT << 16))) {
		*plen = (token & mask) << 14 | ((token >> 8) & EXT_MASK) << 7 | ((token >> 16) & EXT_MASK);
		ret = 3;
	} else {
		*plen = (token & mask) << 21 | ((token >> 8) & EXT_MASK) << 14 | ((token >> 16) & EXT_MASK) << 7 | ((token >> 24) & EXT_MASK);
		ret = 4;
	}

	return ret;
}

static int decompress(char *buffer, int length, char *out_buf)
{
	int off = OFF_DATA;
	int ret;
	int i;
	unsigned char flag;
	unsigned int token, len, data;
	unsigned int rle_len;
	int total = 0;

	if (length <= OFF_DATA)
		return ERROR_LENGTH;

	data = TO_WORD(buffer[OFF_HEADER + 3], buffer[OFF_HEADER + 2], buffer[OFF_HEADER + 1], buffer[OFF_HEADER]);
	if (data != RLE_HEADER)
		return ERROR_RLE_HEADER;

	rle_len = TO_WORD(buffer[OFF_LENGTH + 3], buffer[OFF_LENGTH + 2], buffer[OFF_LENGTH + 1], buffer[OFF_LENGTH]);

	while (off < length) {
		/* Parse the token */
		token = buffer[off];
		if (token & F_EXT) {
			token |= buffer[off + 1] << 8;

			if (token & (F_EXT << 8)) {
				token |= buffer[off + 2] << 16;

				if (token & (F_EXT << 16))
					token |= buffer[off + 3] << 24;
			}
		}

		ret = extract(&flag, &len, token);
		pr_debug("  %x: %x flag=%x len=%x %d ret=%d\n\r", off, token, flag, len, len, ret);
		if (ret <= 0)
			return ret;

		if (flag & F_REPE) {
			if (flag & F_WORD) {
				data = TO_WORD(buffer[off + ret + 3], buffer[off + ret + 2], buffer[off + ret + 1], buffer[off + ret]);

				for (i = 0; i < len; i++)
					((unsigned int *)out_buf)[(total >> 2) + i] = data;

				off += ret + 4;
				total += len * 4;
			} else if (flag & F_ALPH) {
				data = buffer[off + ret];

				for (i = 0; i < len; i++) {
					data = TO_WORD(buffer[off + ret + i*3 + 1], buffer[off + ret + i*3 + 2], buffer[off + ret + i*3 + 3], data);

					((unsigned int *)out_buf)[(total >> 2) + i] = data;
				}

				off += ret + 1 + len * 3;
				total += len * 4;
			} else {
				memset(&out_buf[total], buffer[off + ret], len);

				off += ret + 1;
				total += len;
			}
		} else {
			memcpy(&out_buf[total], &buffer[off + ret], len);

			off += ret + len;
			total += len;
		}

		if (total >= rle_len)
			break;
	}

	if (total != rle_len)
		return ERROR_RLE_LENGTH;

	return 0;
}


int rle_decompress(struct image_info *image)
{
	int ret;

	ret = decompress((char *)image->dest, image->length, (char *)RLE_ADDR);
	if (ret) {
		dbg_info("RLE: Decompress error %d\n\r", ret);

		return -3;
	}

	/* The raw image of the program is now ready */
	image->dest = (unsigned char *)RLE_ADDR;

	return 0;
}
