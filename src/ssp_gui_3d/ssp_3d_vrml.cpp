#include "glew.h"
#include "ssp_3d_vrml.h"
#include "SApplication.h"
#include <QtGui>
#include "JlCompress.h"

#define SSP_SLICES		32
#define SSP_STACKS		32
#define VERTEX_BUFNUM	10000

static int s_vboPos = 0;
static CSspVrmlIndexedFaceSet *s_indexedFaceSet = NULL;
static tVertex s_vertex[VERTEX_BUFNUM];
static tTexCoord s_texcoord[VERTEX_BUFNUM];

void CALLBACK PolyLine3DBegin(GLenum type)  
{  
	glBegin(type);  
}  

void CALLBACK PolyLine3DVertex(GLdouble *vertex)  
{  
	glTexCoord2dv(vertex+3);
	glVertex3d(vertex[0]*2.54,vertex[1]*2.54,vertex[2]*2.54);
	//由于SHAPE过多，多边形暂不用VBO
	//s_vertex[s_vboPos].x = vertex[0]*2.54;
	//s_vertex[s_vboPos].y = vertex[1]*2.54;
	//s_vertex[s_vboPos].z = vertex[2]*2.54;
	//s_texcoord[s_vboPos].u = vertex[3];
	//s_texcoord[s_vboPos].v = vertex[4];
	//s_vboPos++;
}

void CALLBACK PolyLine3DEnd()  
{  
	glEnd();  
}

//----------- CSspVrmlIndexedLineSet ------------
CSspVrmlIndexedLineSet::CSspVrmlIndexedLineSet()
{
	m_ssp3DVrml = NULL;
	m_material = NULL;
	m_coordPoint.setAutoDelete(true);
	m_coordIndex.setAutoDelete(true);
	m_braceCnt = 0;
	m_bracketCnt = 0;
}

CSspVrmlIndexedLineSet::~CSspVrmlIndexedLineSet()
{
}

bool CSspVrmlIndexedLineSet::LoadVrmlText(char **pVrmlText)
{
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.Find("Coordinate { point [") >= 0 || sLine.Find("Coordinate {") >= 0)
		{
			if (!LoadCoordPoint(pVrmlText))
				return false;
		}
		else if (sLine.Find("coordIndex [") >= 0)
		{
			if (!LoadCoordIndex(pVrmlText,sLine))
				return false;
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt == 0)
				break;
			else if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedLineSet brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedLineSet bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

bool CSspVrmlIndexedLineSet::LoadCoordPoint(char **pVrmlText)
{
	SString oLine;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		oLine += sLine;
		if (sLine.Find("}") < 0)
			continue;
		else
		{
			int pos = oLine.Find("]");
			if (pos >= 0)
			{
				oLine = oLine.left(pos); 
				pos = oLine.Find("[");
				if (pos >= 0)
					sLine = oLine.right(oLine.length()-(++pos)).trim();
				else
				{
					m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s left coordPoint not found bracket, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
					LOGERROR("%s",m_ssp3DVrml->errTxt.data());
					return false;
				}
			}
			else
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s right coordPoint not found bracket, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}

		int count = SString::GetAttributeCount(sLine, ",");
		for (int i = 0; i < count; i++)
		{
			SString vals = SString::GetIdAttribute(i+1,sLine,",").trim();
			if (vals.isEmpty())
				continue;

			int cnt = SString::GetAttributeCount(vals, " ");
			if (cnt < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s coordPoint error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			tPoint *point = new tPoint();
			point->m_point1 = SString::GetIdAttributeF(1,vals," ");
			point->m_point2 = SString::GetIdAttributeF(2,vals," ");
			point->m_point3 = SString::GetIdAttributeF(3,vals," ");
			m_coordPoint.append(point);
		}
		break;
	}

	return true;
}

bool CSspVrmlIndexedLineSet::LoadCoordIndex(char **pVrmlText,SString text)
{
	SString oLine;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		if (text.Find("]") < 0)
		{
			sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
			if(sLine.length() == 0 || sLine.left(1) == "#")
				continue;
		}
		else
		{
			oLine = text;
			int pos = oLine.Find("[");
			if (pos >= 0)
			{
				oLine = oLine.right(oLine.length()-(++pos)).trim();
				m_bracketCnt++;
			}
		}

		oLine += sLine;
		int pos = oLine.Find("]");
		if (pos < 0)
			continue;
		else
			sLine = oLine.left(pos); 

		int count = SString::GetAttributeCount(sLine, "-1");
		for (int i = 0; i < count; i++)
		{
			SString vals = SString::GetIdAttribute(i+1,sLine,"-1").trim();
			if (vals.isEmpty())
				continue;

			int cnt = SString::GetAttributeCount(vals, " ");
			if (cnt < 2)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s coordIndex error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			else if (cnt >= 2)
			{
				for (int j = 1; j <= cnt; j++)
				{
					tIndex *index = new tIndex();
					index->m_index1 = SString::GetIdAttributeF(j,vals," ");
					m_coordIndex.append(index);
				}
			}
		}
		break;
	}

	return true;
}

//----------- CSspVrmlIndexedFaceSet ------------
CSspVrmlIndexedFaceSet::CSspVrmlIndexedFaceSet()
{
	m_ssp3DVrml = NULL;
	ccw = false;
	solid = false;
	colorPerVertex = false;
	m_coordPoint.setAutoDelete(true);
	m_coordIndex.setAutoDelete(true);
	m_texCoordPoint.setAutoDelete(true);
	m_texCoordIndex.setAutoDelete(true);
	m_normalVector.setAutoDelete(true);
	m_normalIndex.setAutoDelete(true);
	m_braceCnt = 0;
	m_bracketCnt = 0;
	isPolygon = false;

	m_VBOColorId = 0;
	m_VBOVertexId = 0;
	m_VBONormalId = 0;
	m_VBOTexCoordId = 0;
	m_VBOColors = NULL;
	m_VBOVertexs = NULL;
	m_VBONormals = NULL;
	m_VBOTexCoords = NULL;
	m_VBOCount = 0;
}

CSspVrmlIndexedFaceSet::~CSspVrmlIndexedFaceSet()
{
}

