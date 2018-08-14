#define _USER_H

#define errChk

int Set_ATTR_DIMMED(int state);
int GetConfig(void);
int ReadAndSet(void);
unsigned char LRC( unsigned char *buf,unsigned short int len);
int COMScan(void);
int COM_Enumerate(void);
int Compare(const void*a,const void*b);
