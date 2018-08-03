#include <windows.h>  
#include <formatio.h>
#include "toolbox.h"
#include <utility.h>
#include <ansi_c.h>
#include <rs232.h>
#include <cvirte.h>		
#include <userint.h>
#include "serial.h"
#include "user.h"

char TxBuf[1024],
	 RxBuf[1024],
	 File_1_Name[MAX_PATHNAME_LEN],    
     File_2_Name[MAX_PATHNAME_LEN];
int PANEL_TOGGLEBUTTON_2_State,
	PANEL_TOGGLEBUTTON_State,			   
    ComPortState=-1,
	File_1_Handle,
	File_2_Handle;

struct CONFIG{
    char mark[3];//设备标识
	unsigned int scan;//测量间隔
	unsigned char port[9];//依次为v1,v2,v3,v4,f1,d1,d2,sw12-1,sw12-2
	unsigned char mode;//串口(zigbee)工作方式选择：ascii自动发送或者modbus从机
	unsigned char modbus_addr;//modbus从机地址
	unsigned int baudrate;//串口(zigbee)波特率选择	    
	char sdi12[2][440];//D1口SDI-12指令
	unsigned char lrc;
}sDL212_Config;
 
unsigned char COM=3,
			  QuitCtrl=1,
			  GetConfig_Flag=1;
static const char * DaysOfWeek[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };  

static int panelHandle,panelHandle_1,panelHandle_2;

int main (int argc, char *argv[])
{
	int len,i;
	ssize_t filesize;
	char dirname[MAX_PATHNAME_LEN];
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "serial.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle_1 = LoadPanel (0, "serial.uir", PANEL_1)) < 0)
		return -1;
	if ((panelHandle_2 = LoadPanel (0, "serial.uir", PANEL_2)) < 0)
		return -1;
	GetProjectDir (dirname);	   
	sprintf (File_1_Name, "%s\\D1-SDI12.log", dirname);
	File_1_Handle = OpenFile (File_1_Name, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_ASCII); 
	ReadFile (File_1_Handle, &sDL212_Config.sdi12[0][0], 440);
	CloseFile(File_1_Handle);
    sprintf (File_2_Name, "%s\\D2-SDI12.log", dirname);
    File_2_Handle = OpenFile (File_2_Name, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_ASCII); 
    ReadFile (File_2_Handle, &sDL212_Config.sdi12[1][0], 440);  
	CloseFile(File_2_Handle);        	  
	
	COM = 3;
	DisplayPanel (panelHandle);
	RecallPanelState (panelHandle, "panel_state", 0);  
	ResetTextBox (panelHandle, PANEL_TEXTBOX, 0);    
	SetCtrlVal (panelHandle, PANEL_TOGGLEBUTTON, 0);
    SetCtrlVal (panelHandle, PANEL_TOGGLEBUTTON_2, 0);
	GetCtrlVal (panelHandle, PANEL_RING_12, &COM);  
	
	SetCtrlAttribute(panelHandle_1,PANEL_1_TEXTBOX,ATTR_MAX_ENTRY_CHARS ,440);
	SetCtrlAttribute(panelHandle_2,PANEL_2_TEXTBOX,ATTR_MAX_ENTRY_CHARS ,440);
	SetCtrlAttribute(panelHandle_1,PANEL_1_TEXTBOX,ATTR_WRAP_MODE,VAL_CHAR_WRAP);
	SetCtrlAttribute(panelHandle_2,PANEL_2_TEXTBOX,ATTR_WRAP_MODE,VAL_CHAR_WRAP);
    SetCtrlAttribute(panelHandle,PANEL_TEXTBOX,ATTR_WRAP_MODE,VAL_CHAR_WRAP);  
	SetSleepPolicy (VAL_SLEEP_MORE);
	//RunUserInterface ();
	while(QuitCtrl){
	    ProcessSystemEvents();	
	    if(1 == PANEL_TOGGLEBUTTON_State){   
			if(ComPortState >= 0){						    
			    len = GetInQLen (COM);
		        if(len){
					len = 0;
					ComRdTerm (COM, RxBuf, 256,0x0A);
	          	    SetCtrlVal(panelHandle, PANEL_TEXTBOX,RxBuf);   
					memset(RxBuf,0,256);
		   		} 
			}
	    }
	}
	DiscardPanel (panelHandle);
	return 0;
}
  
