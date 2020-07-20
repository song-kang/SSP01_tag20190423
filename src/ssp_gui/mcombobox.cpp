#include "mcombobox.h"

mCombobox::mCombobox(QWidget *parent,int type)
	: QComboBox(parent),m_type(type)
{
	m_row = 0;
	m_column =0;
	if (m_type==0)
	{
		QObject::connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(changeToWidget(int)));
	}else if (m_type==1)
	{
		QObject::connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(changeToTree(int)));
	}	
}

mCombobox::~mCombobox()
{

}
void mCombobox::changeToWidget(int index)
{
	S_UNUSED(index);
	emit whitch(m_tableItem,m_column);
}

void mCombobox::changeToTree(int index)
{
	S_UNUSED(index);
	emit whitch(m_treeItem,m_column);
}