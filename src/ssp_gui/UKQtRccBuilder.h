/**
 *
 * 文 件 名 : UKQtRccBuilder.h
 * 创建日期 : 2017-10-10 10:55
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 实现RCC文件的动态生成功能，可从qrc文件转换，也可以自行添加文件或缓冲区方式添加图片资源
 *            输出有两种形式：输出到rcc文件和输出rcc缓冲区，这两种形式均可以使用registerResource函数加载
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-10-10	邵凯田　创建文件
 *
 **/

#ifndef __UK_QT_RCC_BUILDER_H__
#define __UK_QT_RCC_BUILDER_H__

#include "SApi.h"
#include "SFile.h"
#include "SString.h"
#include "SList.h"

class CUkQtRccBuilder
{
public:
	struct stuFile
	{
		stuFile(SString alias,unsigned char *buf,int len,bool bFree=true)
		{
			img_alias = alias;
			img_free = bFree;
			img_buf = buf;
			img_len = len;
			nameOffset = dataOffset = 0;
		}
		~stuFile()
		{
			if(img_free)
			{
				delete[] img_buf;
			}
			img_buf = NULL;
			img_len = 0;
		}
		SString img_alias;//别名
		unsigned char* img_buf;//图片内容
		int img_len;//图片长度
		bool img_free;//是否需要负责释放缓冲区

		int nameOffset;
		int dataOffset;
	};
	CUkQtRccBuilder();
	~CUkQtRccBuilder();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置语言
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:05
	// 参数说明:  @lang为语方类型
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetLang(SString lang){m_lang = lang;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置前缀
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:05
	// 参数说明:  @prefix为前缀名称
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetPrefix(SString prefix){m_prefix = prefix;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从qrc文件加载RCC
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:15
	// 参数说明:  qrc_file为qrc本地文件名
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool LoadByQrc(SString qrc_file);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从本地文件添加一个图片
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:06
	// 参数说明:  @alias为图片文件别名
	//         :  @sFileName为本地文件名称
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool AddImage(SString alias,SString sFileName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从缓冲区添加一个图片
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:06
	// 参数说明:  @alias为图片文件另外
	//         :  @buf为文件缓冲区指针 
	//         :  @len为文件内容长度
	//         :  @free_buf表示是否需要释放指定的缓冲区
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool AddImage(SString alias,unsigned char *buf,int len,bool free_buf=true);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  保存RCC内容到缓冲区
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:13
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool SaveRccToBuffer();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  保存RCC内容到本地文件
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:13
	// 参数说明:  sPathFile为本地RCC文件名全路径
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool SaveRccToFile(SString sPathFile);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取RCC缓冲区地址
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:12
	// 参数说明:  void
	// 返 回 值:  NULL表示尚未生成缓冲区
	//////////////////////////////////////////////////////////////////////////
	inline unsigned char* GetRccBuffer(){return m_pRccBuffer;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取RCC缓冲区长度
	// 作    者:  邵凯田
	// 创建时间:  2017-10-10 11:12
	// 参数说明:  void
	// 返 回 值:  0表示尚未生成缓冲区
	//////////////////////////////////////////////////////////////////////////
	inline int GetRccBufferLen(){return m_iRccBufferLen;};

private:
	void write(unsigned char* pBuf,int &pos,void* pBlockBuf,int iBlockLen);
	SString m_lang;//语言
	SString m_prefix;//前缀
	unsigned char* m_pRccBuffer;//RCC缓冲区位置
	int m_iRccBufferLen;//RCC缓冲区大小
	SPtrList<stuFile> m_Files;
};

#endif//__UK_QT_RCC_BUILDER_H__
