#include "utf2gb.h"

unsigned int utf16_string_len(unsigned short * pUTF16Start)
{
	int len = 0;
	unsigned short * pTempUTF16 = pUTF16Start;

	if (pTempUTF16 == 0)
		return 0;

	while (1) 
	{
		if (*pTempUTF16 == 0x00)
			break;
		++len;
		pTempUTF16++;
	}

	return len;
}

void UTF16ToUTF8(unsigned short * pUTF16Start,unsigned short * pUTF16End,
	unsigned char * pUTF8Start,unsigned char * pUTF8End)
{
	unsigned short * pTempUTF16 = pUTF16Start;
	unsigned char  * pTempUTF8 = pUTF8Start;

	while(pTempUTF16 <= pUTF16End)
	{
		if( *pTempUTF16 <= UTF8_ONE_END && pTempUTF8 +1 < pUTF8End)
		{
			*pTempUTF8++=(unsigned char)*pTempUTF16;
		}
		else if(*pTempUTF16 >= UTF8_TWO_START && *pTempUTF16<=UTF8_TWO_END && pTempUTF8+2<pUTF8End)
		{
			*pTempUTF8 ++=(*pTempUTF16>>6)|0xC0;
			*pTempUTF8 ++=(*pTempUTF16 & 0x3f) | 0x80;
		}
		else if(*pTempUTF16 >= UTF8_THERE_START && *pTempUTF16 <= UTF8_THERE_END &&pTempUTF8+3 < pUTF8End)
		{
			*pTempUTF8++=(*pTempUTF16>>12)|0xE0;
			*pTempUTF8++=((*pTempUTF16>>6)&0x3f)|0x80;
			*pTempUTF8++=(*pTempUTF16&0x3f)|0x80;
		}
		else
			break;

		pTempUTF16++;
		if (*pTempUTF16 == 0x00)
			break;
	}

	*pTempUTF8=0;
}

int convert_utf8_gb2312_ex(char * inbuf,size_t inlen,char * outbuf,size_t outlen)
{
	int ret = 0;
	char **pin = &inbuf;
	char **pout = &outbuf;

#ifdef WIN32
	{
		WCHAR* strSrc;
		TCHAR *szRes = (TCHAR*)outbuf;

		int i = MultiByteToWideChar(CP_UTF8, 0, inbuf, -1, NULL, 0);
		strSrc = (WCHAR*)malloc(sizeof(WCHAR) * (i+1));
		memset(strSrc, 0, sizeof(WCHAR) * (i+1));
		MultiByteToWideChar(CP_UTF8, 0, inbuf, -1, strSrc, i);

		i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, strSrc, -1, (LPSTR)szRes, i, NULL, NULL);

		free(strSrc);
	}
#else
	{
		iconv_t cd=0;
		memset(outbuf,0,outlen);
		//cd = iconv_open("utf-8//IGNORE", "gb18030//IGNORE");
		cd = iconv_open("gb18030//IGNORE","utf-8//IGNORE");
		//cd = iconv_open("utf-8", "gb18030");
		printf("begin to iconv(%d,%d,%d)\n\n",cd,inlen,outlen);
		int ret = iconv(cd,pin,(size_t *)&inlen,pout,(size_t *)&outlen);
		if(ret == -1)
		{
			printf("iconv error!cd=%d errno=%d, src(%d)=%s, desc(%d)=%s\n",
				cd,errno,inlen,inbuf,outlen,outbuf);
		}
		iconv_close(cd);
	}
#endif

	return ret;
}

int UTF16ToGB2312(char * inData,char * outData)
{
	int  ret = 0;
	char temp[512] = {'\0'};

	int src_len = utf16_string_len((unsigned short *)inData);
	//if (src_len <= 0 || src_len >= 256) 
	//	src_len = 10;
	if (src_len < 0)
		src_len = 0;
	if (src_len > 256) 
		src_len = 256;

	UTF16ToUTF8((unsigned short*)inData,(unsigned short*)inData + src_len*2,
		(unsigned char*)temp,(unsigned char*)temp + src_len*4);
	ret = convert_utf8_gb2312_ex(temp,src_len,outData,src_len*2);
	return ret;
}
