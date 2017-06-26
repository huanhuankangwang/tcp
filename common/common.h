#ifndef _COMMON_H_
#define _COMMON_H_


	/*
	*  错误值
	*/
	enum {
	    EasybusErr_NoError                  =   0,
	    EasybusErr_Unknown                  =  -1,
	    EasybusErr_NoMemory                 =  -2,
	    EasybusErr_BadParam                 =  -3,
	    EasybusErr_Invalid                  =  -4,
	    EasybusErr_OpenCtrlConnFail         =  -5,
	    EasybusErr_OpenMonitorConnFail      =  -6,
	    EasybusErr_AttachFail               =  -7,
	    EasybusErr_SocketPairFail           =  -8,
	    EasybusErr_DetachFail               =  -9,
	    EasybusErr_InterestFail             =  -10,
	    EasybusErr_CtrlConnNoExist          =  -11,
	    EasybusErr_MonitorConnNoExist       =  -12,
	    EasybusErr_ReceiveFail              =  -13,
	    EasybusErr_ReceiveEOF               =  -14,
	    EasybusErr_SendFail                 =  -15,

	};

#define EASYBUS_ADDR_MAX_LEN (24)
#define EASYBUS_MSGTYPE_MAX_LEN (16)
#define EASYBUS_MSGDATA_MAX_LEN (1024)

typedef struct {
    char ip[EASYBUS_ADDR_MAX_LEN]; /*ip地址，格式: 192.168.1.2*/
    unsigned short port; /*端口号*/
} EasybusAddr;

typedef struct {
    EasybusAddr remoteAddr; /*远端（如手机、PAD）应用的地址*/
    char mode; /*模式：单播，广播，通知*/
    char msgType[EASYBUS_MSGTYPE_MAX_LEN + 1]; /*消息类型*/
    char msgData[EASYBUS_MSGDATA_MAX_LEN]; /*消息数据*/
    int msgDataSize; /*消息数据大小*/
} EasybusMsg;

#endif//_COMMON_H_
