/*
	resource.h

	IDM => IDentity of Menu
	IDS => IDentity of String
	IDC => IDentity of Component
	IDD => IDentity of Dialog box

	Note: the numbers should be within 16-bits
*/
#ifndef _RESOURCE_H
#define _RESOURCE_H


#define IDC_STATIC                      -1
#define IDC_MYICON                      2

#define IDS_WINDOW_CLASS                100
#define IDS_APP_TITLE                   101
#define IDD_ABOUTBOX                    102
#define IDD_GOTOBOX                     103
#define IDI_APP_ICON                    104
#define IDC_APP_MENU                    105
#define IDR_ACCELERATOR                 106
#define IDC_EDIT0                       0x480
#define IDC_EDIT1                       0x481

//--------------------------------------------

#define IDM_NEW                         30000
#define IDM_OPEN                        30001
#define IDM_SAVE                        30002
#define IDM_SAVEAS                      30003
#define IDM_RELOAD                      30004
#define IDM_PRINT                       30005
#define IDM_EXIT                        30006

#define IDM_UNDO                        30100
#define IDM_REDO                        30101
#define IDM_CUT                         30102
#define IDM_COPY                        30103
#define IDM_PASTE                       30104
#define IDM_DELETE                      30105
#define IDM_SELECTALL                   30106
#define IDM_CONVERT_TEXT                30107
#define IDM_FIND                        30108
#define IDM_REPLACE                     30109
#define IDM_GOTO                        30110

#define IDM_CONVERT_0                   30150
#define IDM_CONVERT_1                   30151
#define IDM_CONVERT_2                   30152
#define IDM_CONVERT_3                   30153
#define IDM_CONVERT_4                   30154
#define IDM_CONVERT_5                   30155
#define IDM_CONVERT_6                   30156
#define IDM_CONVERT_7                   30157
#define IDM_CONVERT_8                   30158
#define IDM_CONVERT_9                   30159
#define IDM_CONVERT_10                  30160
#define IDM_CONVERT_11                  30161
#define IDM_CONVERT_12                  30162
#define IDM_CONVERT_13                  30163
#define IDM_CONVERT_14                  30164
#define IDM_CONVERT_15                  30165

#define IDM_FULLSCREEN                  30200
#define IDM_FONT                        30201

#define IDM_STANDARD                    30202
#define IDM_SCIENTIFIC                  30203
#define IDM_STATISTICS                  30204
#define IDM_PROGRAMMING                 30205
#define IDM_BASIC_TEXT                  30206
#define IDM_UNICODE_TEXT                30207
#define IDM_RTEF_TEXT                   30208
#define IDM_VIEW_NONE                   30209

#define IDM_TABSTOP_0                   30210
#define IDM_TABSTOP_1                   30211
#define IDM_TABSTOP_2                   30212
#define IDM_TABSTOP_3                   30213
#define IDM_TABSTOP_4                   30214
#define IDM_TABSTOP_6                   30216
#define IDM_TABSTOP_8                   30218
#define IDM_TABSTOP_12                  30222
#define IDM_TABSTOP_16                  30226

#define IDM_TAKE_PICTURE                30300
#define IDM_SAVEALLOBJECTS              30301
#define IDM_REMOVEALLOBJECTS            30302

#define IDM_HELP                        30400
#define IDM_ABOUT                       30401

//--------------------------------------------

#define IDC_MAIN_TEXT                   30500

#define IDC_EVAL_BUTTON                 30501
#define IDC_MESG_TEXT                   30502
#define IDC_LOCK_BUTTON                 30503

#define IDC_PREV_BUTTON                 30504
#define IDC_NEXT_BUTTON                 30505
#define IDC_DELETE_BUTTON               30506
#define IDC_CLEAR_BUTTON                30507
#define IDC_PATH_TEXT                   30508

#define IDC_PAUSE_BUTTON                30509
#define IDC_FORWARD_BUTTON              30510
#define IDC_LOWER_BUTTON                30511
#define IDC_HIGHER_BUTTON               30512
#define IDC_TIME_TEXT                   30513

#define IDC_CALC_BUTTON                 30514
#define IDC_CALC_INPUT                  30515
#define IDC_CALC_RESULT                 30516

#define IDC_STATUS_BAR                  30517

//--------------------------------------------

#ifndef RT_MANIFEST
#define RT_MANIFEST                       24
#define CREATEPROCESS_MANIFEST_RESOURCE_ID 1
#endif

#endif
