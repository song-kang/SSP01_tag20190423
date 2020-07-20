#include "model.h"

Model::Model(QObject *parent)   
	:  QAbstractTableModel(parent) 
{
	logs=NULL;
}

Model::~Model()
{

}

QVariant Model::data( const QModelIndex &index, int role ) const   
{   
	if (role == Qt::DisplayRole)    
	{    
		if (index.column() == 0)   
 			return logs->at(index.row()).id; 
		if (index.column() == 1)   
			return logs->at(index.row()).threadId;    
		if (index.column() == 2)   
			return logs->at(index.row()).dateTime;
		if (index.column() == 3)   
			return logs->at(index.row()).logType;
		if (index.column() == 4) 
			if(logs->at(index.row()).funName=="")
				return logs->at(index.row()).fileName;
			else
				return logs->at(index.row()).fileName+"("+logs->at(index.row()).funName+")";
		if (index.column() == 5) 
			return logs->at(index.row()).line;
		if (index.column() == 6) 
			return logs->at(index.row()).msg;
	}
	return QVariant();   
}

int Model::rowCount(const QModelIndex &parent) const
{
	if(logs!=NULL)
		return logs->size();
	return 0;
}

int Model::columnCount(const QModelIndex &parent) const
{
	return 7;
}

Qt::ItemFlags Model::flags( const QModelIndex &index ) const   
{   
	if(!index.isValid())   
		return 0;   
	return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;   
}   


bool Model::setData( const QModelIndex &index, const QVariant &value, int role )   
{   
	if(!index.isValid())   
		return false;   
	if (index.isValid() && index.row() != index.column()   && role == Qt::EditRole) 
	{  
		QModelIndex transposedIndex = createIndex(index.column(),index.row());  
		emit dataChanged(index, index);  
		emit dataChanged(transposedIndex, transposedIndex);  
	}  
	return true;   

}   
QVariant Model::headerData(int section, Qt::Orientation orientation , int role) const  
{  
	if (role == Qt::DisplayRole &&orientation==Qt::Horizontal)  
	{
		if(section==0)
			return tr("序号");
		if(section==1)
			return tr("线程号");
		if(section==2)
			return tr("时间");
		if(section==3)
			return tr("级别");
		if(section==4)
			return tr("源文件");
		if(section==5)
			return tr("行号");
		if(section==6)
			return tr("日志内容");
	}
	if (role == Qt::DisplayRole &&orientation==Qt::Vertical)
		return section;  
	return QVariant();  
}  

void Model::setInfo(QList<myLog> *logs)
{
	this->logs= logs;
	reset();
}

QList<myLog> * Model::getInfo()
{
	return logs;
}