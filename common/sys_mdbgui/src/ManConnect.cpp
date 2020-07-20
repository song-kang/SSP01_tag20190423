#include "ManConnect.h"


ManConnect::ManConnect(void)
{
}


ManConnect::~ManConnect(void)
{
	for (list<SMdb*>::iterator it=list_connect.begin();it!=list_connect.end();it++)
	{
		if ((*it)!=NULL)
		{
			delete *it;
		}
	}
}
SMdb * ManConnect::operator[](int i_index)
{
	list<SMdb*>::iterator ipos = list_connect.begin();
	while(--i_index>=0)
	{
		ipos++;
	}
	return *ipos;
}
void ManConnect::add(SMdb * sdb)
{
	list_connect.push_back(sdb);
}
void ManConnect::del(int i_idx)
{
	list<SMdb*>::iterator ipos = list_connect.begin();
	while(--i_idx>=0)
	{
		ipos++;
	}

	(*ipos)->DisConnect();
	delete *ipos;
		list_connect.erase(ipos);
}