int CVICALLBACK MainPanelCB (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:	
			SavePanelState (panelHandle, "panel_state", 0);
			QuitCtrl = 0;
			QuitUserInterface(0);
			break;
	}
	return 0;
}
 
int CVICALLBACK D1_SDI12_CmdCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel (panelHandle_1); 
			memset(&sDL212_Config.sdi12[0][0],0,440);
 			File_1_Handle = OpenFile (File_1_Name, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII); 
		    ReadFile (File_1_Handle, &sDL212_Config.sdi12[0][0], 440); 
			CloseFile(File_1_Handle);
		    ResetTextBox(panelHandle_1,PANEL_1_TEXTBOX,&sDL212_Config.sdi12[0][0]);
			break;
	}
	return 0;
}

int CVICALLBACK D1_SDI12_Command_InitCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int len=0;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle_1, PANEL_1_TEXTBOX, ATTR_STRING_TEXT_LENGTH, &len);			 
			memset(&sDL212_Config.sdi12[0][0],0,440); 
			GetCtrlVal(panelHandle_1,PANEL_1_TEXTBOX  ,&sDL212_Config.sdi12[0][0]);
			if(len > 1){
			    if(sDL212_Config.sdi12[0][len-1] != '\n'){
			        sDL212_Config.sdi12[0][len]='\r',sDL212_Config.sdi12[0][len+1]='\n';
				}
			}  
			File_1_Handle = OpenFile (File_1_Name, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
			WriteFile (File_1_Handle, &sDL212_Config.sdi12[0][0], 440);
			CloseFile(File_1_Handle);
	    	HidePanel(panelHandle_1);      
			break;
	}
	return 0;
}

int CVICALLBACK D1_SDI12_Command_DeInitCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:														    
			HidePanel(panelHandle_1);
			break;
	}
	return 0;
}

int CVICALLBACK D2_SDI12_CmdCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel (panelHandle_2); 
			memset(&sDL212_Config.sdi12[1][0],0,440);
 			File_2_Handle = OpenFile (File_2_Name, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII); 
		    ReadFile (File_2_Handle, &sDL212_Config.sdi12[1][0], 440); 
			CloseFile(File_2_Handle);
		    ResetTextBox(panelHandle_2,PANEL_2_TEXTBOX,&sDL212_Config.sdi12[1][0]);
			break;
	}
	return 0;
}

int CVICALLBACK D2_SDI12_Command_InitCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int len=0;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle_2, PANEL_2_TEXTBOX, ATTR_STRING_TEXT_LENGTH, &len);	
			memset(&sDL212_Config.sdi12[1][0],0,440);
			GetCtrlVal(panelHandle_2,PANEL_2_TEXTBOX  ,&sDL212_Config.sdi12[1][0]);
			if(len > 1){
			    if(sDL212_Config.sdi12[1][len-1] != '\n'){
			        sDL212_Config.sdi12[1][len]='\r',sDL212_Config.sdi12[1][len+1]='\n';
				}
			} 
			File_2_Handle = OpenFile (File_2_Name, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
			WriteFile (File_2_Handle, &sDL212_Config.sdi12[1][0], 440);
			CloseFile(File_2_Handle);
	    	HidePanel(panelHandle_2);      
			break;
	}
	return 0;
}

int CVICALLBACK D2_SDI12_Command_DeInitCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(panelHandle_2);      
			break;
	}
	return 0;
}
  
