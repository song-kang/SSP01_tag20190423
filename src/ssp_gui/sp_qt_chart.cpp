#include "sp_qt_chart.h"


//////////////////////////////////////////////////////////////////////////
// 描    述:  绘制饼图
// 作    者:  郁文枢
// 创建时间:  2015-8-10 14:18
// 参数说明:  @pPainter为QT画板对象
//         :  @rect为绘制区域，绘制行为不可超出该区域
//         :  @pRs为需要显示的数据集
//         :  @sTitle为图表的显示标题，当sTitle为空时取第二列的列标题
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSpQtChartPainter::DrawChartPie(QPainter *pPainter,SRect &rect,SRecordset *pRs,SString sTitle,QColor corText,QColor corLine,SString sExtAttr)
{
	double startAngle = 90.0*16;//起始角
	QBrush oldBrush=pPainter->brush();//还原用
	QPen oldPen=pPainter->pen();
	QFont oldFont=pPainter->font();
	QString name="";
	int numVal=pRs->GetRows();//图例个数
	double spiltName=1.0/10;//名称默认占1/8
	bool HorW=true;//true为左右 false为上下
	double spiltHeight=0;//图例占比例
	double spiltWidth=0;
	double rad=0;//直径
	double colorRectWidth=0;//图例彩色正方形边长
	double rowWidth=0;//图例一行大约占用距离
	double exmpleRect=0;//图例最大字数
	double fontNum=3.0/4;
	pPainter->setRenderHint(QPainter::Antialiasing,true);//设置平滑
	for(int row=0;row<numVal;row++) 
	{
		if(exmpleRect<QString(pRs->GetValue(row,0).data()).length())
			exmpleRect=QString(pRs->GetValue(row,0).data()).length();
		if(exmpleRect==0)
			exmpleRect=1;
	}
	if(exmpleRect < 5)
		exmpleRect=5;
// 	QFontMetrics fm(pPainter->font());
// 	s.cx = fm.width("");
// 	s.cy = fm.height();

	if((rect.bottom-rect.top)*spiltName>=35)
		spiltName=35.0/(rect.bottom-rect.top);
	double height=(rect.bottom-rect.top)*(1-spiltName);
	if((rect.bottom-rect.top)*(1-spiltName)<(rect.right-rect.left))
	{
		//栏目在左侧
		rowWidth=height/(numVal+2);
		if(rowWidth*(exmpleRect+2.5)>(rect.right-rect.left)/2)//防止图例超过中间
			rowWidth=(rect.right-rect.left)/2/(exmpleRect+2.5);
		colorRectWidth=rowWidth*2.0/3;//留一定空隙
		spiltWidth=colorRectWidth*(exmpleRect+1.5)/(rect.right-rect.left);
		if((rect.right-rect.left)*(1-spiltWidth)>(rect.bottom-rect.top)*(1-spiltName-spiltHeight))//园的直径
			rad=(rect.bottom-rect.top)*(1-spiltName-spiltHeight)-colorRectWidth;
		else
			rad=(rect.right-rect.left)*(1-spiltWidth)-colorRectWidth;
		HorW=true;
	}
	else
	{
		//栏目在下侧
		spiltHeight=(1-spiltName)/2;//最少给图例留1/2空间
		if((rect.right-rect.left)*(1-spiltWidth)>(rect.bottom-rect.top)*(1-spiltName-spiltHeight))//园的直径
			rad=(rect.bottom-rect.top)*(1-spiltName-spiltHeight);
		else
			rad=(rect.right-rect.left)*(1-spiltWidth);

		if(rad+(rect.bottom-rect.top)*spiltName<(rect.bottom-rect.top)*(1-spiltHeight))//如果下方空多则图例上移
		{
			rowWidth=((rect.bottom-rect.top)-rad-(rect.bottom-rect.top)*(spiltName))/(numVal+2);
			spiltHeight=1-(spiltName*(rect.bottom-rect.top)+rad)/(rect.bottom-rect.top);
		}
		else
			rowWidth=(rect.bottom-rect.top)*(spiltHeight)/(numVal+2);
  		if(rowWidth*(exmpleRect+1.5)>(rect.right-rect.left))//防止图例超过宽度
  			rowWidth=(rect.right-rect.left)/(exmpleRect+1.5);
		colorRectWidth=rowWidth*2.0/3;//留空用
		HorW=false;
	}


	if(colorRectWidth>20)//最大20
	{
		rowWidth=25;
		colorRectWidth=20;
	}

	if(sTitle != "")
		name=sTitle.data();
	else if(pRs->GetColumns()>=2)
		name=pRs->GetColumnName(1).data();//确定名称
	if(name.size()!=0)
	{
		QFont tmpFont=pPainter->font(); 
		pPainter->setPen(QPen(QBrush(corText),1));

		if((rect.bottom-rect.top)*(spiltName)<(rect.right-rect.left)/name.size())//题目字体大小
			tmpFont.setPointSize((rect.bottom-rect.top)*(spiltName)*fontNum);
		else
			tmpFont.setPointSize((rect.right-rect.left)/name.size()*fontNum);
		if(tmpFont.pointSize()>8)//题目字体过小不绘制
		{
			pPainter->setFont(tmpFont);
			pPainter->drawText(rect.left,rect.top,(rect.right-rect.left)
				,(rect.bottom-rect.top)*(spiltName),Qt::AlignCenter,name);
		}
		else
			spiltName=0.0;//标题位置空出
	}
	else
		spiltName=0.0;//标题位置空出
	pPainter->setBrush(Qt::NoBrush);
	
	pPainter->setPen(QPen(QBrush(corLine),1));
	QFont tmpFont;
	tmpFont.setPointSize(colorRectWidth*fontNum);
	pPainter->setFont(tmpFont);
	double cy=0.0;
	double sum=0.0;
	SString sColor;
	for(int row=0;row<numVal;row++) 
	{
		if(colorRectWidth>8)
		{
			int ti=(row+numVal/2)%numVal;
			sColor = SString::GetAttributeValue(sExtAttr,SString::toFormat("c%d",row+1));
			QColor color;
			if(sColor.length()>0)
				color = StrToQcor(sColor);
			else
				color = QColor(255-(ti*200)/numVal-(ti%2)*50,(100*ti)%155+2*(ti*ti)%100,((ti*2000)/numVal)%150+(ti%2)*100);
			pPainter->setBrush(QBrush(color));  
			if(spiltHeight==0)//右侧
			{
				pPainter->setPen(QPen(QBrush(corText),1));
				pPainter->fillRect(rect.right-colorRectWidth*(exmpleRect+1)-4,
					rect.top+cy+(rect.bottom-rect.top)*(spiltName)+colorRectWidth*1,
					colorRectWidth,colorRectWidth,QBrush(color)); 
				//pPainter->setPen(QPen(QBrush(corText),1));
				pPainter->drawText(rect.right-colorRectWidth*(exmpleRect),
					rect.top+cy+(rect.bottom-rect.top)*(spiltName)+colorRectWidth*1,
					colorRectWidth*exmpleRect,colorRectWidth,Qt::AlignLeft,pRs->GetValue(row,0).data());  
				cy +=(rowWidth*(numVal+2)-colorRectWidth*(numVal+2))/(numVal+1)+colorRectWidth;  
			}
			else//下侧
			{
				pPainter->setPen(QPen(QBrush(corText),1));
				pPainter->fillRect(rect.left+1*colorRectWidth,
					rect.top+rad+(rect.bottom-rect.top)*(spiltName)+1*colorRectWidth+cy,
					colorRectWidth,colorRectWidth,QBrush(color));  
				//pPainter->setPen(QPen(QBrush(corText),1));
				pPainter->drawText( rect.left+2*colorRectWidth+2,
					rect.top+rad+(rect.bottom-rect.top)*(spiltName)+1*colorRectWidth+cy,
					(rect.right-rect.left)-1.5*colorRectWidth,
					colorRectWidth,Qt::AlignLeft,pRs->GetValue(row,0).data());
				cy +=(rowWidth*(numVal+2)-colorRectWidth*(numVal+2))/(numVal+1)+colorRectWidth; 
			}
			
		}	
		else
		{	
			(spiltWidth)=0.0;//位置空出
			(spiltHeight)=0.0;
			colorRectWidth=0;
		}
		sum=sum+pRs->GetValue(row,1).toFloat();//记录数据综合
	}
	pPainter->setBrush(Qt::NoBrush);
	pPainter->setPen(QPen(corText,1));
	if(colorRectWidth>8)
	{
		if(HorW==true)//绘制矩形,右侧
		{
// drawRect函数存在bug，网址报告http://lists.qt-project.org/pipermail/interest/2014-April/011928.html
// 			pPainter->drawRect(rect.right-colorRectWidth*(exmpleRect+2),
// 				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5,
// 				colorRectWidth*(exmpleRect+1.5)-colorRectWidth,rowWidth*(numVal)-colorRectWidth*1);
			pPainter->setRenderHint(QPainter::Antialiasing,false);//设置平滑
			pPainter->drawLine(rect.right-colorRectWidth*(exmpleRect+2),
				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5,
				rect.right-colorRectWidth*(exmpleRect+2),
				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5+rowWidth*(numVal+2)-colorRectWidth*1);
			pPainter->drawLine(rect.right-colorRectWidth*(exmpleRect+2),
				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5,
				rect.right-colorRectWidth*(exmpleRect+2)+colorRectWidth*(exmpleRect+1.5),//-colorRectWidth,
				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5);
			pPainter->drawLine(rect.right-colorRectWidth*(exmpleRect+2)+colorRectWidth*(exmpleRect+1.5),//-colorRectWidth,
				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5,
				rect.right-colorRectWidth*(exmpleRect+2)+colorRectWidth*(exmpleRect+1.5),//-colorRectWidth,
				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5+rowWidth*(numVal+2)-colorRectWidth*1);
			pPainter->drawLine(rect.right-colorRectWidth*(exmpleRect+2),
				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5+rowWidth*(numVal+2)-colorRectWidth*1,
				rect.right-colorRectWidth*(exmpleRect+2)+colorRectWidth*(exmpleRect+1.5),//-colorRectWidth,
				rect.top+(rect.bottom-rect.top)*spiltName+colorRectWidth*0.5+rowWidth*(numVal+2)-colorRectWidth*1);
			pPainter->setRenderHint(QPainter::Antialiasing,true);//设置平滑
		}
		else//绘制矩形,下侧
		{
//			pPainter->drawRect(rect.left+0.5*colorRectWidth,
// 				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth,
// 				(rect.right-rect.left)-1*colorRectWidth,rowWidth*(numVal+2)-colorRectWidth);
			pPainter->setRenderHint(QPainter::Antialiasing,false);//设置平滑
			pPainter->drawLine(rect.left+0.5*colorRectWidth,
				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth,
				rect.left+0.5*colorRectWidth,
				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth+rowWidth*(numVal+2)-colorRectWidth);

			pPainter->drawLine(rect.left+0.5*colorRectWidth,
				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth,
				rect.left+0.5*colorRectWidth+(rect.right-rect.left)-1*colorRectWidth,
				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth);

			pPainter->drawLine(rect.left+0.5*colorRectWidth,
				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth+rowWidth*(numVal+2)-colorRectWidth,
				rect.left+0.5*colorRectWidth+(rect.right-rect.left)-1*colorRectWidth,
				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth+rowWidth*(numVal+2)-colorRectWidth);

			pPainter->drawLine(rect.left+0.5*colorRectWidth+(rect.right-rect.left)-1*colorRectWidth,
				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth,
				rect.left+0.5*colorRectWidth+(rect.right-rect.left)-1*colorRectWidth,
				rect.top+(rect.bottom-rect.top)*(1-spiltHeight)+0.5*colorRectWidth+rowWidth*(numVal+2)-colorRectWidth);
			pPainter->setRenderHint(QPainter::Antialiasing,true);//设置平滑
		}
	}
		//绘制圆形
	int fontSize=pPainter->font().pointSize();//留空给数值
		pPainter->setPen(QPen(corLine,1));
		//pPainter->setPen(QPen(QColor(255,255,255),1));
		rad=rad;
		if(HorW==true)//右侧
		{
			if((rect.right-rect.left)/2-(colorRectWidth*(exmpleRect+1.5)+colorRectWidth)>rad/2)//可以居中
			{
				pPainter->drawEllipse((rect.right-rect.left)/2-rad/2+rect.left,
					rect.top+(rect.bottom-rect.top)*(spiltName)+fontSize,
					rad,rad);
			}
			else
			{
				pPainter->drawEllipse(rect.right-(colorRectWidth*(exmpleRect+1.5)+colorRectWidth)-rad,//不够居中
					rect.top+(rect.bottom-rect.top)*(spiltName)+fontSize,
					rad,rad);
			}
		}
		else//下方
		{
			pPainter->drawEllipse(rect.left,rect.top+(rect.bottom-rect.top)*(spiltName)+fontSize,
				rad,rad);
		}
	
	pPainter->setPen(QPen(QBrush(corLine),1));
	if(rad>20)//小于20不绘制
	{
		for(int row=0;row<numVal;row++) 
		{
			double value =pRs->GetValue(row,1).toFloat();
			if(value==0)
				continue;
			int ti=(row+numVal/2)%numVal;
			double angle = -360*value*16/sum;//顺时针
			sColor = SString::GetAttributeValue(sExtAttr,SString::toFormat("c%d",row+1));
			QColor color;
			if(sColor.length()>0)
				color = StrToQcor(sColor);
			else
				color = QColor(255-(ti*200)/numVal-(ti%2)*50,(100*ti)%155+2*(ti*ti)%100,((ti*2000)/numVal)%150+(ti%2)*100);
			pPainter->setBrush(QBrush(color));
			if(HorW==true)//右侧
			{
				if((rect.right-rect.left)/2-(colorRectWidth*(exmpleRect+1.5)+colorRectWidth)>rad/2)//可以居中
				{
					pPainter->drawPie((rect.right-rect.left)/2-rad/2+rect.left,
						rect.top+(rect.bottom-rect.top)*(spiltName)+fontSize,
						rad,rad,int(startAngle),int(angle));
				}
				else
				{
					pPainter->drawPie(rect.right-(colorRectWidth*(exmpleRect+1.5)+colorRectWidth)-rad,//不够居中
						rect.top+(rect.bottom-rect.top)*(spiltName)+fontSize,
						rad,rad,int(startAngle),int(angle));
				}
			}
			else//下方
			{
				pPainter->drawPie(rect.left,
					rect.top+(rect.bottom-rect.top)*(spiltName)+fontSize,
					rad,rad,int(startAngle),int(angle));
			}
			startAngle += angle;
		}
		//画值，与画饼分开避免被覆盖
		startAngle=90.0*16;
		for(int row=0;row<numVal;row++) 
		{
			double value =pRs->GetValue(row,1).toFloat();
			if(value==0)
				continue;
			int ti=(row+numVal/2)%numVal;
			double angle = -360*value*16/sum;//顺时针
			QColor color;
			sColor = SString::GetAttributeValue(sExtAttr,SString::toFormat("c%d",row+1));
			if(sColor.length()>0)
				color = StrToQcor(sColor);
			else
				color = QColor(255-(ti*200)/numVal-(ti%2)*50,(100*ti)%155+2*(ti*ti)%100,((ti*2000)/numVal)%150+(ti%2)*100);
			pPainter->setBrush(QBrush(color));
			if(HorW==true)//右侧
			{
				if((rect.right-rect.left)/2-(colorRectWidth*(exmpleRect+1.5)+colorRectWidth)>rad/2)//可以居中
				{
					pPainter->setPen(QPen(QBrush(Qt::black),1));
					pPainter->drawText(rect.left+(rect.right-rect.left)/2+cos(((startAngle+angle/2/*/5*4*/)/16)*0.017453)*rad/2,
						rect.top+(rect.bottom-rect.top)*(spiltName)+rad/2+fontSize-sin(((startAngle+angle/2/*/5*4*/)/16)*0.017453)*rad/2,
						QString::number(value));//显示数值
					pPainter->setPen(QPen(QBrush(corLine),1));
				}
				else
				{
					pPainter->setPen(QPen(QBrush(Qt::black),1));
					pPainter->drawText(rect.right-(colorRectWidth*(exmpleRect+1.5)+colorRectWidth)-rad/2+cos(((startAngle+angle/2/*/5*4*/)/16)*0.017453)*rad/2,
						rect.top+(rect.bottom-rect.top)*(spiltName)+rad/2+fontSize-sin(((startAngle+angle/2/*/5*4*/)/16)*0.017453)*rad/2,
						QString::number(value));//显示数值
					pPainter->setPen(QPen(QBrush(corLine),1));
				}
			}
			else//下方
			{
				pPainter->setPen(QPen(QBrush(Qt::black),1));
				pPainter->drawText(rect.left+rad/2+cos(((startAngle+angle/2/*/5*4*/)/16)*0.017453)*rad/2,
					rect.top+(rect.bottom-rect.top)*(spiltName)+fontSize+rad/2-sin(((startAngle+angle/2/*/5*4*/)/16)*0.017453)*rad/2,
					QString::number(value));//显示数值
				pPainter->setPen(QPen(QBrush(corLine),1));
			}
			startAngle += angle;
		}
	}
	//还原
	pPainter->setBrush(oldBrush);
	pPainter->setPen(oldPen);
	pPainter->setFont(oldFont);
	pPainter->setRenderHint(QPainter::Antialiasing,false);//设置平滑
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  绘制柱图
// 作    者:  
// 创建时间:  2015-8-10 14:18
// 参数说明:  @pPainter为QT画板对象
//         :  @rect为绘制区域，绘制行为不可超出该区域
//         :  @pRs为需要显示的数据集
//         :  @sTitle为图表的显示标题，当sTitle为空时取第二列的列标题
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSpQtChartPainter::DrawChartColumn(QPainter *pPainter,SRect &rect,SRecordset *pRs,SString sTitle,QColor corText,QColor corLine,SString sExtAttr)
{
	//TODO:
	int yfrom=SString::GetAttributeValue(sExtAttr,"yfrom").toInt();
	int yto=SString::GetAttributeValue(sExtAttr,"yto").toInt();
	if (pRs->GetRows()==0)
	{
		return;
	}
	if (sTitle.isEmpty())
		sTitle=pRs->GetColumnName(1);
	QPen oldPen=pPainter->pen();
	QFont oldFont=pPainter->font();
	QBrush oldBrush=pPainter->brush();
	//数据准备
	QFont titleFont=oldFont;
	QFont scalFont=oldFont;
	QFont nameFont=oldFont;
	QFont valFont=oldFont;
	titleFont.setPixelSize(25);
	scalFont.setPixelSize(15);
	valFont.setPixelSize(15);
	nameFont.setPixelSize(15);
	int disX=(rect.right-rect.left)/15;
	int disY=(rect.bottom-rect.top)/15;
	int titleHeight=(rect.bottom-rect.top)/15;
	if (titleHeight>=35)
	{
		titleHeight=35;
	}
	if (titleHeight<25)
	{
		titleFont.setPixelSize(titleHeight);
	}
	bool useY;
	SString sColor;

	float fMax=0.0;//最大的值
	float fMin=0.0;//最小的值
	float fScal=1.0;//刻度最大值
	//float fUnit=0.0;//坐标刻度步长
	int scalMaxLen=0;//坐标系数文字最大长度
	int nameMaxLen=0;//名称最大长度文字
	int valueMaxLen=0;//数值最大长度
	int n=0;//刻度个数
	int nameLen=0;
	int valueLen=0;
	int nChart=pRs->GetRows();//个数
	fMin=pRs->GetValue(0,1).toFloat();
	for(int i=0;i<pRs->GetRows();i++)
	{
		if (fMax<pRs->GetValue(i,1).toFloat())
		{
			fMax=pRs->GetValue(i,1).toFloat();
		}
		if (fMin>pRs->GetValue(i,1).toFloat())
		{
			fMin=pRs->GetValue(i,1).toFloat();
		}
		nameLen=QFontMetrics(nameFont).width(pRs->GetValue(i,0).data());
		valueLen=QFontMetrics(nameFont).width(pRs->GetValue(i,1).data());
		if (nameMaxLen<nameLen)
		{
			nameMaxLen=nameLen;
		}
		if (valueMaxLen<valueLen)
		{
			valueMaxLen=valueLen;
		}
	}
	//fMax=50000000;
	if (fMax<=50)
	{
		if (fMax>=1)
		{
			fScal=(int(fMax)/5)*5+5;
		}
		else
		{
			if (fMax>=0.5)
				fScal = 1.00;
			else
				fScal = 0.50;
		}
		n=fScal>1?fScal/5+1:fScal*4+1;
	}else
	{
		for(int i=0;i<8;i++)
		{
			fScal*=10;
			if (fMax<fScal)
			{
				break;
			}
		}
		//if (fMax<(fScal*1.5)/10)
		//{
		//	fScal=(fScal*1.5)/10;
		//}
		//n=11;
		if (fMax<fScal/2)
		{
			n=fMax/(fScal/20) +1;
			fScal=(fScal/20)*n;
		}else
		{
			n=fMax/(fScal/10)+1;
			fScal=(fScal/10)*n;
		}
		n=n+1;
		//n=fMax/(fScal/50);
		//fScal=(fScal/50)*(n+1);
	}
	if(fMin>=yfrom&&fMax<=yto)
		useY=true;
	else
		useY=false;
	if(useY==true)
	{
		fMin=yfrom;fMax=yto;
	}
	QString sScal;
	if(useY==true)
		fScal=yto;
	if (fScal<=1)
	{
		sScal.sprintf("%.2f",fScal);
	}else
	{
		sScal=QString("%L1").arg(fScal,0,'f',0);
	}
	scalMaxLen=QFontMetrics(scalFont).width(sScal);
	if (disX<scalMaxLen&&scalMaxLen<disX*3)
	{
		disX=scalMaxLen;
	}
	if (scalMaxLen>=disX*3)
	{
		scalFont.setPixelSize(10);
	}
	int scalX=rect.left+disX;//坐标系x起点
	

// 	if (colWidth>0)
// 	{
// 		//disY=(nameMaxLen/colWidth+0.5)*15+3;
// 		if (disY<15)
// 		{
// 			disY=15;
// 		}
// 		if (disY>=(rect.bottom-rect.top)/2)
// 		{
// 			disY=(rect.bottom-rect.top)/2;
// 		}
// 	}
// 	int scalY=rect.bottom-disY;//坐标系y起点
// 	int yLen=0;//坐标系y长度
// 	if (rect.bottom-rect.top>15*15)
// 	{
// 		yLen=rect.bottom-rect.top-disY-titleHeight-15;//坐标系y长度
// 	}else
// 	{
// 		yLen=rect.bottom-rect.top-disY-titleHeight-(rect.bottom-rect.top)/15;//坐标系y长度
// 	}
// 	if (yLen==0|xLen==0)
// 	{
// 		return;
// 	}
	bool b_leg=true;//显示柱名称
	bool showFont=false;//显示柱名称列表
	SRect chartRect;//图标区域
	SRect legendRect;//图例区域
	int tempsize=15,tempsize1=15;
	if (rect.right-rect.bottom>rect.top)
	{
	}
	if (nameMaxLen+60>(rect.right-rect.left))
	{
		tempsize=15*(rect.right-rect.left)/(nameMaxLen+60);
		if (tempsize>1)
		{
			nameFont.setPixelSize(tempsize);
		}
	}
	if (nChart*tempsize+(40*tempsize)/15>(rect.bottom-rect.top-titleHeight))
	{
		tempsize1=tempsize*(rect.bottom-rect.top-titleHeight)/(nChart*tempsize+(40*tempsize)/15);
		if (tempsize1>1)
		{
			nameFont.setPixelSize(tempsize1);
		}
	}
	int tsize=nameFont.pixelSize();
	nameMaxLen=(nameMaxLen*nameFont.pixelSize())/15;
	//scalMaxLen=QFontMetrics(nameFont).width(sScal);
	if (tsize<5||rect.bottom-rect.top<15||rect.right-rect.left<15)
	{
		b_leg=false;
		chartRect.left=rect.left;
		chartRect.right=rect.right;
		chartRect.top=rect.top+titleHeight;
		chartRect.bottom=rect.bottom;
	}
	if (tempsize<=1||tempsize1<=1)
	{
		b_leg=false;
	}
	chartRect.left=rect.left;
	chartRect.right=rect.right-nameMaxLen-(70*tsize)/15;
	chartRect.top=rect.top+titleHeight;
	chartRect.bottom=rect.bottom;
	legendRect.left=chartRect.right+(10*tsize)/15;
	legendRect.right=rect.right-(10*tsize)/15;
	legendRect.top=rect.top+titleHeight+(10*tsize)/15;
	legendRect.bottom=rect.bottom-(10*tsize)/15;
	if (legendRect.right-legendRect.left>(rect.right-rect.left)/2)
	{
		b_leg=false;
		chartRect.left=rect.left;
		chartRect.right=rect.right;
		chartRect.top=rect.top+titleHeight;
		chartRect.bottom=rect.bottom;
	}
	int xLen=rect.right-rect.left-2*disX;//坐标系x长度
	if (nameMaxLen+10<(double)xLen/((pRs->GetRows())))
	{
		b_leg=false;
		chartRect.left=rect.left;
		chartRect.right=rect.right;
		chartRect.top=rect.top+titleHeight;
		chartRect.bottom=rect.bottom;
		showFont=true;
	}
	//pPainter->drawRect(chartRect.left,chartRect.top,chartRect.right-chartRect.left,chartRect.bottom-chartRect.top);

	xLen=chartRect.right-chartRect.left-scalMaxLen-20;
	int colSpace=(double)xLen/(4*(pRs->GetRows()));//柱子之间间隔
	int colWidth=colSpace*3;//柱子宽度


	colWidth+= (xLen-colSpace*4*pRs->GetRows())/pRs->GetRows();
	if (colSpace>0)
	{
		if (disY<15)
		{
			disY=15;
		}
		if (disY>=(chartRect.bottom-chartRect.top)/2)
		{
			disY=(chartRect.bottom-chartRect.top)/2;
		}
	}
	int scalY=rect.bottom-disY;//坐标系y起点
	int yLen=0;//坐标系y长度
	disX=scalMaxLen+(10*tsize/15);
	scalX=chartRect.left+scalMaxLen+(10*tsize)/15;
	scalY=chartRect.bottom-disY;
	yLen=chartRect.bottom-chartRect.top-disY-10;

	float sMin=0.0;
	for (int i=1;i<n;i++)
	{
		if(fScal*i/(n-1)>fMin)
		{
			sMin=fScal*(i-1)/(n-1);
			break;
		}
	}
	if(!useY)
		fScal=fScal-sMin;
	else
	{
		sMin=yfrom;
		fScal=yto-sMin;
	}
	//数据准备

	//绘制标题
	if (titleHeight>10)
	{
		pPainter->setFont(titleFont);
		pPainter->setPen(corText);
		pPainter->drawText(scalX,rect.top,xLen,titleHeight,Qt::AlignCenter,sTitle.data());
	}
	//绘制标题


	//绘制图例
	pPainter->setBrush(Qt::NoBrush);
	if (b_leg)
	{
		pPainter->setPen(QPen(QBrush(corLine),1));
		//pPainter->drawRect(legendRect.left,legendRect.top,legendRect.right-legendRect.left,legendRect.bottom-legendRect.top);
		if(legendRect.top+nChart*(nameFont.pixelSize()+4)+(10*tsize)/15>=legendRect.bottom)//显示不下截断多余的图例
			pPainter->drawRect(legendRect.left,legendRect.top,legendRect.right-legendRect.left,legendRect.bottom-legendRect.top);
		else
			pPainter->drawRect(legendRect.left,legendRect.top,legendRect.right-legendRect.left,nChart*(nameFont.pixelSize()+4)+nameFont.pixelSize()/2+(10*tsize)/15);
		pPainter->setFont(nameFont);
		for(int i=0;i<nChart;i++)
		{			
			if(legendRect.top+i*(nameFont.pixelSize()+4)+(10*tsize)/15>=legendRect.bottom)//显示不下截断多余的图例
				break;
			int ti=(i+nChart/2)%nChart;
			sColor = SString::GetAttributeValue(sExtAttr,SString::toFormat("c%d",i+1));
			QColor color;
			if(sColor.length()>0)
				color = StrToQcor(sColor);
			else
				color = qRgb(255-(ti*200)/nChart-(ti%2)*50,(100*ti)%155+2*(ti*ti)%100,((ti*2000)/nChart)%150+(ti%2)*100);
			pPainter->setBrush(QBrush(color));
			pPainter->setPen(QPen(QBrush(color),1));
			pPainter->drawRect(legendRect.left+(5*tsize)/15,legendRect.top+i*(nameFont.pixelSize()+4)+(10*tsize)/15,(20*tsize)/15,nameFont.pixelSize());
			pPainter->setPen(corText);
			pPainter->drawText(legendRect.left+(40*tsize)/15,legendRect.top+i*(nameFont.pixelSize()+4)+(10*tsize)/15,nameMaxLen,nameFont.pixelSize(),Qt::AlignLeft,QString("%1").arg(pRs->GetValue(i,0).data()));

		}
	}
	//绘制图例


	//绘制坐标系
	int num1=fScal;
	int num2=2;
	if(num1>=0&&num1<=10)
		num2=2;
	else 
	{
		int n1=10;int n2=50;
		while(1)
		{
			if(n1<num1&&num1<=n2)
			{
				num2=n1;
				break;
			}
			if(n2<num1&&num1<=n1*10)
			{
				num2=n1*2;break;
			}
			n1=n1*10;n2=n2*10;
		}
	}
	n=num1/num2;
	int m=num1%num2;

	if(m!=0)
		n=n+2;
	else
		n=n+1;
	fScal=(n-1)*num2;
	pPainter->setPen(QPen(QBrush(corLine),1));
	pPainter->drawLine(scalX,scalY,scalX+xLen,scalY);
	pPainter->drawLine(scalX,scalY,scalX,scalY-yLen);
	//int n=fScal>1?fScal/5+1:fScal*2+1;
	int scalHeight=yLen-yLen%5;//刻度最大长度和坐标系最大长度有误差
	if(n==1)
		return;
	pPainter->setFont(scalFont);
	for (int i=0;i<n;i++)
	{	
		if (scalHeight/(n-1)<8)
		{
			if (i%4!=0)
			{
				continue;
			}
		}
		if (scalHeight/(n-1)<15)
		{
			if (i%2==1)
				continue;
		}
		pPainter->setPen(QPen(QBrush(corLine),1,Qt::DotLine));
		pPainter->drawLine(scalX,scalY-(scalHeight*i)/(n-1),scalX+xLen,scalY-(scalHeight*i)/(n-1));
		//pPainter->drawLine(scalX,scalY-num2*i,scalX+xLen,scalY-num2*i);
		if (disY>10&&disX>10)
		{
			pPainter->setPen(corText);//存在误差
			if (fScal<=1)
				//pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString().sprintf("%.2f",fScal*i/(n-1)+sMin));
// 				if(i==n-1)
// 					pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString().sprintf("%.2f",fScal+sMin));
// 				else
					pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString().sprintf("%.2f",num2*i+sMin));
			else
				//pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(fScal*i/(n-1)+sMin,0,'f',0));
// 				if(i==n-1)
// 					pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(fScal+sMin,0,'f',0));
// 				else
					pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(num2*i+sMin,0,'f',0));
		}
	}
	//绘制坐标系

	if(fScal==0)
	{
		pPainter->setPen(oldPen);
		pPainter->setFont(oldFont);
		pPainter->setBrush(oldBrush);
		return;
	}
	//绘制柱子
	pPainter->setPen(corText);
	int colHeight=0;
	pPainter->setPen(QPen(QBrush(corText),1));
	if (disY<15||colWidth<15)
	{
		nameFont.setPixelSize(colWidth>disY?disY:colWidth);
	}
	if (colWidth<valueMaxLen)
	{
		valFont.setPixelSize((colWidth*15)/valueMaxLen);
	}
	//int nrow=pRs->GetRows();
	int ti=0;
	for(int i=0;i<nChart;i++)
	{
		colHeight=scalHeight*(pRs->GetValue(i,1).toFloat()-sMin)/fScal;
		ti=(i+nChart/2)%nChart;
		sColor = SString::GetAttributeValue(sExtAttr,SString::toFormat("c%d",i+1));
		QColor color;
		if(sColor.length()>0)
			color = StrToQcor(sColor);
		else
			color = qRgb(255-(ti*200)/nChart-(ti%2)*50,(100*ti)%155+2*(ti*ti)%100,((ti*2000)/nChart)%150+(ti%2)*100);
		pPainter->fillRect(scalX+(i+1)*colSpace+i*colWidth,scalY-colHeight,colWidth,colHeight,color);
		if (colWidth>10)
		{
			pPainter->setFont(valFont);
			if (QFontMetrics(valFont).width(pRs->GetValue(i,1).data())>colWidth+2)
			{
				pPainter->drawText(scalX+(i+1)*colSpace+i*colWidth-colSpace/2,rect.top+titleHeight,colWidth+colSpace,scalY-colHeight-titleHeight-rect.top,Qt::AlignBottom|Qt::AlignLeft,QString("%1").arg(pRs->GetValue(i,1).data()));
			}else
			{
				pPainter->drawText(scalX+(i+1)*colSpace+i*colWidth,rect.top+titleHeight,colWidth,scalY-colHeight-titleHeight-rect.top,Qt::AlignBottom|Qt::AlignHCenter,QString("%1").arg(pRs->GetValue(i,1).data()));
			}
		}
		if (disY>10&&showFont==true)
		{
			pPainter->setFont(nameFont);
			pPainter->drawText(scalX+(i+1)*colSpace+i*colWidth,scalY+3,colWidth,disY-3,Qt::AlignTop|Qt::TextWrapAnywhere,QString("%1").arg(pRs->GetValue(i,0).data()));
		}
	}
	//绘制柱子
	pPainter->setPen(oldPen);
	pPainter->setFont(oldFont);
	pPainter->setBrush(oldBrush);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  绘制折线图
