#ifndef _MANCONNECT_H
#define _MANCONNECT_H
#include <list>
#include "SApi.h"
#include "SFile.h"
#include "db/mdb/SMdb.h"
class ManConnect
{
public:
	ManConnect(void);
	~ManConnect(void);
	void add(SMdb * sdb);
	void del(int i_idx);
	int getSize(){return list_connect.size();}
	SMdb * operator[](int i_index);
public:
	struct SMdb_info
	{
		SMdb* p;
		int i_index;
	};
private:
	list<SMdb*> list_connect;
};
#endif


