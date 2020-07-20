#include <stdio.h>
#include "uklib.h"

int main()
{
	unsigned long status;
	unsigned int lednum;
	int ret;
	while(1)
	{
		printf("输入设定值：");
		scanf("%x\n%x",&lednum,&status);
		UK_UsrLed(lednum,status);
		sleep(3);
	}
	return 0;
}