// 作    者:  
// 创建时间:  2015-8-10 14:18
// 参数说明:  @pPainter为QT画板对象
//         :  @rect为绘制区域，绘制行为不可超出该区域
//         :  @pRs为需要显示的数据集
//         :  @sTitle为图表的显示标题，当sTitle为空时取第二列的列标题
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSpQtChartPainter::DrawChartLine(QPainter *pPainter,SRect &rect,SRecordset *pRs,SString sTitle,QColor corText,QColor corLine,SString sExtAttr)
{
	//TODO:
	//pPainter->drawText(rect.left,rect.top+20,"这是一个折线图");
	int yfrom=SString::GetAttributeValue(sExtAttr,"yfrom").toInt();
	int yto=SString::GetAttributeValue(sExtAttr,"yto").toInt();
	bool useY=false;

	if (pRs->GetRows()==0)
	{
		return;
	}
	bool b_sm;//是否平滑
	bool b_leg=true;//是否绘制图例
 	b_sm = pPainter->testRenderHint(QPainter::Antialiasing);
// 	if (!b_sm)
// 	{
// 		pPainter->setRenderHint(QPainter::Antialiasing,true);
// 	}
 	pPainter->setRenderHint(QPainter::Antialiasing,true);//设置平滑
	QPen oldPen=pPainter->pen();
	QFont oldFont=pPainter->font();
	QBrush oldBrush=pPainter->brush();
	//数据准备
	QFont titleFont=oldFont;
	QFont scalFont=oldFont;
	QFont nameFont=oldFont;
	QFont valFont=oldFont;
	int nLine=pRs->GetColumns()-1;//折线个数
	int nrow=pRs->GetRows();//数据个数
	titleFont.setPixelSize(25);
	scalFont.setPixelSize(15);
	valFont.setPixelSize(15);
	nameFont.setPixelSize(15);
	int titleHeight=(rect.bottom-rect.top)/15;
	if (titleHeight>=35)
	{
		titleHeight=35;
	}
	if (titleHeight<25)
	{
		titleFont.setPixelSize(titleHeight);
	}
	int xLen=0;
	int yLen=0;
	int scalX=0;
	int scalY=0;
	int disX=0;
	int disY=0;
	float colSpace=0;//点之间间隔
	float fMax=0.0;//最大的值
	float fMin=0.0;//最大的值
	float fScal=1.0;//刻度最大值
	SString sColor;
	float ft=0.0;
	int nameLen=0;
	int lineNameLen=0;
	int nameMaxLen=0;//名称最大长度
	int lineNameMaxLen=0;//折线名称最大长度
	int scalMaxLen=0;//刻度最大长度
	int n=0;
	fMin=0;//pRs->GetValue(0,0).toFloat();
	for (int i=0;i<nrow;i++)
	{
		//if (i==0)
		//{
			nameLen=QFontMetrics(nameFont).width(pRs->GetValue(i,0).data());
			if (nameMaxLen<nameLen)
			{
				nameMaxLen=nameLen;
			}
		//	continue;
		//}
		for(int j=1;j<=nLine;j++)
		{
			sColor = pRs->GetValue(i,j);
			if(sColor.length() == 0)
				continue;
			ft=sColor.toFloat();
			if (fMax<ft)
			{
				fMax=ft;
			}
			if (fMin>ft)
			{
				fMin=ft;
			}
		}
	}
// 	if(fMin == fMax)
// 	{
// 		fMin -= fMin/10;
// 	}
	for (int i=0;i<=nLine;i++)
	{
		lineNameLen=QFontMetrics(nameFont).width(SString::GetIdAttribute(i+2,sTitle,",").data());//pRs->GetColumnName(i)
		if (lineNameMaxLen<lineNameLen)
		{
			lineNameMaxLen=lineNameLen;
		}
	}
	if(fMin>=yfrom&&fMax<=yto)
		useY=true;
	else
		useY=false;
	if(useY==true)
	{
		fMin=yfrom;fMax=yto;
	}
	
	//fMax=50000000;
	if (fMax<=50)
	{
		if (fMax>=1)
		{
			fScal=(int(fMax)/5)*5+5;
		}
		else
		{
			if (fMax>=0.5)
				fScal = 1.00;
			else
				fScal = 0.50;
		}
		n=fScal>1?fScal/5+1:fScal*4+1;
	}else
	{
		for(int i=0;i<8;i++)
		{
			fScal*=10;
			if (fMax<=fScal)
			{
				break;
			}
		}
		if (fMax<=fScal/2)
		{
			n=fMax/(fScal/20) +1;
			fScal=(fScal/20)*n;
		}
		else if(fMax==fScal)
		{

		}
		else
		{
			n=fMax/(fScal/10)+1;
			fScal=(fScal/10)*n;
		}
		n=n+1;
	}
	QString sScal;
	if(useY==true)
		fScal=yto;
	if (fScal<=1)
	{
		sScal.sprintf("%.2f",fScal);
	}else
	{
		sScal=QString("%L1").arg(fScal,0,'f',0);
	}

	SRect chartRect;//图标区域
	SRect legendRect;//图例区域
	int tempsize=15,tempsize1=15;
	if (rect.right-rect.bottom>rect.top)
	{
	}
	if (lineNameMaxLen+60>(rect.right-rect.left))
	{
		tempsize=15*(rect.right-rect.left)/(lineNameMaxLen+60);
		if (tempsize>1)
		{
			nameFont.setPixelSize(tempsize);
		}

	}
	if (nLine*tempsize+(40*tempsize)/15>(rect.bottom-rect.top-titleHeight))
	{
		tempsize1=tempsize*(rect.bottom-rect.top-titleHeight)/(nLine*tempsize+(40*tempsize)/15);
		if (tempsize1>1)
		{
			nameFont.setPixelSize(tempsize1);
		}
	}
	int tsize=nameFont.pixelSize();
	lineNameMaxLen=(lineNameMaxLen*nameFont.pixelSize())/15;
	nameMaxLen=(nameMaxLen*nameFont.pixelSize())/15;
	scalMaxLen=QFontMetrics(nameFont).width(sScal);
	if (tsize<5||rect.bottom-rect.top<15||rect.right-rect.left<15)
	{
		b_leg=false;
		chartRect.left=rect.left;
		chartRect.right=rect.right;
		chartRect.top=rect.top+titleHeight;
		chartRect.bottom=rect.bottom;
	}
	if (tempsize<=1||tempsize1<=1)
	{
		b_leg=false;
	}
	if (100*(rect.right-rect.left)/(lineNameMaxLen+(50*tsize)/15)<100*(rect.bottom-rect.top)/(nLine*nameFont.pixelSize()+(20*tsize)/15))//位于下侧
	{

		chartRect.left=rect.left;
		chartRect.right=rect.right;
		chartRect.bottom=rect.bottom-nLine*nameFont.pixelSize()-(40*tsize)/15;
		chartRect.top=rect.top+titleHeight;
		legendRect.left=rect.left+(10*tsize)/15;
		legendRect.right=rect.right-(10*tsize)/15;
		legendRect.top=chartRect.bottom+(10*tsize)/15;
		legendRect.bottom=rect.bottom-(10*tsize)/15;
		if (legendRect.bottom-legendRect.top>(rect.bottom-rect.top)/2)
		{
			b_leg=false;
			chartRect.left=rect.left;
			chartRect.right=rect.right;
			chartRect.top=rect.top+titleHeight;
			chartRect.bottom=rect.bottom;
		}
	}else//位于右侧
	{
		chartRect.left=rect.left;
		chartRect.right=rect.right-lineNameMaxLen-(70*tsize)/15;
		chartRect.top=rect.top+titleHeight;
		chartRect.bottom=rect.bottom;
		legendRect.left=chartRect.right+(10*tsize)/15;
		legendRect.right=rect.right-(10*tsize)/15;
		legendRect.top=rect.top+titleHeight+(10*tsize)/15;
		legendRect.bottom=rect.bottom-(10*tsize)/15;
		if (legendRect.right-legendRect.left>(rect.right-rect.left)/2)
		{
			b_leg=false;
			chartRect.left=rect.left;
			chartRect.right=rect.right;
			chartRect.top=rect.top+titleHeight;
			chartRect.bottom=rect.bottom;
		}
	}

	//pPainter->drawRect(chartRect.left,chartRect.top,chartRect.right-chartRect.left,chartRect.bottom-chartRect.top);

	xLen=chartRect.right-chartRect.left-scalMaxLen-20;
	colSpace=xLen/float((pRs->GetRows()));
// 	if (colSpace<1)
// 	{
// 		colSpace=0;
// 	}
	if (colSpace>0)
	{
//  		if (nameMaxLen>colSpace)
//  		{
//  			disY=((nameMaxLen*5)/(4*colSpace)+1)*15+3;
//  		}
		if (disY<15)
		{
			disY=15;
		}
		if (disY>=(chartRect.bottom-chartRect.top)/2)
		{
			disY=(chartRect.bottom-chartRect.top)/2;
		}
	}
	disX=scalMaxLen+(10*tsize/15);
	scalX=chartRect.left+scalMaxLen+(10*tsize)/15;
	scalY=chartRect.bottom-disY;
	yLen=chartRect.bottom-chartRect.top-disY-10;

	float sMin=0.0;
	for (int i=1;i<n;i++)
	{
		if(fScal*i/(n-1)>=fMin)
		{
			sMin=fScal*(i-1)/(n-1);
			break;
		}
	}
	if(!useY)
		fScal=fScal-sMin;
	else
	{
		sMin=yfrom;
		fScal=yto-yfrom;
	}
	//数据准备

	//绘制图例

	pPainter->setBrush(Qt::NoBrush);
	if (b_leg)
	{
		pPainter->setRenderHint(QPainter::Antialiasing,false);//设置平滑
		pPainter->setPen(QPen(QBrush(corLine),1));
		//pPainter->drawRect(legendRect.left,legendRect.top,legendRect.right-legendRect.left,legendRect.bottom-legendRect.top);
 		if(nLine*nameFont.pixelSize()+nameFont.pixelSize()/2+(10*tsize)/15>legendRect.bottom-legendRect.top)//显示不下截断多余的图例
 			pPainter->drawRect(legendRect.left,legendRect.top,legendRect.right-legendRect.left,legendRect.bottom-legendRect.top);
 		else
 			pPainter->drawRect(legendRect.left,legendRect.top,legendRect.right-legendRect.left,nLine*nameFont.pixelSize()+nameFont.pixelSize()/2+(10*tsize)/15);
		pPainter->setRenderHint(QPainter::Antialiasing,true);//设置平滑
		pPainter->setFont(nameFont);
		for(int i=0;i<nLine;i++)
		{
			if(legendRect.top+i*nameFont.pixelSize()+(10*tsize)/15>legendRect.bottom)//截断显示不下的图例
				break;
			sColor = SString::GetAttributeValue(sExtAttr,SString::toFormat("c%d",i+1));
			QColor color;
			if(sColor.length()>0)
				color = StrToQcor(sColor);
			else
				color = qRgb(((i+3)*101)%255,((i+3)*61)%201+((i+3)%2)*50,255-((i+3)%2)*150-((i+3)*21)%101);
			pPainter->setBrush(QBrush(color));
			pPainter->setPen(QPen(QBrush(color),1));
			pPainter->drawLine(legendRect.left+(5*tsize)/15,legendRect.top+i*nameFont.pixelSize()+nameFont.pixelSize()/2+(10*tsize)/15,legendRect.left+(35*tsize)/15,legendRect.top+i*nameFont.pixelSize()+nameFont.pixelSize()/2+(10*tsize)/15);
			pPainter->drawEllipse(QPoint(legendRect.left+(20*tsize)/15,legendRect.top+i*nameFont.pixelSize()+nameFont.pixelSize()/2+(10*tsize)/15),nameFont.pixelSize()/10,nameFont.pixelSize()/10);
			pPainter->setPen(corText);
			pPainter->drawText(legendRect.left+(40*tsize)/15,legendRect.top+i*nameFont.pixelSize()+(10*tsize)/15,lineNameMaxLen,nameFont.pixelSize(),Qt::AlignLeft,SString::GetIdAttribute(i+2,sTitle,",").data());
		}
	}
	//绘制图例

	//绘制标题
	if (titleHeight>10)
	{
		pPainter->setFont(titleFont);
		pPainter->setPen(corText);
		pPainter->drawText(rect.left,rect.top,rect.right-rect.left,titleHeight,Qt::AlignCenter,SString::GetIdAttribute(1,sTitle,",").data());
	}
	//绘制标题

	//绘制坐标系
	int num1=fScal;
	int num2=2;
	if(num1>0&&num1<=10)
		num2=2;
	else 
	{
		int n1=10;int n2=50;
		while(1)
		{
			if(n1<num1&&num1<=n2)
			{
				num2=n1;
				break;
			}
			if(n2<num1&&num1<=n1*10)
			{
				num2=n1*2;break;
			}
			n1=n1*10;n2=n2*10;
		}
	}
	n=num1/num2;
	int m=num1%num2;

	if(m!=0)
		n=n+2;
	else
		n=n+1;
	int scalHeight=yLen-yLen%5;//刻度最大长度和坐标系最大长度有误差
	if(n==1)
		return;
	//if (xLen>10&&yLen>10)
//	{
		pPainter->setPen(QPen(QBrush(corLine),1));
		pPainter->drawLine(scalX,scalY,scalX+xLen,scalY);
		pPainter->drawLine(scalX,scalY,scalX,scalY-yLen);
		//int n=fScal>1?fScal/5+1:fScal*2+1;
		pPainter->setFont(nameFont);
		for (int i=0;i<n;i++)
		{	
			if (scalHeight/(n-1)<8)
			{
				if (i%4!=0)
				{
					continue;
				}
			}
			if (scalHeight/(n-1)<15)
			{
				if (i%2==1)
					continue;
			}
			pPainter->setRenderHint(QPainter::Antialiasing,false);//设置平滑
			pPainter->setPen(QPen(QBrush(corLine),1,Qt::DotLine));
			pPainter->drawLine(scalX,scalY-(scalHeight*i)/(n-1),scalX+xLen,scalY-(scalHeight*i)/(n-1));
			pPainter->setRenderHint(QPainter::Antialiasing,true);//设置平滑
			if (disY>10&&disX>10)
			{
				pPainter->setPen(corText);
				if (fScal<=1)
					//pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(fScal*i/(n-1)+sMin,0,'f',3));
// 					if(i==n-1)
// 						pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(fScal+sMin,0,'f',3));
// 					else
						pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(num2*i+sMin,0,'f',3));
 				else
 					//pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(fScal*i/(n-1)+sMin,0,'f',0));
// 					if(i==n-1)
// 						pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(fScal+sMin,0,'f',0));
// 					else
						pPainter->drawText(rect.left,scalY-disY/2-(scalHeight*i)/(n-1),disX,disY,Qt::AlignCenter,QString("%L1").arg(num2*i+sMin,0,'f',0));
			}
		}
