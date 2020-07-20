#include "mpushbutton.h"

mPushButton::mPushButton(QWidget *parent)
	: QPushButton(parent)
{
	QObject::connect(this,SIGNAL(clicked()),this,SLOT(onPressed()));
	m_strName = "";
	m_nID = -1;
}

mPushButton::~mPushButton()
{

}
