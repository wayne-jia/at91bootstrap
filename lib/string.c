// Copyright (C) 2006 Microchip Technology Inc. and its subsidiaries
//
// SPDX-License-Identifier: MIT

#include "string.h"
#include "common.h"

//#define ENABLE_MEMSETX

#ifdef __GNUC__
  #ifdef CONFIG_THUMB
    #define MAX_CHUNK_SIZE 48 /* By bytes */
  #else
    #define MAX_CHUNK_SIZE 64
  #endif
#endif

#ifdef MAX_CHUNK_SIZE
  #define CHUNK_SIZE (MAX_CHUNK_SIZE / 4)
#else
  #define CHUNK_SIZE 8
#endif

#define C_SIZE (CHUNK_SIZE / sizeof(unsigned long))

struct chunk {
	unsigned long val[C_SIZE];
};

static const struct chunk c_zero = {{0}};

void *memcpy(void *dst, const void *src, int cnt)
{
	char *d;
	const char *s;
	const struct chunk *csrc = (const struct chunk *)src;
	struct chunk *cdst = (struct chunk *)dst;

	/* Check if the addresses are word-aligned */
	if (!(((unsigned int)src | (unsigned int)dst) & 0x3)) {
		while (cnt >= CHUNK_SIZE) {
			*cdst++ = *csrc++;
			cnt -= CHUNK_SIZE;
		}
	}

	d = (char *) cdst;
	s = (const char *) csrc;

	while (cnt--)
		*d++ = *s++;

	return dst;
}

void *memset(void *dst, int val, int cnt)
{
	char *d;
	const struct chunk *cpattern;
	struct chunk *cdst = (struct chunk *)dst;

	/* Check if the address is word-aligned */
	if (!val && (cnt >= CHUNK_SIZE) &&
		 !((unsigned long)dst & 0x3)) {
		cpattern = &c_zero;

		while (cnt >= CHUNK_SIZE) {
			*cdst++ = *cpattern;
			cnt -= CHUNK_SIZE;
		}
	}

	d = (char *) cdst;

	while (cnt--)
		*d++ = (char)val;

	return dst;
}

#ifdef ENABLE_MEMSETX
void *memset2(void *dst, int val, int cnt)
{
	char *d = (char *)dst;
	int i;
	const struct chunk *cpattern;
	struct chunk *cdst = (struct chunk *)dst;
	struct chunk c_tmp;

	i = (int)dst & 0x3;
	if (i)
		i = 4 - i;

	if ((cnt - i) >= CHUNK_SIZE) {
		cnt -= i;
		while (i--)
			*d++ = (char)val;

		cdst = (struct chunk *)d;

		if (!val) {
			cpattern = &c_zero;
		} else {
			val &= 0xff;
			val |= val << 8;
			val |= val << 16;

			for (i = 0; i < C_SIZE; i++)
				c_tmp.val[i] = val;

			cpattern = &c_tmp;
		}

		while (cnt >= CHUNK_SIZE) {
			*cdst++ = *cpattern;
			cnt -= CHUNK_SIZE;
		}
	}

	d = (char *) cdst;

	while (cnt--)
		*d++ = (char)val;

	return dst;
}

void *memset4(void *dst, long val, int cnt)
{
	long *d;
	int i;
	const struct chunk *cpattern;
	struct chunk *cdst = (struct chunk *)dst;
	struct chunk c_tmp;

	/* Check if the address is word-alighed */
	if ((unsigned long)dst & 0x3)
		return 0;

	if (cnt >= C_SIZE) {
		if (!val) {
			cpattern = &c_zero;
		} else {
			for (i = 0; i < C_SIZE; i++)
				c_tmp.val[i] = val;

			cpattern = &c_tmp;
		}

		while (cnt >= C_SIZE) {
			*cdst++ = *cpattern;
			cnt -= C_SIZE;
		}
	}

	d = (long *) cdst;

	while (cnt--)
		*d++ = val;

	return dst;
}
#endif

int memcmp(const void *dst, const void *src, unsigned int cnt)
{
	const char *d = (const char *)dst;
	const char *s = (const char *)src;
	int r = 0;

	while (cnt-- && (r = *d++ - *s++) == 0) ;

	return r;
}

unsigned int strlen(const char *str)
{
	int i = 0;

	while (str[i++] != '\0') ;

	return i - 1;
}

char *strcpy(char *dst, const char *src)
{
	char *bak = dst;

	while ((*dst++ = *src++) != '\0') ;

	return bak;
}

char *strcat(char *dst, const char *src)
{
	char *p = dst;

	while (*dst != '\0')
		dst++;

	while ((*dst++ = *src++) != '\0') ;

	return p;
}

int strcmp(const char *p1, const char *p2)
{
	unsigned char c1, c2;

	while (1) {
		c1 = *p1++;
		c2 = *p2++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
	}

	return 0;
}

int strncmp(const char *p1, const char *p2, unsigned int cnt)
{
	unsigned char c1, c2;

	while (cnt--) {
		c1 = *p1++;
		c2 = *p2++;

		if (c1 != c2)
			return c1 < c2 ? -1 : 1;

		if (!c1)
			break;
	}

	return 0;
}

char *strchr(const char *s, int c)
{
	for (; *s != (char) c; ++s)
		if (*s == '\0')
			return NULL;

	return (char *)s;
}

/* NOTE: This is the simple-minded O(len(s1) * len(s2)) worst-case approach. */

char *strstr(const char *s1, const char *s2)
{
	register const char *s = s1;
	register const char *p = s2;

	do {
		if (!*p) {
			return (char *) s1;;
		}
		if (*p == *s) {
			++p;
			++s;
		} else {
			p = s2;
			if (!*s) {
				return NULL;
			}
			s = ++s1;
		}
	} while (1);
}

void *memchr(void *src, int val, unsigned int cnt)
{
	char *p = NULL;
	char *s = (char *)src;

	while (cnt) {
		if (*s == val) {
			p = s;
			break;
		}
		s++;
		cnt--;
	}

	return p;
}

void *memmove(void *dst, const void *src, unsigned int cnt)
{
	char *p, *s;

	if (dst <= src) {
		p = (char *)dst;
		s = (char *)src;
		while (cnt--)
			*p++ = *s++;
		}
	else {
		p = (char *)dst + cnt;
		s = (char *)src + cnt;
		while (cnt--)
			*--p = *--s;
		}

	return dst;
}
