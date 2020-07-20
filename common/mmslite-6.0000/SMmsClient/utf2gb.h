#ifndef _WIN32
#include <stdio.h>
#include <iconv.h>
#include <string.h>
#include <errno.h>
#else
#include <windows.h>
#endif

#define UTF8_ONE_START (0x0001)
#define UTF8_ONE_END (0x0007f)
#define UTF8_TWO_START (0x0080)
#define UTF8_TWO_END (0x07ff)
#define UTF8_THERE_START (0x0800)
#define UTF8_THERE_END (0xffff)

#ifdef __cplusplus
extern "C" {
#endif

	unsigned int utf16_string_len(unsigned short * pUTF16Start);
	void UTF16ToUTF8(unsigned short * pUTF16Start,unsigned short * pUTF16End,
		unsigned char * pUTF8Start,unsigned char * pUTF8End);
	int convert_utf8_gb2312_ex(char * inbuf,size_t inlen,char * outbuf,size_t outlen);
	int UTF16ToGB2312(char * inData,char * outData);

#ifdef __cplusplus
}
#endif
