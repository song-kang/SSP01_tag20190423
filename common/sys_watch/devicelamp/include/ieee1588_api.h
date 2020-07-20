#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

#define PTP_ENABLE      0x8001
#define PTP_DISABLE     0x8002
#define PTP_CHECK_EVENT 0x8003
#define PTP_SYNC_TIME   0x8004



int PTP_DeviceOpen(void);
void PTP_DeviceClose(int fd);
int PTP_EnableDevice(int fd);
int PTP_DisableDevice(int fd);
int PTP_GetPTPState(int fd);
#ifdef __cplusplus
}
#endif

