#define _USER_H



int Set_ATTR_DIMMED(int state);
int GetConfig(void);
int ReadAndSet(void);
unsigned char LRC( unsigned char *buf,unsigned short int len);
