#ifndef __SSP_3D_VRML_H__
#define __SSP_3D_VRML_H__

#include "SString.h"
#include "ssp_3d_inc.h"
#include "GLU.h"
#include "glext.h"
#include "glut.h"
#include "ssp_gui_3d_inc.h"

class CSsp3DVrml;
class CSspVrmlTransform;
class CSspVrmlMaterial;

//----------- CSspVrmlIndexedLineSet ------------
class SSP_GUI_3D_EXPORT CSspVrmlIndexedLineSet
{
public:
	struct SSP_GUI_3D_EXPORT tPoint
	{
		tPoint() { m_point1 = 0;m_point2 = 0;m_point3 = 0;}
		GLfloat m_point1;
		GLfloat m_point2;
		GLfloat m_point3;
	};

	struct SSP_GUI_3D_EXPORT tIndex
	{
		tIndex() { m_index1 = 0;m_index2 = 0;m_index3 = 0; }
		GLuint m_index1;
		GLuint m_index2;
		GLuint m_index3;
	};

public:
	CSspVrmlIndexedLineSet();
	~CSspVrmlIndexedLineSet();

	bool LoadVrmlText(char **pVrmlText);

public:
	CSsp3DVrml *m_ssp3DVrml;
	CSspVrmlMaterial *m_material;

	SString m_defName;
	SPtrList<tPoint> m_coordPoint;
	SPtrList<tIndex> m_coordIndex;
	int m_braceCnt;
	int m_bracketCnt;

private:
	bool LoadCoordPoint(char **pVrmlText);
	bool LoadCoordIndex(char **pVrmlText,SString text);
};

