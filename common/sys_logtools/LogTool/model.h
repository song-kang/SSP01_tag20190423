#ifndef MODEL_H
#define MODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVector>
#include <QDateTime>
struct myLog
{
	QString id;
	QString threadId;
	QString dateTime;
	QString logType;
	QString fileName;
	QString funName;
	QString line;
	QString msg;
};
class Model : public QAbstractTableModel
{
	Q_OBJECT

public:
	Model(QObject *parent);
	~Model();
public:   
	int rowCount(const QModelIndex &parent = QModelIndex()) const;   
	int columnCount(const QModelIndex &parent = QModelIndex()) const;   
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;   
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;   
	Qt::ItemFlags flags(const QModelIndex &index) const;   
	bool setData(const QModelIndex &index, const QVariant &value, int role); 
	void setInfo(QList<myLog> *logs);
	QList<myLog> *getInfo();
private:
	QList<myLog> *logs;

};

#endif // MODEL_H