bool CSspVrmlIndexedFaceSet::LoadVrmlText(char **pVrmlText)
{
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.Find("ccw ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("ccw ") + strlen("ccw "), sLine.length() - strlen("ccw ")).trim();
			if (val == "TRUE")
				ccw = true;
		}
		else if (sLine.Find("solid ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("solid ") + strlen("solid "), sLine.length() - strlen("solid ")).trim();
			if (val == "TRUE")
				solid = true;
		}
		else if (sLine.Find("colorPerVertex ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("colorPerVertex ") + strlen("colorPerVertex "), sLine.length() - strlen("colorPerVertex ")).trim();
			if (val == "TRUE")
				colorPerVertex = true;
		}
		else if (sLine.Find("TextureCoordinate { point [") >= 0 || sLine.Find("TextureCoordinate {") >= 0)
		{
			if (!LoadTextureCoordPoint(pVrmlText))
				return false;
		}
		else if (sLine.Find("texCoordIndex [") >= 0)
		{
			if (!LoadTexCoordIndex(pVrmlText))
				return false;
		}
		else if (sLine.Find("Coordinate { point [") >= 0 || sLine.Find("Coordinate {") >= 0)
		{
			if (!LoadCoordPoint(pVrmlText))
				return false;
		}
		else if (sLine.Find("coordIndex [") >= 0)
		{
			if (!LoadCoordIndex(pVrmlText))
				return false;
		}
		else if (sLine.Find("Normal { vector [") >= 0)
		{
			if (!LoadNormalVector(pVrmlText))
				return false;
		}
		else if (sLine.Find("normalIndex [") >= 0)
		{
			if (!LoadNormalIndex(pVrmlText))
				return false;
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt == 0)
				break;
			else if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

bool CSspVrmlIndexedFaceSet::LoadCoordPoint(char **pVrmlText)
{
	SString oLine;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		oLine += sLine;
		if (sLine.Find("[") > 0)
		{
			oLine = "";
			continue;
		}
		else if (sLine.Find("}") < 0)
			continue;
		else
		{
			int pos = oLine.Find("]");
			if (pos >= 0)
				sLine = oLine.left(pos); 
			else
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s coordPoint not found bracket, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}

		int count = SString::GetAttributeCount(sLine, ",");
		for (int i = 0; i < count; i++)
		{
			SString vals = SString::GetIdAttribute(i+1,sLine,",").trim();
			if (vals.isEmpty())
				continue;

			int cnt = SString::GetAttributeCount(vals, " ");
			if (cnt < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s coordPoint error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			tPoint *point = new tPoint();
			point->m_point1 = SString::GetIdAttributeF(1,vals," ");
			point->m_point2 = SString::GetIdAttributeF(2,vals," ");
			point->m_point3 = SString::GetIdAttributeF(3,vals," ");
			m_coordPoint.append(point);
			point->m_points[0] = (GLdouble)point->m_point1;
			point->m_points[1] = (GLdouble)point->m_point2;
			point->m_points[2] = (GLdouble)point->m_point3;
		}
		break;
	}

	return true;
}

bool CSspVrmlIndexedFaceSet::LoadCoordIndex(char **pVrmlText)
{
	SString oLine;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.Find(",") < 0)
		{
			oLine += sLine+",";
			isPolygon = true;
		}
		else
		{
			oLine += sLine;
			isPolygon = false;
		}

		int pos = oLine.Find("]");
		if (pos < 0)
			continue;
		else
			sLine = oLine.left(pos); 

		int count = SString::GetAttributeCount(sLine, "-1,");
		for (int i = 0; i < count; i++)
		{
			SString vals = SString::GetIdAttribute(i+1,sLine,"-1,").trim();
			if (vals.isEmpty())
				continue;

			int cnt = SString::GetAttributeCount(vals, ",");
			if (cnt < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s coordIndex error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}

			if (isPolygon)
			{
				for (int j = 1; j < cnt; j++)
				{
					tIndex *index = new tIndex();
					index->m_index1 = SString::GetIdAttributeF(j,vals,",");
					m_coordIndex.append(index);
				}
			}
			else
			{
				tIndex *index = new tIndex();
				index->m_index1 = SString::GetIdAttributeF(1,vals,",");
				index->m_index2 = SString::GetIdAttributeF(2,vals,",");
				index->m_index3 = SString::GetIdAttributeF(3,vals,",");
				m_coordIndex.append(index);
			}
		}
		break;
	}

	return true;
}

bool CSspVrmlIndexedFaceSet::LoadTextureCoordPoint(char **pVrmlText)
{
	int pos = 0;
	SString oLine;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		oLine += sLine;
		if (sLine.Find("[") > 0)
		{
			oLine = "";
			continue;
		}
		else if (sLine.Find("}") < 0)
			continue;
		else
		{
			int pos = oLine.Find("]");
			if (pos >= 0)
				sLine = oLine.left(pos); 
			else
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s textureCoordPoint not found bracket, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}

		int count = SString::GetAttributeCount(sLine, ",");
		for (int i = 0; i < count; i++)
		{
			SString vals = SString::GetIdAttribute(i+1,sLine,",").trim();
			if (vals.isEmpty())
				continue;

			int cnt = SString::GetAttributeCount(vals, " ");
			if (cnt < 2)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s textureCoordPoint error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			tPoint *point = new tPoint();
			point->m_point1 = SString::GetIdAttributeF(1,vals," ");
			point->m_point2 = SString::GetIdAttributeF(2,vals," ");
			m_texCoordPoint.append(point);
			if (m_coordPoint.count() > pos)
			{
				m_coordPoint.at(pos)->m_points[3] = (GLdouble)point->m_point1;
				m_coordPoint.at(pos)->m_points[4] = (GLdouble)point->m_point2;
				pos++;
			}
		}
		break;
	}

	return true;
}

bool CSspVrmlIndexedFaceSet::LoadTexCoordIndex(char **pVrmlText)
{
	SString oLine;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		oLine += sLine;
		int pos = oLine.Find("]");
		if (pos < 0)
			continue;
		else
			sLine = oLine.left(pos); 

		int count = SString::GetAttributeCount(sLine, "-1,");
		for (int i = 0; i < count; i++)
		{
			SString vals = SString::GetIdAttribute(i+1,sLine,"-1,").trim();
			if (vals.isEmpty())
				continue;

			int cnt = SString::GetAttributeCount(vals, ",");
			if (cnt < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s texCoordIndex error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			tIndex *index = new tIndex();
			index->m_index1 = SString::GetIdAttributeF(1,vals,",");
			index->m_index2 = SString::GetIdAttributeF(2,vals,",");
			index->m_index3 = SString::GetIdAttributeF(3,vals,",");
			m_texCoordIndex.append(index);
		}
		break;
	}

	return true;
}

bool CSspVrmlIndexedFaceSet::LoadNormalVector(char **pVrmlText)
{
	SString oLine;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		oLine += sLine;
		if (sLine.Find("}") < 0)
			continue;
		else
		{
			int pos = oLine.Find("]");
			if (pos >= 0)
				sLine = oLine.left(pos); 
			else
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s normalVector not found bracket, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}

		int count = SString::GetAttributeCount(sLine, ",");
		for (int i = 0; i < count; i++)
		{
			SString vals = SString::GetIdAttribute(i+1,sLine,",").trim();
			if (vals.isEmpty())
				continue;

			int cnt = SString::GetAttributeCount(vals, " ");
			if (cnt < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s normalVector error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			tPoint *point = new tPoint();
			point->m_point1 = SString::GetIdAttributeF(1,vals," ");
			point->m_point2 = SString::GetIdAttributeF(2,vals," ");
			point->m_point3 = SString::GetIdAttributeF(3,vals," ");
			m_normalVector.append(point);
		}
		break;
	}

	return true;
}