//----------- CSspVrmlIndexedFaceSet ------------
struct SSP_GUI_3D_EXPORT tVertex
{
	tVertex() { x = 0.0; y = 0.0; z = 0.0; }
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct SSP_GUI_3D_EXPORT tNormal
{
	tNormal() { x = 0.0; y = 0.0; z = 0.0; }
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct SSP_GUI_3D_EXPORT tColor
{
	tColor() { x = 0.0; y = 0.0; z = 0.0; }
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct SSP_GUI_3D_EXPORT tTexCoord
{
	tTexCoord() { u = 0.0; v = 0.0; }
	GLfloat u;
	GLfloat v;
};

class SSP_GUI_3D_EXPORT CSspVrmlIndexedFaceSet
{
public:
	struct SSP_GUI_3D_EXPORT tPoint
	{
		tPoint() { m_point1 = 0;m_point2 = 0;m_point3 = 0; m_points[0] = 0; m_points[1] = 0; m_points[2] = 0; m_points[3] = 0; m_points[4] = 0;}
		GLfloat m_point1;
		GLfloat m_point2;
		GLfloat m_point3;
		GLdouble m_points[5];
	};

	struct SSP_GUI_3D_EXPORT tIndex
	{
		tIndex() { m_index1 = 0;m_index2 = 0;m_index3 = 0; }
		GLuint m_index1;
		GLuint m_index2;
		GLuint m_index3;
	};

public:
	CSspVrmlIndexedFaceSet();
	~CSspVrmlIndexedFaceSet();

	bool LoadVrmlText(char **pVrmlText);
	void BuildTriangleVBOs();
	void BuildPolygonVBOs();

public:
	CSsp3DVrml *m_ssp3DVrml;
	CSspVrmlMaterial *m_material;

	SString m_defName;
	bool ccw;
	bool solid;
	bool colorPerVertex;
	SPtrList<tPoint> m_coordPoint;
	SPtrList<tIndex> m_coordIndex;
	SPtrList<tPoint> m_texCoordPoint;
	SPtrList<tIndex> m_texCoordIndex;
	SPtrList<tPoint> m_normalVector;
	SPtrList<tIndex> m_normalIndex;
	int m_braceCnt;
	int m_bracketCnt;
	bool isPolygon;

	GLuint m_VBOColorId;
	tColor *m_VBOColors;
	GLuint m_VBOVertexId;
	tVertex *m_VBOVertexs;
	GLuint m_VBONormalId;
	tNormal *m_VBONormals;
	GLuint m_VBOTexCoordId;
	tTexCoord *m_VBOTexCoords;
	int m_VBOCount;

private:
	bool LoadCoordPoint(char **pVrmlText);
	bool LoadCoordIndex(char **pVrmlText);
	bool LoadTextureCoordPoint(char **pVrmlText);
	bool LoadTexCoordIndex(char **pVrmlText);
	bool LoadNormalVector(char **pVrmlText);
	bool LoadNormalIndex(char **pVrmlText);

};

//----------- CSspVrmlCylinder ------------
class SSP_GUI_3D_EXPORT CSspVrmlCylinder
{
public:
	CSspVrmlCylinder();
	~CSspVrmlCylinder();

	bool LoadVrmlText(char **pVrmlText);

public:
	CSsp3DVrml *m_ssp3DVrml;

	GLfloat m_radius;
	GLfloat m_height;
	bool b_top;
	bool b_bottom;
	bool b_side;
	int m_braceCnt;
	int m_bracketCnt;
};

//----------- CSspVrmlImageTexture ------------
class SSP_GUI_3D_EXPORT CSspVrmlImageTexture
{
public:
	CSspVrmlImageTexture();
	~CSspVrmlImageTexture();

	bool LoadVrmlText(char **pVrmlText);

public:
	CSsp3DVrml *m_ssp3DVrml;

	GLuint m_Texture;
	int m_braceCnt;
	int m_bracketCnt;
};

//----------- CSspVrmlMaterial ------------
class SSP_GUI_3D_EXPORT CSspVrmlMaterial
{
public:
	CSspVrmlMaterial();
	~CSspVrmlMaterial();

	bool LoadVrmlText(char **pVrmlText,SString text);

public:
	CSsp3DVrml *m_ssp3DVrml;

	GLfloat m_diffuseColor[3];
	GLfloat m_specularColor[3];
	GLfloat m_ambientIntensity;
	GLfloat m_shininess;
	GLfloat m_transparency;
	int m_braceCnt;
	int m_bracketCnt;
	bool b_diffuseColor;
	bool b_specularColor;
};

//----------- CSspVrmlAppearance ------------
class SSP_GUI_3D_EXPORT CSspVrmlAppearance
{
public:
	CSspVrmlAppearance();
	~CSspVrmlAppearance();

	bool LoadVrmlText(char **pVrmlText);

public:
	CSsp3DVrml *m_ssp3DVrml;

	SString m_defName;
	CSspVrmlMaterial *m_material;
	CSspVrmlImageTexture *m_ImageTexture;
	int m_braceCnt;
	int m_bracketCnt;
};

//----------- CSspVrmlShape ------------
class SSP_GUI_3D_EXPORT CSspVrmlShape
{
public:
	CSspVrmlShape();
	~CSspVrmlShape();

	bool LoadVrmlText(char **pVrmlText);

public:
	CSsp3DVrml *m_ssp3DVrml;

	CSspVrmlAppearance *m_appearance;
	CSspVrmlIndexedFaceSet *m_indexedFaceSet;
	CSspVrmlIndexedLineSet *m_indexedLineSet;
	CSspVrmlCylinder *m_cylinder;
	int m_braceCnt;
	int m_bracketCnt;
	bool b_newAppearance;
};

//----------- CSspVrmlChildren ------------
class SSP_GUI_3D_EXPORT CSspVrmlChildren
{
public:
	CSspVrmlChildren();
	~CSspVrmlChildren();

	bool LoadVrmlText(char **pVrmlText);

public:
	CSsp3DVrml *m_ssp3DVrml;

	SPtrList<CSspVrmlTransform> m_Transforms;
	SPtrList<CSspVrmlShape> m_Shapes;
	int m_braceCnt;
	int m_bracketCnt;
};

//----------- CSspVrmlTransform ------------
class SSP_GUI_3D_EXPORT CSspVrmlTransform
{
public:
	CSspVrmlTransform();
	~CSspVrmlTransform();

	bool LoadVrmlText(char **pVrmlText);

public:
	CSsp3DVrml *m_ssp3DVrml;

	SString m_defName;
	GLfloat m_translation[3];
	GLfloat m_rotation[4];
	GLfloat m_scale[3];
	CSspVrmlChildren *m_children;
	int m_braceCnt;
	int m_bracketCnt;

};

//----------- CSsp3DVrml ------------
class SSP_GUI_3D_EXPORT CSsp3DVrml
{
public:
	CSsp3DVrml();
	~CSsp3DVrml();

	bool Load(bool bLoadVrml);
	bool LoadFile(SString sPath);
	bool LoadVrmlText(char **pVrmlText);
	SString ReadLine(char* &pText);

	bool Draw();
	bool DrawTransforms(CSspVrmlTransform *transform);
	bool DrawShape(CSspVrmlShape *shape);
	bool DrawIndexedFaceSetFromTriangle(CSspVrmlIndexedFaceSet *indexedFaceSet,GLuint texture);
	bool DrawIndexedFaceSetFromPolygon(CSspVrmlIndexedFaceSet *indexedFaceSet,GLuint texture);
	bool DrawCylinder(CSspVrmlCylinder *cylinder);
	bool DrawIndexedLineSet(CSspVrmlIndexedLineSet *indexLineSet);

	int CalTriNormal(CSspVrmlIndexedFaceSet::tPoint *point1,
		CSspVrmlIndexedFaceSet::tPoint *point2,
		CSspVrmlIndexedFaceSet::tPoint *point3,
		float normal[3]);

	CSspVrmlAppearance* FindAppearance(SString val);
	void SetRecord(t_ssp_3d_vrml *record) { memcpy(&m_record,record,sizeof(m_record)); }

public:
	SString sFilePath;
	int m_iCurrTextRow;
	SPtrList<CSspVrmlTransform> m_Transforms;
	SPtrList<CSspVrmlAppearance> m_Appearances;
	BYTE *m_vrmlText;
	int m_braceCnt;
	int m_bracketCnt;
	SString errTxt;
	GLUquadricObj *m_quadric;

	t_ssp_3d_vrml m_record;
	bool vboSupported;
};

#endif//__SSP_3D_VRML_H__