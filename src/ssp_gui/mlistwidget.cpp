#include "mlistwidget.h"

mListWidget::mListWidget(QWidget *parent)
	: QListWidget(parent)
{
	QObject::connect(this,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(OnItemClicked(QListWidgetItem*)));
	QObject::connect(this,SIGNAL(currentItemChanged (QListWidgetItem *,QListWidgetItem *)),this,SLOT(OnCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));
}

mListWidget::~mListWidget()
{

}
void mListWidget::OnItemClicked(QListWidgetItem*item)
{
	emit choosed(item);
}
void mListWidget::OnCurrentItemChanged(QListWidgetItem * current, QListWidgetItem * previous)
{
	S_UNUSED(previous);
	if (current==NULL)
		return;
	OnItemClicked(current);
}
