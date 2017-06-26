#ifndef _COMMON_H_
#define _COMMON_H_


	/*
	*  ����ֵ
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
    char ip[EASYBUS_ADDR_MAX_LEN]; /*ip��ַ����ʽ: 192.168.1.2*/
    unsigned short port; /*�˿ں�*/
} EasybusAddr;

typedef struct {
    EasybusAddr remoteAddr; /*Զ�ˣ����ֻ���PAD��Ӧ�õĵ�ַ*/
    char mode; /*ģʽ���������㲥��֪ͨ*/
    char msgType[EASYBUS_MSGTYPE_MAX_LEN + 1]; /*��Ϣ����*/
    char msgData[EASYBUS_MSGDATA_MAX_LEN]; /*��Ϣ����*/
    int msgDataSize; /*��Ϣ���ݴ�С*/
} EasybusMsg;

#endif//_COMMON_H_
