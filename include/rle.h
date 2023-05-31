#ifndef _RLE_H
#define _RLE_H

#define ERROR_UNKNOWN		-1
#define ERROR_LENGTH		-2
#define ERROR_OVERFLOW		-3
#define ERROR_RLE_HEADER	-4
#define ERROR_RLE_LENGTH	-5

int rle_decompress(struct image_info *image);

#endif /* _RLE_H */
