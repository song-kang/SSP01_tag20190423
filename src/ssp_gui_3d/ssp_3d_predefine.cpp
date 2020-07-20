/**
 *
 * 文 件 名 : ssp_3d_predefine.cpp
 * 创建日期 : 2017-3-2 8:57
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 预定义对象封装
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-2	邵凯田　创建文件
 *
 **/

#include "ssp_3d_predefine.h"
#include "SApplication.h"

#define BITMAP_ID		0x4D42

CSsp3DPreDefineObj::CSsp3DPreDefineObj(t_ssp_3d_predefine_obj *p/*=NULL*/)
{
	if(p != NULL)
		memcpy(&m_record,p,sizeof(m_record));
	else
		memset(&m_record,0,sizeof(m_record));
	m_pTexture = NULL;
}

CSsp3DPreDefineObj::~CSsp3DPreDefineObj()
{
	if(m_record.draw_type == 5)
	{
		GLuint m_Texures = m_record.texture_id;
		glDeleteTextures(1,&m_Texures);
	}
}

bool CSsp3DPreDefineObj::LoadTexture()
{
	SString sPath = SBASE_SAPP->GetHomePath()+"textures\\"+m_record.obj_name+".bmp";
	if (!LoadRGBATexture(sPath))
		return false;

	return true;
}

bool CSsp3DPreDefineObj::LoadRGBATexture(SString path)
{
	BITMAPINFOHEADER bitmapInfoHeader;    
	unsigned char*   bitmapData = NULL;
	unsigned char color[3] = { 0xc8,0xc8,0xc8 };

	bitmapData = LoadBMPFile(path.data(),&bitmapInfoHeader,color,true);
	if (!bitmapData)
		return false;

	GLuint m_Texture;
	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);      
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
	glTexImage2D(GL_TEXTURE_2D,  
		0,							//mipmap层次(通常为0，表示最上层)       
		GL_RGBA,					//红、绿、蓝、alpha数据      
		bitmapInfoHeader.biWidth,	//纹理宽带，必须是n，若有边框+2       
		bitmapInfoHeader.biHeight, //纹理高度，必须是n，若有边框+2       
		0,							//边框(0=无边框, 1=有边框)       
		GL_RGBA,					//bitmap数据的格式      
		GL_UNSIGNED_BYTE,			//每个颜色数据的类型      
		bitmapData);				//bitmap数据指针

	m_record.texture_id = m_Texture;

	return true;
}

unsigned char* CSsp3DPreDefineObj::LoadBMPFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader,unsigned char *backgroundColor,bool bAlpha)
{
	FILE *filePtr;
	int imageIdx = 0;   
	BITMAPFILEHEADER bitmapFileHeader;    //bitmap文件头      
	unsigned char    *bitmapImage;        //bitmap图像数据      

	filePtr = fopen(filename, "rb");  
	if (filePtr == NULL) 
	{  
		LOGERROR("纹理文件[%s]打开失败。",filename);
		return NULL;  
	}  

	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);        
	if (bitmapFileHeader.bfType != BITMAP_ID) 
	{  
		LOGERROR("纹理文件[%s]不是BMP文件。",filename);
		return NULL;  
	}  

	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (bitmapInfoHeader->biSizeImage == 0 || bitmapInfoHeader->biSizeImage+54 != bitmapFileHeader.bfSize)
		bitmapInfoHeader->biSizeImage = bitmapFileHeader.bfSize-54;

	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);       
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];     
	if (!bitmapImage) 
	{  
		LOGERROR("加载纹理文件[%s]失败，原因：内存不足。",filename); 
		return NULL;  
	}  

	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);      
	if (bitmapImage == NULL) 
	{  
		LOGERROR("读取纹理文件[%s]失败，原因：内存不足。",filename); 
		return NULL;  
	} 

	unsigned char* bitmapData = NULL;
	bitmapData = new unsigned char[bitmapInfoHeader->biSizeImage / 3 * 4];  
	if (!bitmapData) 
	{  
		LOGERROR("创建alpha纹理[%s]内存失败，原因：内存不足。",filename); 
		return NULL;  
	}

	int count = 0;  
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) 
	{  
		bitmapData[count] = bitmapImage[imageIdx + 2];  
		bitmapData[count + 1] = bitmapImage[imageIdx + 1];  
		bitmapData[count + 2] = bitmapImage[imageIdx];  
		if (bAlpha && bitmapData[count] == backgroundColor[0] && bitmapData[count + 1] == backgroundColor[1] && bitmapData[count + 2] == backgroundColor[2])
		{  
			bitmapData[count + 3] = 0;  
		}  
		else 
		{
			bitmapData[count + 3] = 255; 
		}

		count += 4;  
	}  

	delete bitmapImage;
	fclose(filePtr);

	return bitmapData;  
}

CSsp3DPreDefine::CSsp3DPreDefine(t_ssp_3d_predefine *p/*=NULL*/)
{
	if(p != NULL)
		memcpy(&m_record,p,sizeof(m_record));
	else
		memset(&m_record,0,sizeof(m_record));
	m_PreObjs.setAutoDelete(true);
}

CSsp3DPreDefine::~CSsp3DPreDefine()
{

}