//	}

	//绘制坐标系
	if(fScal==0)
	{
		pPainter->setPen(oldPen);
		pPainter->setFont(oldFont);
		pPainter->setBrush(oldBrush);
		pPainter->setRenderHint(QPainter::Antialiasing,b_sm);
		return;
	}
	//绘制连线
	if (xLen>10&&yLen>10)
	{
		int colHeight1=0,colHeight2=0;
		pPainter->setFont(nameFont);
		int num=nrow;
		if(colSpace!=0)
			num=(nameMaxLen+10)/colSpace;
 		if(num==0)
 			num=1;
		SString val1,val2;
		for (int i=0;i<nLine+1;i++)
		{
			sColor = SString::GetAttributeValue(sExtAttr,SString::toFormat("c%d",i));
			QColor color;
			if(sColor.length()>0)
				color = StrToQcor(sColor);
			else
				color = qRgb(((i+2)*101)%255,((i+2)*61)%201+((i+2)%2)*50,255-((i+2)%2)*150-((i+2)*21)%101);
			for (int j=0;j<nrow;j++)
			{
				if (i==0)
				{
// 					if (colSpace<valFont.pixelSize())
// 					{
// 						if (j%2==1)
// 							continue;				
// 					}
					if (j%num!=0)
					{
						continue;
					}
					pPainter->setPen(QPen(QBrush(corText),1));
// 					if (disY>15)
// 					{
 						pPainter->drawText(scalX+colSpace*(j+1)-colSpace/*/2-colSpace/2*/,scalY,nameMaxLen+20,disY,Qt::AlignTop|Qt::TextWrapAnywhere,QString("%1").arg(pRs->GetValue(j,0).data()));
// 					}else
// 					{
// 						pPainter->drawText(scalX+colSpace*(j+1)-colSpace/2-colSpace/2,scalY,colSpace-colSpace/5,disY,Qt::AlignCenter,QString("%1").arg(pRs->GetValue(j,0).data()));
// 					}
					continue;
				}
				val1 = pRs->GetValue(j,i);
				val2 = pRs->GetValue(j+1,i);
				if(val1.length() == 0||val2.length() == 0)
					continue;
				colHeight1=scalHeight*(val1.toFloat()-sMin)/fScal;
				colHeight2=scalHeight*(val2.toFloat()-sMin)/fScal;
				if (j==nrow-1)
				{
					pPainter->drawEllipse(QPoint(scalX+colSpace*(j+1)-colSpace/*/2*/,scalY-colHeight1),tsize/10,tsize/10);
					continue;
				}
				if(scalY-colHeight1<0||scalY-colHeight2<0)
					continue;
				pPainter->setPen(QPen(QBrush(color),2));
				pPainter->drawLine(scalX+colSpace*(j+1)-colSpace/*/2*/,scalY-colHeight1,scalX+colSpace*(j+2)-colSpace/*/2*/,scalY-colHeight2);
				pPainter->setBrush(QBrush(color));
				pPainter->drawEllipse(QPoint(scalX+colSpace*(j+1)-colSpace/*/2*/,scalY-colHeight1),tsize/10,tsize/10);
			}
		}
	}

	//绘制连线
	pPainter->setPen(oldPen);
	pPainter->setFont(oldFont);
	pPainter->setBrush(oldBrush);
	pPainter->setRenderHint(QPainter::Antialiasing,b_sm);
}
void  CSpQtChartPainter::DrawPanel(QPainter *pPainter,SRect &rect,float currentValue,SString sExtAttr)
{
	//drawpie起点3点，逆时针
	double startAngle = 0.0;//起始角
	//double endAngle= 270.0;//结束角
	QBrush oldBrush=pPainter->brush();//还原用
	QPen oldPen=pPainter->pen();
	QFont oldFont=pPainter->font();
	pPainter->setRenderHint(QPainter::Antialiasing,true);//设置平滑
	pPainter->setPen(QPen(QColor(0,0,0),1));
	SString pcorext= SString::GetAttributeValue(sExtAttr,SString::toFormat("p_cor"));//圆环颜色

	SString pstartext=SString::GetAttributeValue(sExtAttr,SString::toFormat("p_start"));//最小值
	SString pendext=SString::GetAttributeValue(sExtAttr,SString::toFormat("p_end"));//最大值
	SString pbakgext=SString::GetAttributeValue(sExtAttr,SString::toFormat("p_bakgcor"));//背景色
	SString pdimeext=SString::GetAttributeValue(sExtAttr,SString::toFormat("dime"));//单位
	int pstart=pstartext.toInt();
	int pend=pendext.toInt();
	if(pendext.toInt()<pstartext.toInt())//检查数据正确
		pendext=pstartext;
	QVector<SString> vpcorext;//子段颜色
	SString str="";
	pPainter->setPen(QPen(QColor(0,0,0,0)));//透明线条
	for (int i=0;;i++)
	{
		str = SString::GetAttributeValue(sExtAttr,SString::toFormat("p_cor_%d",i));
		if (str!="")
			vpcorext.push_back(str);
		else
			break;
	}
	QVector<SString> vpcorcntext;
	str="";
	for (int i=0;;i++)//子段数值
	{
		str = SString::GetAttributeValue(sExtAttr,SString::toFormat("p_corcnt_%d",i));
		if (str!="")
			vpcorcntext.push_back(str);
		else
			break;
	}
	int height=rect.bottom-rect.top;
	int width=rect.right-rect.left;
	int rad=height/2>width/2?width/2:height/2;//取最小为直径
	//int angle=0;
	
	pPainter->setBrush(ScorToQcor(pcorext));//画圆
	pPainter->drawPie(rect.left+(width/2-rad),rect.top+(height/2-rad),
		rad*2,rad*2,int((225-startAngle)*16),int(-270*16));
	//int pointX=rect.left+width/2;//圆心
	//int pointY=rect.top+height/2;
	int cont=vpcorext.size()>vpcorcntext.size()?vpcorext.size():vpcorcntext.size();
	if(vpcorext.size()!=0)
	{
		int lastangle=0;
		for(int i=0;i<cont;i++)
		{
			SString tmp= vpcorcntext.at(i);
			int num=tmp.toInt();
			if(num>pend)//检查数值正确性
				num=pend;
			if(num<pstart)
				num=pstart;
			int angle=num*270/(pend-pstart);
			if(lastangle>angle)
				angle=lastangle;
			pPainter->setBrush(ScorToQcor(vpcorext.at(i)));
			pPainter->drawPie(rect.left+(width/2-rad),rect.top+(height/2-rad),//画子段
				rad*2,rad*2,int((225-lastangle)*16),int((lastangle-angle)*16));			
			lastangle=angle;
		}
	}
	//QColor color=ScorToQcor(pbakgext);
 	pPainter->setBrush(ScorToQcor(pbakgext));
  	pPainter->drawPie(rect.left+(width/2-rad*9/10),rect.top+(height/2-rad*9/10),//画内圆形成圆环
  		rad*2*9/10,rad*2*9/10,int((225-startAngle)*16),int(-360*16));	

	int angleMax=0;
	int j=0;		
	QFont fnt=oldFont;
	fnt.setPixelSize(rad/8);//刻度字体
	fnt.setFamily("黑体");
	if(currentValue<pstart)//检查当前数值正确
		currentValue=pstart;
	else if(currentValue>pend)
		currentValue=pend;
	//currentValue=4;//测试
	//int mincur=pend-currentValue;
	QColor curColor(ScorToQcor(pcorext));
	for(int i=0;i<cont;i++)
	{			
		SString tmp= vpcorcntext.at(i);
		int num=tmp.toInt();
		if(num>pend)//检查数值正确性
			num=pend;
		if(num<pstart)
			num=pstart;
		int angle=num*270/(pend-pstart);
		if(angleMax<angle)
			angleMax=angle;
		if(i == 0)
			curColor=ScorToQcor(vpcorext.at(i));
		if(currentValue >= num && i<cont-1)
			curColor=ScorToQcor(vpcorext.at(i+1));
// 		if(num>=currentValue&&num-currentValue<=mincur)
// 		{
// 			curColor=ScorToQcor(vpcorext.at(i));
// 			mincur=num-currentValue;
// 		}
		for(;j<270;j+=3)//画子段刻度
		{
			if(j>angle)
				break;
			if((j/3)%9==0)
			{
				pPainter->setPen(QPen(ScorToQcor(vpcorext.at(i)),2));
				pPainter->drawLine(rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10,
					rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10*18/20,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10*18/20);
				pPainter->setFont(fnt);
				int num=j*(pend-pstart)/270;
				if(j<135)
					pPainter->drawText(rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10*16/20,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10*16/20,QString::number(num));
				else
					pPainter->drawText(rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10*16/20-rad/8,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10*16/20,QString::number(num));
			}
			else
			{
				pPainter->setPen(QPen(ScorToQcor(vpcorext.at(i)),1));
				pPainter->drawLine(rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10,
					rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10*19/20,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10*19/20);
			}

		}
	}
	for(int j=angleMax;j<=270;j+=3)//补足刻度
	{
		if((j/3)%9==0)
		{
			pPainter->setPen(QPen(ScorToQcor(pcorext),2));
			pPainter->drawLine(rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10,
				rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10*18/20,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10*18/20);
			pPainter->setFont(fnt);
			int num=j*(pend-pstart)/270;
			if(j<135)
				pPainter->drawText(rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10*16/20,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10*16/20,QString::number(num));
			else
				pPainter->drawText(rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10*16/20-rad/8,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10*16/20,QString::number(num));
		}
		else
		{
			pPainter->setPen(QPen(ScorToQcor(pcorext),1));
			pPainter->drawLine(rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10,
				rect.left+(width/2)+cos((225-j)*0.017453)*rad*9/10*19/20,rect.bottom-(height/2)-sin((225-j)*0.017453)*rad*9/10*19/20);
		}
	}
 	pPainter->setPen(QPen(QColor(0,0,0,0)));//透明线条
	pPainter->setBrush(curColor);
	int curAngle=270*(currentValue-pstart)/(pend-pstart);//计算角度
	QPointF points[4] = {
		QPointF(rect.left+(width/2)+cos((225-curAngle)*0.017453)*rad*9/10,rect.bottom-(height/2)-sin((225-curAngle)*0.017453)*rad*9/10),
		QPointF(rect.left+(width/2)+cos((225-curAngle+90)*0.017453)*rad/20,rect.bottom-(height/2)-sin((225-curAngle+90)*0.017453)*rad/20),
		QPointF(rect.left+(width/2)+cos((225-curAngle+180)*0.017453)*rad/20,rect.bottom-(height/2)-sin((225-curAngle+180)*0.017453)*rad/20),
		QPointF(rect.left+(width/2)+cos((225-curAngle+270)*0.017453)*rad/20,rect.bottom-(height/2)-sin((225-curAngle+270)*0.017453)*rad/20)};
	pPainter->drawConvexPolygon(points,4);//绘制箭头
	fnt.setPixelSize(rad/4);//当前值字体
	fnt.setFamily("黑体");
	pPainter->setFont(fnt);
	pPainter->setPen(QPen(curColor,2));
	pPainter->drawText(rect.left+(width/2)-rad/2/*4*/,rect.bottom-height/2+rad/2,
		rad/*/2*/,rad/4,Qt::AlignCenter,
		QString::number(currentValue)+QString(pdimeext.data()));//当前数值
	//还原
	pPainter->setRenderHint(QPainter::Antialiasing,false);//设置平滑
	pPainter->setBrush(oldBrush);
	pPainter->setPen(oldPen);
	pPainter->setFont(oldFont);
	
}
QColor CSpQtChartPainter::ScorToQcor(SString srgb)
{
	if(srgb.length()<4)
		return qRgb(0,0,0);
	srgb = srgb.right(srgb.length()-4);
	srgb = srgb.left(srgb.length()-1);
	return qRgb(SString::GetIdAttribute(1,srgb,",").toInt(),SString::GetIdAttribute(2,srgb,",").toInt(),SString::GetIdAttribute(3,srgb,",").toInt());
}

QColor CSpQtChartPainter::StrToQcor(SString srgb)
{
	return qRgb(SString::GetIdAttribute(1,srgb,",").toInt(),SString::GetIdAttribute(2,srgb,",").toInt(),SString::GetIdAttribute(3,srgb,",").toInt());
}