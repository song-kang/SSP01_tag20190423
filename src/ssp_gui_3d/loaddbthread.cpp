#include "loaddbthread.h"
#include "ssp_3d_widget.h"
#include "JlCompress.h"

LoadDbThread::LoadDbThread(QObject *parent)
	: QThread(parent)
{
	
}

LoadDbThread::~LoadDbThread()
{
	
}

void LoadDbThread::run()
{
	int step = 0;
	SString sql;
	SRecordset rs;

	sql.sprintf("select texture_id,tex_filename from t_ssp_3d_texture");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			SString filename = rs.GetValue(i,1);
			SString sPath = SBASE_SAPP->GetHomePath()+"textures\\"+filename;
			bool b = DB->ReadLobToFile("t_ssp_3d_texture","tex_image",SString::toFormat("texture_id=%d",rs.GetValue(i,0).toInt()),sPath);
			emit m_3DPannel->sigProgressValue(++step);
		}
	}

	sql.sprintf("select obj_name,tex1_x,tex1_y from t_ssp_3d_scene_object where draw_type=5");
	cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			SString filename = rs.GetValue(i,0);
			float width = rs.GetValue(i,1).toFloat();
			float hight = rs.GetValue(i,2).toFloat();
			if (width <= 0.0 || hight <= 0.0)
				continue;

			SString sPath = SBASE_SAPP->GetHomePath()+"textures\\"+filename;
			makeCharBMP(filename,sPath,width,hight);
		}
		emit m_3DPannel->sigProgressValue(++step);
	}

	sql.sprintf("select obj_name,tex1_x,tex1_y from t_ssp_3d_predefine_obj where draw_type=5");
	cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			SString filename = rs.GetValue(i,0);
			float width = rs.GetValue(i,1).toFloat();
			float hight = rs.GetValue(i,2).toFloat();
			if (width <= 0.0 || hight <= 0.0)
				continue;

			SString sPath = SBASE_SAPP->GetHomePath()+"textures\\"+filename;
			makeCharBMP(filename,sPath,width,hight);
		}
		emit m_3DPannel->sigProgressValue(++step);
	}

	sql.sprintf("select vrml_id,vrml_name,vrml_filename from t_ssp_3d_vrml");
	cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			SString name = rs.GetValue(i,1);
			SString filename = rs.GetValue(i,2);

			SString sPath = SBASE_SAPP->GetHomePath()+SString::toFormat("vrmls\\%s\\",name.data());
			QDir dir = QDir(sPath.data());
			if (!dir.exists())
				bool b = dir.mkdir(sPath.data());

			SString sPathZip = sPath + filename;
			if (DB->ReadLobToFile("t_ssp_3d_vrml","vrml_content",SString::toFormat("vrml_id=%d",rs.GetValue(i,0).toInt()),sPathZip))
				JlCompress::extractDir(QString("%1").arg(sPathZip.data()), QString("%1").arg(sPath.data()));
			emit m_3DPannel->sigProgressValue(++step);
		}
	}
}

void LoadDbThread::makeCharBMP(SString filename,SString path,float width,float hight)
{
	QSize size((int)(width*10),(int)(hight*10));
	QImage image(size,QImage::Format_ARGB32);
	image.fill(qRgba(200,200,200,0));

	QPainter painter(&image);
	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	QPen pen = painter.pen();
	pen.setColor(Qt::white);

	QFont font = painter.font();
	font.setFamily("ºÚÌå");
	font.setBold(true);
	font.setPixelSize((int)(hight*10));

	painter.setPen(pen);
	painter.setFont(font);
	painter.drawText(image.rect(),Qt::AlignCenter,QString("%1").arg(filename.data()));
	image.save(QString("%1.bmp").arg(path.data()),"BMP",100);
}