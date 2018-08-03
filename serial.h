/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: MainPanelCB */
#define  PANEL_STRING                     2       /* control type: string, callback function: (none) */
#define  PANEL_NUMERIC                    3       /* control type: numeric, callback function: (none) */
#define  PANEL_RING_7                     4       /* control type: ring, callback function: (none) */
#define  PANEL_RING_6                     5       /* control type: ring, callback function: (none) */
#define  PANEL_RING_5                     6       /* control type: ring, callback function: (none) */
#define  PANEL_RING_4                     7       /* control type: ring, callback function: (none) */
#define  PANEL_RING_3                     8       /* control type: ring, callback function: (none) */
#define  PANEL_RING_2                     9       /* control type: ring, callback function: (none) */
#define  PANEL_RING                       10      /* control type: ring, callback function: (none) */
#define  PANEL_COMMANDBUTTON_2            11      /* control type: command, callback function: D2_SDI12_CmdCB */
#define  PANEL_COMMANDBUTTON              12      /* control type: command, callback function: D1_SDI12_CmdCB */
#define  PANEL_RING_9                     13      /* control type: ring, callback function: (none) */
#define  PANEL_RING_8                     14      /* control type: ring, callback function: (none) */
#define  PANEL_RING_10                    15      /* control type: ring, callback function: (none) */
#define  PANEL_NUMERIC_2                  16      /* control type: numeric, callback function: (none) */
#define  PANEL_RING_11                    17      /* control type: ring, callback function: (none) */
#define  PANEL_COMMANDBUTTON_4            18      /* control type: command, callback function: ReadConfigCB */
#define  PANEL_COMMANDBUTTON_3            19      /* control type: command, callback function: WriteConfigCB */
#define  PANEL_TOGGLEBUTTON               20      /* control type: textButton, callback function: ValueDisplayCB */
#define  PANEL_TEXTBOX                    21      /* control type: textBox, callback function: TextBoxCB */
#define  PANEL_RING_12                    22      /* control type: ring, callback function: SelectComCB */
#define  PANEL_TOGGLEBUTTON_2             23      /* control type: textButton, callback function: OpenComCB */
#define  PANEL_TIMER                      24      /* control type: timer, callback function: TimerCB */
#define  PANEL_CLOCK                      25      /* control type: string, callback function: (none) */
#define  PANEL_DECORATION                 26      /* control type: deco, callback function: (none) */

#define  PANEL_1                          2
#define  PANEL_1_TEXTBOX                  2       /* control type: textBox, callback function: (none) */
#define  PANEL_1_COMMANDBUTTON_2          3       /* control type: command, callback function: D1_SDI12_Command_DeInitCB */
#define  PANEL_1_COMMANDBUTTON            4       /* control type: command, callback function: D1_SDI12_Command_InitCB */

#define  PANEL_2                          3
#define  PANEL_2_TEXTBOX                  2       /* control type: textBox, callback function: (none) */
#define  PANEL_2_COMMANDBUTTON_2          3       /* control type: command, callback function: D2_SDI12_Command_DeInitCB */
#define  PANEL_2_COMMANDBUTTON            4       /* control type: command, callback function: D2_SDI12_Command_InitCB */


     /* Control Arrays: */

#define  CTRLARRAY                        1

     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_MENU1                    2
#define  MENUBAR_MENU1_ITEM1              3       /* callback function: ClearBoxCB */


     /* Callback Prototypes: */

void CVICALLBACK ClearBoxCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK D1_SDI12_CmdCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK D1_SDI12_Command_DeInitCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK D1_SDI12_Command_InitCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK D2_SDI12_CmdCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK D2_SDI12_Command_DeInitCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK D2_SDI12_Command_InitCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenComCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ReadConfigCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SelectComCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TextBoxCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ValueDisplayCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK WriteConfigCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