bool CSspVrmlIndexedFaceSet::LoadNormalIndex(char **pVrmlText)
{
	SString oLine;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		oLine += sLine;
		int pos = oLine.Find("]");
		if (pos < 0)
			continue;
		else
			sLine = oLine.left(pos); 

		int count = SString::GetAttributeCount(sLine, "-1,");
		for (int i = 0; i < count; i++)
		{
			SString vals = SString::GetIdAttribute(i+1,sLine,"-1,").trim();
			if (vals.isEmpty())
				continue;

			int cnt = SString::GetAttributeCount(vals, ",");
			if (cnt < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("IndexedFaceSet\'s normalIndex error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			tIndex *index = new tIndex();
			index->m_index1 = SString::GetIdAttributeF(1,vals,",");
			index->m_index2 = SString::GetIdAttributeF(2,vals,",");
			index->m_index3 = SString::GetIdAttributeF(3,vals,",");
			m_normalIndex.append(index);
		}
		break;
	}

	return true;
}

void CSspVrmlIndexedFaceSet::BuildTriangleVBOs()
{
	float normal[3] = {0,0,0};
	m_VBOCount = m_coordIndex.count();
	if (m_VBOCount)
	{
		m_VBOVertexs = new tVertex[m_VBOCount*3];
		m_VBONormals = new tNormal[m_VBOCount*3];
	}
	if (m_texCoordIndex.count())
		m_VBOTexCoords = new tTexCoord[m_VBOCount*3];

	for (int i = 0,j = 0; i < m_VBOCount; i++,j+=3)
	{
		CSspVrmlIndexedFaceSet::tIndex *index = m_coordIndex.at(i);
		if (ccw)
		{
			m_VBOVertexs[j].x = m_coordPoint.at(index->m_index1)->m_point1;
			m_VBOVertexs[j].y = m_coordPoint.at(index->m_index1)->m_point2;
			m_VBOVertexs[j].z = m_coordPoint.at(index->m_index1)->m_point3;
			m_VBOVertexs[j+1].x = m_coordPoint.at(index->m_index2)->m_point1;
			m_VBOVertexs[j+1].y = m_coordPoint.at(index->m_index2)->m_point2;
			m_VBOVertexs[j+1].z = m_coordPoint.at(index->m_index2)->m_point3;
			m_VBOVertexs[j+2].x = m_coordPoint.at(index->m_index3)->m_point1;
			m_VBOVertexs[j+2].y = m_coordPoint.at(index->m_index3)->m_point2;
			m_VBOVertexs[j+2].z = m_coordPoint.at(index->m_index3)->m_point3;

			m_ssp3DVrml->CalTriNormal(m_coordPoint.at(index->m_index1),m_coordPoint.at(index->m_index2),m_coordPoint.at(index->m_index3),normal);
			m_VBONormals[j].x = m_VBONormals[j+1].x = m_VBONormals[j+2].x = normal[0];
			m_VBONormals[j].y = m_VBONormals[j+1].y = m_VBONormals[j+2].y = normal[1];
			m_VBONormals[j].z = m_VBONormals[j+1].z = m_VBONormals[j+2].z = normal[2];
		}
		else
		{
			m_VBOVertexs[j].x = m_coordPoint.at(index->m_index3)->m_point1;
			m_VBOVertexs[j].y = m_coordPoint.at(index->m_index3)->m_point2;
			m_VBOVertexs[j].z = m_coordPoint.at(index->m_index3)->m_point3;
			m_VBOVertexs[j+1].x = m_coordPoint.at(index->m_index2)->m_point1;
			m_VBOVertexs[j+1].y = m_coordPoint.at(index->m_index2)->m_point2;
			m_VBOVertexs[j+1].z = m_coordPoint.at(index->m_index2)->m_point3;
			m_VBOVertexs[j+2].x = m_coordPoint.at(index->m_index1)->m_point1;
			m_VBOVertexs[j+2].y = m_coordPoint.at(index->m_index1)->m_point2;
			m_VBOVertexs[j+2].z = m_coordPoint.at(index->m_index1)->m_point3;

			m_ssp3DVrml->CalTriNormal(m_coordPoint.at(index->m_index3),m_coordPoint.at(index->m_index2),m_coordPoint.at(index->m_index1),normal);
			m_VBONormals[j].x = m_VBONormals[j+1].x = m_VBONormals[j+2].x = normal[0];
			m_VBONormals[j].y = m_VBONormals[j+1].y = m_VBONormals[j+2].y = normal[1];
			m_VBONormals[j].z = m_VBONormals[j+1].z = m_VBONormals[j+2].z = normal[2];
		}

		if (m_texCoordIndex.count())
		{
			index = m_texCoordIndex.at(i);
			m_VBOTexCoords[j].u = m_texCoordPoint.at(index->m_index1)->m_point1;
			m_VBOTexCoords[j].v = m_texCoordPoint.at(index->m_index1)->m_point2;
			m_VBOTexCoords[j+1].u = m_texCoordPoint.at(index->m_index2)->m_point1;
			m_VBOTexCoords[j+1].v = m_texCoordPoint.at(index->m_index2)->m_point2;
			m_VBOTexCoords[j+2].u = m_texCoordPoint.at(index->m_index3)->m_point1;
			m_VBOTexCoords[j+2].v = m_texCoordPoint.at(index->m_index3)->m_point2;
		}
	}

	glGenBuffersARB(1,&m_VBOVertexId);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_VBOVertexId);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,m_VBOCount*3*3*sizeof(GLfloat),m_VBOVertexs,GL_STATIC_DRAW_ARB);
	delete [] m_VBOVertexs;

	glGenBuffersARB(1,&m_VBONormalId);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_VBONormalId);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,m_VBOCount*3*3*sizeof(GLfloat),m_VBONormals,GL_STATIC_DRAW_ARB);
	delete [] m_VBONormals;

	if (m_texCoordIndex.count())
	{
		glGenBuffersARB(1,&m_VBOTexCoordId);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_VBOTexCoordId);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,m_VBOCount*3*2*sizeof(GLfloat),m_VBOTexCoords,GL_STATIC_DRAW_ARB);
		delete [] m_VBOTexCoords;
	}
}

void CSspVrmlIndexedFaceSet::BuildPolygonVBOs()
{
	GLUtesselator *tess = gluNewTess(); 
	if (!tess) 
		return; 

	gluTessCallback(tess,GLU_TESS_BEGIN,(void(CALLBACK*)())&PolyLine3DBegin);   
	gluTessCallback(tess,GLU_TESS_VERTEX,(void(CALLBACK*)())&PolyLine3DVertex);   
	gluTessCallback(tess,GLU_TESS_END,(void(CALLBACK*)())&PolyLine3DEnd); 

	s_vboPos = 0;
	s_indexedFaceSet = this;
	memset(s_vertex,0,sizeof(tVertex)*VERTEX_BUFNUM);
	memset(s_vertex,0,sizeof(tVertex)*VERTEX_BUFNUM);

	gluTessBeginPolygon(tess,NULL);
	gluTessBeginContour(tess); 
	int cnt = m_coordIndex.count();
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			CSspVrmlIndexedFaceSet::tIndex *index = m_coordIndex.at(i);
			CSspVrmlIndexedFaceSet::tPoint *point1 = m_coordPoint.at(index->m_index1);
			gluTessVertex(tess,point1->m_points,point1->m_points);		
		}
	}
	gluTessEndContour(tess);
	gluTessEndPolygon(tess);
	gluDeleteTess(tess);

	if (s_vboPos <= 0 || s_vboPos > VERTEX_BUFNUM)
		return;

	m_VBOCount = s_vboPos;
	m_VBOVertexs = new tVertex[s_vboPos];
	m_VBOTexCoords = new tTexCoord[s_vboPos];
	for (int i = 0; i < s_vboPos; i++)
	{
		m_VBOVertexs[i].x = s_vertex[i].x;
		m_VBOVertexs[i].y = s_vertex[i].y;
		m_VBOVertexs[i].z = s_vertex[i].z;
		m_VBOTexCoords[i].u = s_texcoord[i].u;
		m_VBOTexCoords[i].v = s_texcoord[i].v;
	}

	glGenBuffersARB(1,&m_VBOVertexId);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_VBOVertexId);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,m_VBOCount*3*sizeof(GLfloat),m_VBOVertexs,GL_STATIC_DRAW_ARB);
	delete [] m_VBOVertexs;

	glGenBuffersARB(1,&m_VBOTexCoordId);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_VBOTexCoordId);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,m_VBOCount*2*sizeof(GLfloat),m_VBOTexCoords,GL_STATIC_DRAW_ARB);
	delete [] m_VBOTexCoords;
}