int CVICALLBACK WriteConfigCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{   
	int i=0,j=0,len,progress_dialog;
	char buf[66],lrc=0;
    char *p;
	
	switch (event)
	{
		case EVENT_COMMIT:
			FlushOutQ (COM);  
			FlushInQ (COM);     
			strcpy(buf,"DL212 Configuration Utility Write");
			len = ComWrt (COM,buf,34);   
		    SyncWait (Timer(),0.02);
			p = (char *)&sDL212_Config;
			i = sizeof(sDL212_Config)/60+1; 
			sDL212_Config.lrc = 0;
			sDL212_Config.lrc = LRC((unsigned char *)p,sizeof(sDL212_Config)-4);   
			for(j=0;j<i-1;j++){
			    len = ComWrt (COM,(const char*)(p+j*60),60); 
				SyncWait (Timer(),0.02);
			} 
		 	len = ComWrt (COM,(const char*)(p+j*60),sizeof(sDL212_Config)-j*60);   
			SyncWait (Timer(),0.02);													   
			memset(RxBuf,0,10);
			len = ComRdTerm (COM, RxBuf, 10,0x0A);  
			if(len == -99){
				MessagePopup ("","   未响应    ");    
				return -1;
			}
            //progress_dialog = CreateProgressDialog ("Completion Status", "Percent Complete", 1, VAL_NO_INNER_MARKERS, "确定"); 	 
			//UpdateProgressDialog (progress_dialog, 10, 0);
	        if(0 == strncmp("lrc ok",RxBuf,6)){
		    	MessagePopup ("","    发送完成，擦写FLASH需要3秒左右的时间，请不要立刻断电或者再次发送配置    ");
			}
			else if(0 == strncmp("lrc error",RxBuf,9)){
				MessagePopup ("","    通讯校验错误    ");
			}
			else{
				MessagePopup ("","    通讯超时    ");  	
			}    				  
			break;
	}
	return 0;
}

int CVICALLBACK ReadConfigCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{   
	int i=0,len=0;
	unsigned char lrc=0;
	char byte,buf[33];
    char *p;
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetConfig_Flag = 0;
			p = &sDL212_Config;
			FlushOutQ (COM);  
			FlushInQ (COM);  
			strcpy(buf,"DL212 Configuration Utility Read");
			len = ComWrt (COM,buf,33);
			while(i < sizeof(sDL212_Config)){
				byte = ComRdByte (COM);   
			 	if(byte == -99){
				    MessagePopup ("","   未响应    ");    
					return -1;
				}
				else{
					*(p+i) = byte;
				}
				i++;
			}
			lrc = LRC((unsigned char *)&sDL212_Config,sizeof(sDL212_Config)-4);
			if(lrc == sDL212_Config.lrc){
			    MessagePopup ("","   读取参数完成    ");  
				ReadAndSet();
			}
			else{
			    MessagePopup ("","   读取参数校验错误    ");  
			}
			GetConfig_Flag = 1;
			break;
	}
	return 0;	
	
}

int CVICALLBACK OpenComCB (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	int res=0;
	
	switch (event)
	{
		case EVENT_COMMIT:  
			GetCtrlVal (panelHandle, PANEL_TOGGLEBUTTON_2, &PANEL_TOGGLEBUTTON_2_State); 
	        if(0 == PANEL_TOGGLEBUTTON_2_State){
				ComWrt (COM, "value display off", 18);  
				Set_ATTR_DIMMED(1);								   
				SetCtrlVal (panelHandle, PANEL_TOGGLEBUTTON, 0);
				CloseCom (COM);
				ComPortState = -1;
			}
			else{	
				ComPortState = OpenComConfig (COM, "", 115200, 0, 8, 1, 1024, 1024); 
				if(ComPortState < 0){
					SetCtrlVal (panelHandle, PANEL_TOGGLEBUTTON_2, 0);
				    MessagePopup ("","         串口打开失败         ");         
				}
				else{
					SetComTime(COM,2);
					FlushOutQ (COM);
				    Set_ATTR_DIMMED(0);      
				}	  
			}  
			break;
	}
	return 0;
}

int CVICALLBACK SelectComCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle, PANEL_RING_12, &COM);
			break;
	}
	return 0;
}
   