//----------- CSspVrmlCylinder ------------
CSspVrmlCylinder::CSspVrmlCylinder()
{
	m_ssp3DVrml = NULL;
	m_radius = 0.0;
	m_height = 0.0;
	b_top = true;
	b_bottom = true;
	b_side = true;
	m_braceCnt = 0;
	m_bracketCnt = 0;
}

CSspVrmlCylinder::~CSspVrmlCylinder()
{
}

bool CSspVrmlCylinder::LoadVrmlText(char **pVrmlText)
{
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.Find("radius ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("radius ") + strlen("radius "), sLine.length() - strlen("radius ")).trim();
			if (!val.isEmpty())
				m_radius = SString::toFloat(val);
		}
		else if (sLine.Find("height ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("height ") + strlen("height "), sLine.length() - strlen("height ")).trim();
			if (!val.isEmpty())
				m_height = SString::toFloat(val);
		}
		else if (sLine.Find("top ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("top ") + strlen("top "), sLine.length() - strlen("top ")).trim();
			if (val == "FALSE")
				b_top = false;
		}
		else if (sLine.Find("bottom ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("bottom ") + strlen("bottom "), sLine.length() - strlen("bottom ")).trim();
			if (val == "FALSE")
				b_bottom = false;
		}
		else if (sLine.Find("side ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("side ") + strlen("side "), sLine.length() - strlen("side ")).trim();
			if (val == "FALSE")
				b_side = false;
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt == 0)
				break;
			else if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Cylinder brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Cylinder bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

//----------- CSspVrmlImageTexture ------------
#include "FreeImage.h"
#pragma comment(lib,"FreeImage.lib")

CSspVrmlImageTexture::CSspVrmlImageTexture()
{
	m_ssp3DVrml = NULL;
	m_Texture = 0;
	m_braceCnt = 0;
	m_bracketCnt = 0;
}

CSspVrmlImageTexture::~CSspVrmlImageTexture()
{
}

bool CSspVrmlImageTexture::LoadVrmlText(char **pVrmlText)
{
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.Find("url ") >= 0)
		{
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;

			SString val = sLine.Mid(sLine.Find("url ") + strlen("url "), sLine.length() - strlen("url ")).trim();
			val = val.replace("[","");
			val = val.replace("]","");
			val = val.replace("\"","");
			//SString path = SBASE_SAPP->GetHomePath()+val;
			SString path = SString::toFormat("%s",QCoreApplication::applicationDirPath().toStdString().data())+
				SString::toFormat("/../vrmls/%s/",m_ssp3DVrml->m_record.vrml_name)+val;
			
			FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(path.data());
			FIBITMAP *dib = FreeImage_Load(fifmt,path.data());
			FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType(dib);
			if (fifmt != FIF_PNG || (fifmt == FIF_PNG && type != FIC_RGBALPHA))
			{
				QString s = QString("%1").arg(path.data());
				QImage image(s);
				image = image.convertToFormat(QImage::Format_RGB888);
				image = image.mirrored();

				glGenTextures(1, &m_Texture);
				glBindTexture(GL_TEXTURE_2D, m_Texture);  
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
			}
			else
			{
				BYTE *bits = new BYTE[FreeImage_GetWidth(dib)*FreeImage_GetHeight(dib)*4];
				BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);
				for(int pix=0; pix<FreeImage_GetWidth(dib)*FreeImage_GetHeight(dib); pix++)
				{
					bits[pix*4+0] = pixels[pix*4+2];
					bits[pix*4+1] = pixels[pix*4+1];
					bits[pix*4+2] = pixels[pix*4+0];
					bits[pix*4+3] = pixels[pix*4+3];
				}
				glGenTextures(1, &m_Texture);
				glBindTexture(GL_TEXTURE_2D, m_Texture);  
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
				gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, FreeImage_GetWidth(dib),FreeImage_GetHeight(dib), GL_RGBA,GL_UNSIGNED_BYTE, bits);
				delete bits;
			}
			FreeImage_Unload(dib);
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt == 0)
				break;
			else if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Appearance brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Appearance bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

//----------- CSspVrmlMaterial ------------
CSspVrmlMaterial::CSspVrmlMaterial()
{
	m_diffuseColor[0] = 0.0;
	m_diffuseColor[1] = 0.0;
	m_diffuseColor[2] = 0.0;
	m_specularColor[0] = 0.0;
	m_specularColor[1] = 0.0;
	m_specularColor[2] = 0.0;
	m_ambientIntensity = 0.0;
	m_shininess = 0.0;
	m_transparency = 0.0;
	m_braceCnt = 0;
	m_bracketCnt = 0;
	b_diffuseColor = false;
	b_specularColor = false;
}

CSspVrmlMaterial::~CSspVrmlMaterial()
{
}

bool CSspVrmlMaterial::LoadVrmlText(char **pVrmlText,SString text)
{
	SString sLine = text;
	while(**pVrmlText != '\0')
	{
		if (!text.isEmpty())
			text = "";
		else
		{
			sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
			if(sLine.length() == 0 || sLine.left(1) == "#")
				continue;
		}

		if (sLine.Find("diffuseColor ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("diffuseColor ") + strlen("diffuseColor "), sLine.length() - strlen("diffuseColor ")).trim();
			int count = SString::GetAttributeCount(val, " ");
			if (count < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("Material\'s diffuseColor error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			for (int i = 0; i < 3; i++)
				m_diffuseColor[i] = SString::GetIdAttributeF(i+1,val," ");
			b_diffuseColor = true;
		}
		else if (sLine.Find("specularColor ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("specularColor ") + strlen("specularColor "), sLine.length() - strlen("specularColor ")).trim();
			int count = SString::GetAttributeCount(val, " ");
			if (count < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("Material\'s specularColor error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			for (int i = 0; i < 3; i++)
				m_specularColor[i] = SString::GetIdAttributeF(i+1,val," ");
			b_specularColor = true;
		}
		else if (sLine.Find("ambientIntensity ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("ambientIntensity ") + strlen("ambientIntensity "), sLine.length() - strlen("ambientIntensity ")).trim();
			m_ambientIntensity = SString::toFloat(val);
		}
		else if (sLine.Find("shininess ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("shininess ") + strlen("shininess "), sLine.length() - strlen("shininess ")).trim();
			m_shininess = SString::toFloat(val);
		}
		else if (sLine.Find("transparency ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("transparency ") + strlen("transparency "), sLine.length() - strlen("transparency ")).trim();
			m_transparency = SString::toFloat(val);
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt == 0)
				break;
			else if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Material brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Material bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

//----------- CSspVrmlAppearance ------------
CSspVrmlAppearance::CSspVrmlAppearance()
{
	m_ssp3DVrml = NULL;
	m_material = NULL;
	m_ImageTexture = NULL;
	m_braceCnt = 0;
	m_bracketCnt = 0;
}

CSspVrmlAppearance::~CSspVrmlAppearance()
{
	if (m_material != NULL)
		delete m_material;
	if (m_ImageTexture != NULL)
		delete m_ImageTexture;
}

bool CSspVrmlAppearance::LoadVrmlText(char **pVrmlText)
{
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.Find("material Material{") >= 0 || sLine.Find("material Material {") >= 0)
		{
			SString text;
			if (sLine.Find("material Material{") >= 0)
				text = sLine.right(sLine.length()-sLine.Find("material Material{")-strlen("material Material{")).trim();
			else if (sLine.Find("material Material {") >= 0)
				text = sLine.right(sLine.length()-sLine.Find("material Material {")-strlen("material Material {")).trim();
			if (text.length() == 1 && text.Find("}") >= 0)
				continue;

			m_material = new CSspVrmlMaterial();
			m_material->m_ssp3DVrml = m_ssp3DVrml;
			m_material->m_braceCnt++;
			if (!m_material->LoadVrmlText(pVrmlText,text))
				return false;
			sLine = "";
		}
		else if (sLine.Find("texture ImageTexture{") >= 0 || sLine.Find("texture ImageTexture {") >= 0)
		{
			m_ImageTexture = new CSspVrmlImageTexture();
			m_ImageTexture->m_ssp3DVrml = m_ssp3DVrml;
			m_ImageTexture->m_braceCnt++;
			if (!m_ImageTexture->LoadVrmlText(pVrmlText))
				return false;
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt == 0)
				break;
			else if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Appearance brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Appearance bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

//----------- CSspVrmlShape ------------
CSspVrmlShape::CSspVrmlShape()
{
	m_ssp3DVrml = NULL;
	m_appearance = NULL;
	m_indexedFaceSet = NULL;
	m_indexedLineSet = NULL;
	m_cylinder = NULL;
	m_braceCnt = 0;
	m_bracketCnt = 0;
	b_newAppearance = true;
}

CSspVrmlShape::~CSspVrmlShape()
{
	if (m_appearance != NULL && b_newAppearance)
		delete m_appearance;
	if (m_indexedFaceSet != NULL)
		delete m_indexedFaceSet;
	if (m_cylinder != NULL)
		delete m_cylinder;
	if (m_indexedLineSet != NULL)
		delete m_indexedLineSet;
}

bool CSspVrmlShape::LoadVrmlText(char **pVrmlText)
{
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.Find("appearance USE ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("appearance USE ") + strlen("appearance USE "), sLine.length() - strlen("appearance USE ")).trim();
			m_appearance = m_ssp3DVrml->FindAppearance(val);
			b_newAppearance = false;
		}
		else if (sLine.left(11) == "appearance " && (sLine.Find("Appearance{") >= 0 || sLine.Find("Appearance {") >= 0))
		{
			m_appearance = new CSspVrmlAppearance();
			m_appearance->m_ssp3DVrml = m_ssp3DVrml;
			m_appearance->m_braceCnt++;
			if (sLine.Find("Appearance{") >= 0)
				m_appearance->m_defName = sLine.Mid(sLine.Find("appearance ") + 11, sLine.Find("Appearance{") - sLine.Find("appearance ") - 12).trim();
			else if (sLine.Find("Appearance {") >= 0)
				m_appearance->m_defName = sLine.Mid(sLine.Find("appearance ") + 11, sLine.Find("Appearance {") - sLine.Find("appearance ") - 12).trim();
			if (m_appearance->m_defName.Find("DEF ") >= 0)
				m_appearance->m_defName = m_appearance->m_defName.right(m_appearance->m_defName.length()-4).trim();

			if (!m_appearance->LoadVrmlText(pVrmlText))
				return false;
			m_ssp3DVrml->m_Appearances.append(m_appearance);
		}
		else if (sLine.left(9) == "geometry " && (sLine.Find("IndexedFaceSet{") >= 0 || sLine.Find("IndexedFaceSet {") >= 0))
		{
			m_indexedFaceSet = new CSspVrmlIndexedFaceSet();
			m_indexedFaceSet->m_ssp3DVrml = m_ssp3DVrml;
			m_indexedFaceSet->m_material = m_appearance->m_material;
			m_indexedFaceSet->m_braceCnt++;
			if (sLine.Find("IndexedFaceSet{") >= 0)
			{
				SString def = sLine.Mid(sLine.Find("geometry ") + 9, sLine.Find("IndexedFaceSet{") - sLine.Find("geometry ") - 10).trim();
				if (!def.isEmpty())
					m_indexedFaceSet->m_defName = SString::GetIdAttribute(1,def,"DEF ");
			}
			else if (sLine.Find("IndexedFaceSet {") >= 0)
			{
				SString def = sLine.Mid(sLine.Find("geometry ") + 9, sLine.Find("IndexedFaceSet {") - sLine.Find("geometry ") - 10).trim();
				if (!def.isEmpty())
					m_indexedFaceSet->m_defName = SString::GetIdAttribute(1,def,"DEF ");
			}
			if (m_indexedFaceSet->LoadVrmlText(pVrmlText))
			{
				if (m_ssp3DVrml->vboSupported)
				{
					if (m_indexedFaceSet->isPolygon)
					{
						//m_indexedFaceSet->BuildPolygonVBOs(); //由于SHAPE过多，多边形暂不用VBO
					}
					else
					{
						m_indexedFaceSet->BuildTriangleVBOs();
					}
				}
			}
			else
				return false;
		}
		else if (sLine.left(9) == "geometry " && (sLine.Find("Cylinder {") >= 0 || sLine.Find("Cylinder{") >= 0))
		{
			m_cylinder = new CSspVrmlCylinder();
			m_cylinder->m_ssp3DVrml = m_ssp3DVrml;
			m_cylinder->m_braceCnt++;
			if (!m_cylinder->LoadVrmlText(pVrmlText))
				return false;
		}
		else if (sLine.left(9) == "geometry " && (sLine.Find("IndexedLineSet{") >= 0 || sLine.Find("IndexedLineSet {") >= 0))
		{
			m_indexedLineSet = new CSspVrmlIndexedLineSet();
			m_indexedLineSet->m_ssp3DVrml = m_ssp3DVrml;
			m_indexedLineSet->m_material = m_appearance->m_material;
			m_indexedLineSet->m_braceCnt++;
			if (sLine.Find("IndexedLineSet{") >= 0)
			{
				SString def = sLine.Mid(sLine.Find("geometry ") + 9, sLine.Find("IndexedLineSet{") - sLine.Find("geometry ") - 10).trim();
				if (!def.isEmpty())
					m_indexedLineSet->m_defName = SString::GetIdAttribute(1,def,"DEF ");
			}
			else if (sLine.Find("IndexedLineSet {") >= 0)
			{
				SString def = sLine.Mid(sLine.Find("geometry ") + 9, sLine.Find("IndexedLineSet {") - sLine.Find("geometry ") - 10).trim();
				if (!def.isEmpty())
					m_indexedLineSet->m_defName = SString::GetIdAttribute(1,def,"DEF ");
			}
			if (!m_indexedLineSet->LoadVrmlText(pVrmlText))
				return false;
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt == 0)
				break;
			else if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Shape brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Shape bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

//----------- CSspVrmlChildren ------------
CSspVrmlChildren::CSspVrmlChildren()
{
	m_ssp3DVrml = NULL;
	m_Transforms.setAutoDelete(true);
	m_Shapes.setAutoDelete(true);
	m_braceCnt = 0;
	m_bracketCnt = 0;
}

CSspVrmlChildren::~CSspVrmlChildren()
{
}

bool CSspVrmlChildren::LoadVrmlText(char **pVrmlText)
{
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.left(4) == "DEF " && (sLine.Find("Transform{") >= 0 || sLine.Find("Transform {") >= 0))
		{
			CSspVrmlTransform *transform = new CSspVrmlTransform();
			transform->m_ssp3DVrml = m_ssp3DVrml;
			transform->m_braceCnt++;
			if (sLine.Find("Transform{") >= 0)
				transform->m_defName = sLine.Mid(sLine.Find("DEF ") + 4, sLine.Find("Transform{") - sLine.Find("DEF ") - 5).trim();
			else if (sLine.Find("Transform {") >= 0)
				transform->m_defName = sLine.Mid(sLine.Find("DEF ") + 4, sLine.Find("Transform {") - sLine.Find("DEF ") - 5).trim();
			if (transform->LoadVrmlText(pVrmlText))
				m_Transforms.append(transform);
			else
				return false;
		}
		else if (sLine.Find("Transform{") >= 0 || sLine.Find("Transform {") >= 0)
		{
			CSspVrmlTransform *transform = new CSspVrmlTransform();
			transform->m_ssp3DVrml = m_ssp3DVrml;
			transform->m_braceCnt++;
			if (transform->LoadVrmlText(pVrmlText))
				m_Transforms.append(transform);
			else
				return false;
		}
		else if (sLine.Find("Shape{") >= 0 || sLine.Find("Shape {") >= 0)
		{
			CSspVrmlShape *shape = new CSspVrmlShape();
			shape->m_ssp3DVrml = m_ssp3DVrml;
			shape->m_braceCnt++;
			if (shape->LoadVrmlText(pVrmlText))
				m_Shapes.append(shape);
			else
				return false;
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Children brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt == 0)
				break;
			else if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Children bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

//----------- CSspVrmlTransform ------------
CSspVrmlTransform::CSspVrmlTransform()
{
	m_ssp3DVrml = NULL;
	m_translation[0] = 0.0;
	m_translation[1] = 0.0;
	m_translation[2] = 0.0;
	m_rotation[0] = 0.0;
	m_rotation[1] = 0.0;
	m_rotation[2] = 0.0;
	m_rotation[3] = 0.0;
	m_scale[0] = 1.0;
	m_scale[1] = 1.0;
	m_scale[2] = 1.0;
	m_children = NULL;
	m_braceCnt = 0;
	m_bracketCnt = 0;
}

CSspVrmlTransform::~CSspVrmlTransform()
{
	if (m_children != NULL)
		delete m_children;
}

bool CSspVrmlTransform::LoadVrmlText(char **pVrmlText)
{
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = m_ssp3DVrml->ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.Find("translation ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("translation ") + strlen("translation "), sLine.length() - strlen("translation ")).trim();
			int count = SString::GetAttributeCount(val, " ");
			if (count < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("Transform\'s translation error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			for (int i = 0; i < count; i++)
				m_translation[i] = SString::GetIdAttributeF(i+1,val," ");
		}
		else if (sLine.Find("rotation ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("rotation ") + strlen("rotation "), sLine.length() - strlen("rotation ")).trim();
			int count = SString::GetAttributeCount(val, " ");
			if (count < 4)
			{
				m_ssp3DVrml->errTxt.sprintf("Transform\'s rotation error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			for (int i = 0; i < count; i++)
				m_rotation[i] = SString::GetIdAttributeF(i+1,val," ");
		}
		else if (sLine.Find("scale ") >= 0)
		{
			SString val = sLine.Mid(sLine.Find("scale ") + strlen("scale "), sLine.length() - strlen("scale ")).trim();
			int count = SString::GetAttributeCount(val, " ");
			if (count < 3)
			{
				m_ssp3DVrml->errTxt.sprintf("Transform\'s scale error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
			for (int i = 0; i < count; i++)
				m_scale[i] = SString::GetIdAttributeF(i+1,val," ");
		}
		else if (sLine.Find("children[") >= 0 || sLine.Find("children [") >= 0)
		{
			m_children = new CSspVrmlChildren();
			m_children->m_bracketCnt++;
			m_children->m_ssp3DVrml = m_ssp3DVrml;
			if (!m_children->LoadVrmlText(pVrmlText))
				return false;
		}
		else if (sLine.Find("{") >= 0 || sLine.Find("[") >= 0)
		{
			if (sLine.Find("{") >= 0)
				m_braceCnt++;
			if (sLine.Find("[") >= 0)
				m_bracketCnt++;
		}
		if (sLine.Find("}") >= 0)
		{
			m_braceCnt--;
			if (m_braceCnt == 0)
				break;
			else if (m_braceCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Transform brace error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
		if (sLine.Find("]") >= 0)
		{
			m_bracketCnt--;
			if (m_bracketCnt < 0)
			{
				m_ssp3DVrml->errTxt.sprintf("Transform bracket error, vrmlFile:%s,line:%d",m_ssp3DVrml->sFilePath.data(),m_ssp3DVrml->m_iCurrTextRow);
				LOGERROR("%s",m_ssp3DVrml->errTxt.data());
				return false;
			}
		}
	}

	return true;
}

//----------- CSsp3DVrml ------------
CSsp3DVrml::CSsp3DVrml()
{
	m_iCurrTextRow = 0;
	m_Transforms.setAutoDelete(true);
	m_Appearances.setAutoDelete(false);
	m_braceCnt = 0;
	m_bracketCnt = 0;

	m_quadric = gluNewQuadric();
	gluQuadricNormals(m_quadric, GL_SMOOTH);
	//gluQuadricTexture(m_quadric, GL_TRUE);

	memset(&m_record,0,sizeof(m_record));
	vboSupported = false;
}

CSsp3DVrml::~CSsp3DVrml()
{
}

SString CSsp3DVrml::ReadLine(char* &pText)
{
	char *pNext = strstr(pText,"\n");
	if(pNext == NULL)
		pNext = pText + strlen(pText);
	else
		pNext++;
	SString sLine = SString::toString(pText,pNext-pText).trim();

	pText = pNext;
	m_iCurrTextRow++;
	return sLine;
}

bool CSsp3DVrml::Load(bool bLoadVrml)
{
	SString sPath = SBASE_SAPP->GetHomePath()+SString::toFormat("vrmls\\%s\\",m_record.vrml_name);
	QDir dir = QDir(sPath.data());
	if (!dir.exists())
		bool b = dir.mkdir(sPath.data());
	SString sPathZip = sPath + m_record.vrml_filename;
	
	if(bLoadVrml)
	{
		if (!DB->ReadLobToFile("t_ssp_3d_vrml","vrml_content",SString::toFormat("vrml_id=%d",m_record.vrml_id),sPathZip))
		{
			LOGERROR("下载VRML文件%d-%s失败!",m_record.vrml_id,m_record.vrml_filename);
			return false;
		}
		JlCompress::extractDir(QString("%1").arg(sPathZip.data()), QString("%1").arg(sPath.data()));
	}

	SString sPathFile = sPath + m_record.vrml_name + ".wrl";
	if (!LoadFile(sPathFile))
		return false;

	return true;
}

bool CSsp3DVrml::LoadFile(SString sPath)
{
	sFilePath = sPath;
	SFile f(sFilePath.data());
	if(!f.open(IO_ReadOnly))
		return false;

	int fsize = f.size();
	if(fsize == 0)
		return false;

	m_vrmlText = new BYTE[fsize+1];
	m_vrmlText[fsize] = '\0';
	f.seekBegin();
	f.readBlock(m_vrmlText,fsize);
	f.close();

	BYTE *text = m_vrmlText;
	if (!LoadVrmlText((char**)&text))
	{
		LOGERROR("加载VRML文件失败!原因：%s",errTxt.data());
		delete[] m_vrmlText;
		return false;
	}

	delete[] m_vrmlText;

	return true;
}

bool CSsp3DVrml::LoadVrmlText(char **pVrmlText)
{
	m_iCurrTextRow = 0;
	SString sLine;
	while(**pVrmlText != '\0')
	{
		sLine = ReadLine(*pVrmlText);
		if(sLine.length() == 0 || sLine.left(1) == "#")
			continue;

		if (sLine.left(4) == "DEF " && (sLine.Find("Transform{") >= 0 || sLine.Find("Transform {") >= 0))
		{
			CSspVrmlTransform *transform = new CSspVrmlTransform();
			transform->m_ssp3DVrml = this;
			transform->m_braceCnt++;
			if (sLine.Find("Transform{") >= 0)
				transform->m_defName = sLine.Mid(sLine.Find("DEF ") + 4, sLine.Find("Transform{") - sLine.Find("DEF ") - 5).trim();
			else if (sLine.Find("Transform {") >= 0)
				transform->m_defName = sLine.Mid(sLine.Find("DEF ") + 4, sLine.Find("Transform {") - sLine.Find("DEF ") - 5).trim();
			if (transform->LoadVrmlText(pVrmlText))
				m_Transforms.append(transform);
			else
				return false;
		}
		else if (sLine.Find("Transform{") >= 0 || sLine.Find("Transform {") >= 0)
		{
			CSspVrmlTransform *transform = new CSspVrmlTransform();
			transform->m_ssp3DVrml = this;
			transform->m_braceCnt++;
			if (transform->LoadVrmlText(pVrmlText))
				m_Transforms.append(transform);
			else
				return false;
		}
	}

	return true;
}

bool CSsp3DVrml::Draw()
{
	SString text;

	unsigned long pos;
	CSspVrmlTransform *transf = m_Transforms.FetchFirst(pos);
	while(transf)
	{
		if (!DrawTransforms(transf))
		{
			text.sprintf("Draw transform error, file=%s,name=%s\n",sFilePath.data(),transf->m_defName.data());
			errTxt += text;
			LOGERROR("%s",errTxt.data());
			return false;
		}
		transf = m_Transforms.FetchNext(pos);
	}

	return true;
}

bool CSsp3DVrml::DrawTransforms(CSspVrmlTransform *transform)
{
	SString text;

	glPushMatrix();
	glTranslatef(transform->m_translation[0],transform->m_translation[1],transform->m_translation[2]);
	GLfloat angle = transform->m_rotation[3] * 180.0 / 3.1415926;
	glRotatef(angle,transform->m_rotation[0],transform->m_rotation[1],transform->m_rotation[2]);
	glScaled(transform->m_scale[0],transform->m_scale[1],transform->m_scale[2]);

	unsigned long pos;
	CSspVrmlTransform *transf = transform->m_children->m_Transforms.FetchFirst(pos);
	while(transf)
	{
		if (!DrawTransforms(transf))
		{
			text.sprintf("Draw transform error, file=%s,name=%s\n",sFilePath.data(),transf->m_defName.data());
			errTxt += text;
			LOGERROR("%s",errTxt.data());
			return false;
		}
		transf = transform->m_children->m_Transforms.FetchNext(pos);
	}

	unsigned long pos1;
	CSspVrmlShape *shape = transform->m_children->m_Shapes.FetchFirst(pos1);
	while(shape)
	{
		if (!DrawShape(shape))
		{
			text.sprintf("Draw shape error.\n");
			errTxt += text;
			LOGERROR("%s",errTxt.data());
			return false;
		}
		shape = transform->m_children->m_Shapes.FetchNext(pos1);
	}

	glTranslatef(0.0-transform->m_translation[0],0.0-transform->m_translation[1],0.0-transform->m_translation[2]);
	glPopMatrix();

	return true;
}

bool CSsp3DVrml::DrawShape(CSspVrmlShape *shape)
{
	GLfloat m_light_diffuse[4] = { 0,0,0,1 };
	GLfloat m_light_specular[4] = { 0,0,0,1 };

	glPushMatrix();

	if (shape->m_appearance && shape->m_appearance->m_material && shape->m_appearance->m_material->b_diffuseColor)
	{
		m_light_diffuse[0] = shape->m_appearance->m_material->m_diffuseColor[0];
		m_light_diffuse[1] = shape->m_appearance->m_material->m_diffuseColor[1];
		m_light_diffuse[2] = shape->m_appearance->m_material->m_diffuseColor[2];
		m_light_diffuse[3] = 1.0;
		glLightfv(GL_LIGHT0, GL_DIFFUSE, m_light_diffuse);
	}
	if (shape->m_appearance && shape->m_appearance->m_material && shape->m_appearance->m_material->b_specularColor)
	{
		m_light_specular[0] = shape->m_appearance->m_material->m_specularColor[0];
		m_light_specular[1] = shape->m_appearance->m_material->m_specularColor[1];
		m_light_specular[2] = shape->m_appearance->m_material->m_specularColor[2];
		m_light_specular[3] = 1.0;
		glLightfv(GL_LIGHT0, GL_SPECULAR, m_light_specular);
	}

	if (shape->m_indexedFaceSet)
	{
		GLuint texture = 0;
		if (shape->m_appearance && shape->m_appearance->m_ImageTexture)
			texture = shape->m_appearance->m_ImageTexture->m_Texture;
		if (shape->m_indexedFaceSet->isPolygon)
			DrawIndexedFaceSetFromPolygon(shape->m_indexedFaceSet,texture);
		else
			DrawIndexedFaceSetFromTriangle(shape->m_indexedFaceSet,texture);
	}
	if (shape->m_cylinder)
	{
		DrawCylinder(shape->m_cylinder);
	}
	if (shape->m_indexedLineSet)
	{
		DrawIndexedLineSet(shape->m_indexedLineSet);
	}

	glPopMatrix();
	return true;
}

bool CSsp3DVrml::DrawIndexedFaceSetFromTriangle(CSspVrmlIndexedFaceSet *indexedFaceSet,GLuint texture)
{
	float normal[3] = {0,0,0};
	CSspVrmlIndexedFaceSet::tPoint *point1;
	CSspVrmlIndexedFaceSet::tPoint *point2;
	CSspVrmlIndexedFaceSet::tPoint *point3;
	CSspVrmlIndexedFaceSet::tPoint *texture1;
	CSspVrmlIndexedFaceSet::tPoint *texture2;
	CSspVrmlIndexedFaceSet::tPoint *texture3;
	if (indexedFaceSet->solid)
	{
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}

	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture);
	else
	{
		glDisable(GL_TEXTURE_2D);
		if (indexedFaceSet->m_material)
			glColor3f(indexedFaceSet->m_material->m_diffuseColor[0],indexedFaceSet->m_material->m_diffuseColor[1],indexedFaceSet->m_material->m_diffuseColor[2]);
	}

	int cnt = indexedFaceSet->m_coordIndex.count();
	if (cnt > 0)
	{
		if (vboSupported)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			if (texture)
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glBindBufferARB(GL_ARRAY_BUFFER_ARB,indexedFaceSet->m_VBOVertexId);
			glVertexPointer(3,GL_FLOAT,0,(char*)NULL);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,indexedFaceSet->m_VBONormalId);
			glNormalPointer(GL_FLOAT,0,(char*)NULL);
			if (texture)
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, indexedFaceSet->m_VBOTexCoordId);
				glTexCoordPointer(2,GL_FLOAT,0,(char*)NULL);
			}
			glDrawArrays(GL_TRIANGLES,0,indexedFaceSet->m_VBOCount*3);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			if (texture)
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else
		{
			for (int i = 0; i < cnt; i++)
			{
				glBegin(GL_TRIANGLES);
				CSspVrmlIndexedFaceSet::tIndex *index = indexedFaceSet->m_coordIndex.at(i);
				if (indexedFaceSet->ccw)
				{
					point1 = indexedFaceSet->m_coordPoint.at(index->m_index1);
					point2 = indexedFaceSet->m_coordPoint.at(index->m_index2);
					point3 = indexedFaceSet->m_coordPoint.at(index->m_index3);
				}
				else
				{
					point3 = indexedFaceSet->m_coordPoint.at(index->m_index1);
					point2 = indexedFaceSet->m_coordPoint.at(index->m_index2);
					point1 = indexedFaceSet->m_coordPoint.at(index->m_index3);
				}

				if (texture)
				{
					index = indexedFaceSet->m_texCoordIndex.at(i);
					texture1 = indexedFaceSet->m_texCoordPoint.at(index->m_index1);
					texture2 = indexedFaceSet->m_texCoordPoint.at(index->m_index2);
					texture3 = indexedFaceSet->m_texCoordPoint.at(index->m_index3);
				}

				CalTriNormal(point1,point2,point3,normal);
				glNormal3f(normal[0],normal[1],normal[2]);
				if (texture) glTexCoord2f(texture1->m_point1,texture1->m_point2);
				glVertex3f(point1->m_point1,point1->m_point2,point1->m_point3);
				if (texture) glTexCoord2f(texture2->m_point1,texture2->m_point2);
				glVertex3f(point2->m_point1,point2->m_point2,point2->m_point3);
				if (texture) glTexCoord2f(texture3->m_point1,texture3->m_point2);
				glVertex3f(point3->m_point1,point3->m_point2,point3->m_point3);
				glEnd();
			}
		}
	}

	if (!texture)
	{
		glColor3f(1,1,1);
		glEnable(GL_TEXTURE_2D);
	}

	if (indexedFaceSet->solid)
		glDisable(GL_CULL_FACE);

	return true;
}

bool CSsp3DVrml::DrawIndexedFaceSetFromPolygon(CSspVrmlIndexedFaceSet *indexedFaceSet,GLuint texture)
{
	CSspVrmlIndexedFaceSet::tPoint *point1;
	CSspVrmlIndexedFaceSet::tPoint *texture1;

	if (indexedFaceSet->solid)
	{
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}

	glEnable(GL_BLEND);
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture);
	else
	{
		glDisable(GL_TEXTURE_2D);
		if (indexedFaceSet->m_material)
			glColor3f(indexedFaceSet->m_material->m_diffuseColor[0],indexedFaceSet->m_material->m_diffuseColor[1],indexedFaceSet->m_material->m_diffuseColor[2]);
	}

	//if (!vboSupported) //由于SHAPE过多，多边形暂不用VBO
	//{
	//	if (indexedFaceSet->m_VBOVertexId != 0 && indexedFaceSet->m_VBOTexCoordId != 0)
	//	{
	//		glEnableClientState(GL_VERTEX_ARRAY);
	//		if (texture)
	//			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//		glBindBufferARB(GL_ARRAY_BUFFER_ARB,indexedFaceSet->m_VBOVertexId);
	//		glVertexPointer(3,GL_FLOAT,0,(char*)NULL);
	//		if (texture)
	//		{
	//			glBindBufferARB(GL_ARRAY_BUFFER_ARB,indexedFaceSet->m_VBOTexCoordId);
	//			glTexCoordPointer(2,GL_FLOAT,0,(char*)NULL);
	//		}
	//		glDrawArrays(GL_POLYGON,0,indexedFaceSet->m_VBOCount);

	//		glDisableClientState(GL_VERTEX_ARRAY);
	//		if (texture)
	//			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//	}
	//	else
	//		int a = 0;
	//}
	//else
	//{
		GLUtesselator *tess = gluNewTess(); 
		if (!tess) 
			return false; 

		gluTessCallback(tess,GLU_TESS_BEGIN,(void(CALLBACK*)())&PolyLine3DBegin);   
		gluTessCallback(tess,GLU_TESS_VERTEX,(void(CALLBACK*)())&PolyLine3DVertex);   
		gluTessCallback(tess,GLU_TESS_END,(void(CALLBACK*)())&PolyLine3DEnd); 

		gluTessBeginPolygon(tess,NULL);
		gluTessBeginContour(tess); 
		int cnt = indexedFaceSet->m_coordIndex.count();
		if (cnt > 0)
		{
			for (int i = 0; i < cnt; i++)
			{
				CSspVrmlIndexedFaceSet::tIndex *index = indexedFaceSet->m_coordIndex.at(i);
				point1 = indexedFaceSet->m_coordPoint.at(index->m_index1);
				gluTessVertex(tess,point1->m_points,point1->m_points);		
			}
		}
		gluTessEndContour(tess);  
		gluTessEndPolygon(tess); 
		gluDeleteTess(tess);
	//}

	if (!texture)
	{
		glColor3f(1,1,1);
		glEnable(GL_TEXTURE_2D);
	}
	glDisable(GL_BLEND);

	if (indexedFaceSet->solid)
		glDisable(GL_CULL_FACE);

	return true;
}

bool CSsp3DVrml::DrawCylinder(CSspVrmlCylinder *cylinder)
{
	glTranslatef(0.0,0.0-cylinder->m_height/2.0,0.0);
	glRotatef(-90.0,1,0,0);
	gluCylinder(m_quadric,cylinder->m_radius,cylinder->m_radius,cylinder->m_height,SSP_SLICES,SSP_STACKS);

	return true;
}

bool CSsp3DVrml::DrawIndexedLineSet(CSspVrmlIndexedLineSet *indexLineSet)
{
	CSspVrmlIndexedLineSet::tPoint *point;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LINE_SMOOTH);  
	glHint(GL_LINE_SMOOTH,GL_NICEST);
	if (indexLineSet->m_material)
		glColor3fv(indexLineSet->m_material->m_diffuseColor);
	
	int cnt = indexLineSet->m_coordIndex.count();
	if (cnt > 0)
	{  
		glLineWidth(1);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < cnt; i++)
		{
			point = indexLineSet->m_coordPoint.at(i);
			glVertex3f(point->m_point1*2.54,point->m_point2*2.54,point->m_point3*2.54);
		}
		glEnd();
	}

	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);

	return true;
}

int CSsp3DVrml::CalTriNormal(CSspVrmlIndexedFaceSet::tPoint *point1,
							 CSspVrmlIndexedFaceSet::tPoint *point2,
							 CSspVrmlIndexedFaceSet::tPoint *point3,
							 float normal[3])
{
	GLfloat temp1[3];
	GLfloat temp2[3];
	if (!point1 || !point2 || !point3)
		return -1;

	temp1[0] = point1->m_point1 - point2->m_point1;
	temp1[1] = point1->m_point2 - point2->m_point2;
	temp1[2] = point1->m_point3 - point2->m_point3;
	temp2[0] = point2->m_point1 - point3->m_point1;
	temp2[1] = point2->m_point2 - point3->m_point2;
	temp2[2] = point2->m_point3 - point3->m_point3;

	//计算法线
	normal[0] = temp1[1] * temp2[2] - temp1[2] * temp2[1];
	normal[1] = temp1[0] * temp2[2] - temp1[2] * temp2[0];
	normal[2] = temp1[0] * temp2[1] - temp1[1] * temp2[0];

	//法线单位化
	GLfloat length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	if (length == 0.0f)
		length = 1.0f;

	normal[0] /= length;
	normal[1] /= length;
	normal[2] /= length;

	return 0;
}

CSspVrmlAppearance* CSsp3DVrml::FindAppearance(SString val)
{
	unsigned long pos;
	CSspVrmlAppearance *appearance = m_Appearances.FetchFirst(pos);
	while(appearance)
	{
		if (appearance->m_defName == val)
			return appearance;
		
		appearance = m_Appearances.FetchNext(pos);
	}

	return NULL;
}