int CVICALLBACK ValueDisplayCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			FlushOutQ (COM);  
			FlushInQ (COM);
			GetCtrlVal (panelHandle, PANEL_TOGGLEBUTTON, &PANEL_TOGGLEBUTTON_State); 
	        if(0 == PANEL_TOGGLEBUTTON_State){							    
				ComWrt (COM, "value display off", 18);
				SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_3,ATTR_DIMMED,0);
				SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_4,ATTR_DIMMED,0);
                SetCtrlAttribute (panelHandle, PANEL_DECORATION, ATTR_FRAME_COLOR, VAL_PANEL_GRAY);    
				ProcessDrawEvents();				    
			}
			else{														          
				ComWrt (COM, "value display on", 17);	
				SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_3,ATTR_DIMMED,1); 
				SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_4,ATTR_DIMMED,1);
				SetCtrlAttribute (panelHandle, PANEL_DECORATION, ATTR_FRAME_COLOR, VAL_RED); 
				ProcessDrawEvents();     
			}
			break;
	}
	return 0;
}

int menuHandle;
int CVICALLBACK TextBoxCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	int i;
	switch (event)
	{
		case EVENT_RIGHT_CLICK:
		    menuHandle = LoadMenuBar(0,"serial.uir", MENUBAR);
            i = RunPopupMenu (menuHandle, MENUBAR_MENU1, panel, eventData1,eventData2,1,1,1,1);
        break;
	}
	return 0;
}

void CVICALLBACK ClearBoxCB(int menubar, int menuItem, void *callbackData, int panel)
{
    ResetTextBox (panelHandle, PANEL_TEXTBOX, 0);     
}   

int CVICALLBACK TimerCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned int year, month, day, hour, min, sec, weekDay;
	char buffer[512];
	CVIAbsoluteTime absTime;
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			GetCurrentCVIAbsoluteTime(&absTime);
			CVIAbsoluteTimeToLocalCalendar(absTime, &year, &month, &day, &hour, &min, &sec, 0, &weekDay);
			sprintf(buffer, "%02d:%02d:%02d  %s %02d/%02d/%04d", hour, min, sec, 
		    DaysOfWeek[weekDay], month, day, year);
			SetCtrlVal(panel, PANEL_CLOCK, buffer); 
			
			GetConfig();
			SavePanelState (panelHandle, "panel_state", 0);
			break;
	}
	return 0;
}

int Set_ATTR_DIMMED(int state){
    SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_3,ATTR_DIMMED,state);
	SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_4,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_STRING         ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_NUMERIC        ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_NUMERIC_2      ,ATTR_DIMMED,state);  
    SetCtrlAttribute(panelHandle,PANEL_RING_11        ,ATTR_DIMMED,state);  
    SetCtrlAttribute(panelHandle,PANEL_RING_10        ,ATTR_DIMMED,state);  
    SetCtrlAttribute(panelHandle,PANEL_RING_9         ,ATTR_DIMMED,state);  
    SetCtrlAttribute(panelHandle,PANEL_RING_8         ,ATTR_DIMMED,state);  
    SetCtrlAttribute(panelHandle,PANEL_RING_7         ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_RING_6         ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_RING_5         ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_RING_4         ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_RING_3         ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_RING_2         ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_RING           ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_2,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON  ,ATTR_DIMMED,state);
    SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_3,ATTR_DIMMED,state);   
	SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_4,ATTR_DIMMED,state);
	SetCtrlAttribute(panelHandle,PANEL_TOGGLEBUTTON   ,ATTR_DIMMED,state);
	
	return 0;
}

int GetConfig(void){
	if(1 == GetConfig_Flag){
  		GetCtrlVal(panelHandle,PANEL_RING_12  ,&COM);
   	    GetCtrlVal(panelHandle,PANEL_STRING   ,&sDL212_Config.mark[0]);
        GetCtrlVal(panelHandle,PANEL_RING_11  ,&sDL212_Config.mode);
    	GetCtrlVal(panelHandle,PANEL_NUMERIC_2,&sDL212_Config.modbus_addr);
    	GetCtrlVal(panelHandle,PANEL_RING     ,&sDL212_Config.port[0]);
    	GetCtrlVal(panelHandle,PANEL_RING_2   ,&sDL212_Config.port[1]);
    	GetCtrlVal(panelHandle,PANEL_RING_3   ,&sDL212_Config.port[2]);
    	GetCtrlVal(panelHandle,PANEL_RING_4   ,&sDL212_Config.port[3]);
    	GetCtrlVal(panelHandle,PANEL_RING_5   ,&sDL212_Config.port[4]);
    	GetCtrlVal(panelHandle,PANEL_RING_6   ,&sDL212_Config.port[5]);
    	GetCtrlVal(panelHandle,PANEL_RING_7   ,&sDL212_Config.port[6]);
    	GetCtrlVal(panelHandle,PANEL_RING_8   ,&sDL212_Config.port[7]);
    	GetCtrlVal(panelHandle,PANEL_RING_9   ,&sDL212_Config.port[8]);
    	GetCtrlVal(panelHandle,PANEL_RING_10  ,&sDL212_Config.baudrate);
    	GetCtrlVal(panelHandle,PANEL_NUMERIC  ,&sDL212_Config.scan);
		if(0 == ComPortState){
	        if(1 == sDL212_Config.port[5]){
	  		    SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON,ATTR_DIMMED,0);    
			}
			else{
       		 	SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON,ATTR_DIMMED,1);	
			}
			if(1 == sDL212_Config.port[6]){
				SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_2,ATTR_DIMMED,0);
			}
			else{
        		SetCtrlAttribute(panelHandle,PANEL_COMMANDBUTTON_2,ATTR_DIMMED,1);	
			}
		}
	}
    return 0;
}

int ReadAndSet(void){							 
    SetCtrlVal(panelHandle,PANEL_STRING   ,&sDL212_Config.mark[0]);
    SetCtrlVal(panelHandle,PANEL_RING_11  ,sDL212_Config.mode);
    SetCtrlVal(panelHandle,PANEL_NUMERIC_2,sDL212_Config.modbus_addr);
    SetCtrlVal(panelHandle,PANEL_RING     ,sDL212_Config.port[0]);
    SetCtrlVal(panelHandle,PANEL_RING_2   ,sDL212_Config.port[1]);
    SetCtrlVal(panelHandle,PANEL_RING_3   ,sDL212_Config.port[2]);
    SetCtrlVal(panelHandle,PANEL_RING_4   ,sDL212_Config.port[3]);
    SetCtrlVal(panelHandle,PANEL_RING_5   ,sDL212_Config.port[4]);
    SetCtrlVal(panelHandle,PANEL_RING_6   ,sDL212_Config.port[5]);
    SetCtrlVal(panelHandle,PANEL_RING_7   ,sDL212_Config.port[6]);
    SetCtrlVal(panelHandle,PANEL_RING_8   ,sDL212_Config.port[7]);
    SetCtrlVal(panelHandle,PANEL_RING_9   ,sDL212_Config.port[8]);
    SetCtrlVal(panelHandle,PANEL_RING_10  ,sDL212_Config.baudrate);
    SetCtrlVal(panelHandle,PANEL_NUMERIC  ,sDL212_Config.scan);
    SetCtrlVal(panelHandle_1,PANEL_1_TEXTBOX  ,&sDL212_Config.sdi12[0][0]);
    File_1_Handle = OpenFile (File_1_Name, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
	WriteFile (File_1_Handle, &sDL212_Config.sdi12[0][0], 440);
	CloseFile(File_1_Handle);
	SetCtrlVal(panelHandle_2,PANEL_2_TEXTBOX  ,&sDL212_Config.sdi12[1][0]);
    File_2_Handle = OpenFile (File_2_Name, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
	WriteFile (File_2_Handle, &sDL212_Config.sdi12[1][0], 440);
	CloseFile(File_1_Handle);
	
    return 0;
}

unsigned char LRC( unsigned char *buf,unsigned short int len)
{
    unsigned char lrc=0;

    while( len-- )
    {
        lrc += *buf++;   /* Add buffer byte without carry */
    }

    /* Return twos complement */
    lrc = (unsigned char)(-((char)lrc));
    return lrc;
}

