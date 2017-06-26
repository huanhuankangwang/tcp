#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>  
#include <net/if.h>


#include <gobleDefine.h>

#include "server.h"



#define EB_LOGE                     printf


#define VPSD_PARM_LEN                         (sizeof(VpsdParm))


pthread_t  connect_pid;
int        connect_flag = 0;

typedef struct udpSimulateTcp
{
/*客户端 (上创建服务器)*/
    struct sockaddr_in client_server_addr;//客户端(服务器)地址
    int    client_server_port;
    int    client_server_socket;
    char   client_server_ip[MAX_CHAR_IP_LEN];
	pthread_t tServer;

/*服务器 ip*/    
    struct sockaddr_in server_addr;//客户端(服务器)地址
    int    server_port;
    int    server_socket;
    char   server_ip[MAX_CHAR_IP_LEN];
}UdpSimulateTcp;


UdpSimulateTcp udpsimulatetcp;

VpsdParm ntohVpsdParm(VpsdParm parm)
{
    VpsdParm temp={0};

    temp.gop = ntohl(parm.gop);
    temp.gop_use= ntohl(parm.gop_use);
    temp.fps= ntohl(parm.fps);
    temp.fps_use= ntohl(parm.fps_use);
    temp.bitrate= ntohl(parm.bitrate);
    temp.bitrate_use= ntohl(parm.bitrate_use);
    temp.resolution= ntohl(parm.resolution);
    temp.resolution_use= ntohl(parm.resolution_use);

    return temp;
}


VpsdParm htonVpsdParm(VpsdParm parm)
{
    VpsdParm temp={0};

    temp.gop = htonl(parm.gop);
    temp.gop_use= htonl(parm.gop_use);
    temp.fps= htonl(parm.fps);
    temp.fps_use= htonl(parm.fps_use);
    temp.bitrate= htonl(parm.bitrate);
    temp.bitrate_use= htonl(parm.bitrate_use);
    temp.resolution= htonl(parm.resolution);
    temp.resolution_use= htonl(parm.resolution_use);

    return temp;
}


void print_parm(VpsdParm parm)
{
    char str[100]="";
	char str2[100]="";
    sprintf(str," gop = %d gop_use =%d fps=  %d fps_use =%d bitrate =%d bitrate_use =%d",parm.gop ,parm.gop_use,parm.fps,parm.fps_use,parm.bitrate,parm.bitrate_use);
    if(parm.resolution_use)
    {
        if(parm.resolution == 2)
            sprintf(str2,"resolution is 1080P \r\n");
        else
            sprintf(str2,"resolution is 720P \r\n");
    }else
        sprintf(str2," not resolution \r\n");


	printf("wangkang receive vpsdparm is %s %s",str,str2);
}

void do_parser_parm(VpsdParm vpsdparm)
{
   
    VpsdParm tmp = ntohVpsdParm(vpsdparm);

    print_parm(tmp);


	//add receive message to queue
	//vpsd_do_parser_parm(tmp);
}


void vpsd_send_parm(struct sockaddr_in * addr)
{
	int socket;
	VpsdParm parm;
	
	//parm  = vpsd_get_currnet_parm();

	parm  = htonVpsdParm(parm);
	
	char *cmd = (char*)(&parm);

	sendto(socket, cmd, sizeof(VpsdParm), 0 , (struct sockaddr *)addr,sizeof(struct sockaddr_in));
}

int getLocalIp(char *localIp)  
{
    int sockfd;
    char ipaddr[50];
  
    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;
  
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
         EB_LOGE("socket create failse...getLocalIp!\r\n");  
         return -1;
    }  
     
    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);
   
    if( ioctl( sockfd, SIOCGIFADDR, &ifr_ip) < 0 )     
    {     
         return -1;     
    }       
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));
      
    EB_LOGE("local ip:%s \r\n",ipaddr); 
    close( sockfd);  
      
    return 0;  
}  



void sendSetupOk(struct sockaddr_in * addr)
{
    int socket;
    VpsdParm parm;
    memset(&parm,0,sizeof(parm));
    memcpy(&parm,SETUP_SUCESS,strlen(SETUP_SUCESS));

    char *cmd = (char*)(&parm);

    socket  = udpsimulatetcp.client_server_socket;
    sendto(socket, cmd, sizeof(VpsdParm), 0 , (struct sockaddr *)addr,sizeof(struct sockaddr_in));
}

void *MessageSocketServerThread(void *arg)
{
    struct sockaddr_in recvAddr;
    socklen_t servaddr_len;
    fd_set read_fds;
    struct timeval selectTimeout;
    int ret;

    VpsdParm vpsdparm ={0};    
    memset(&vpsdparm,0,VPSD_PARM_LEN);
    int addr_len = sizeof(struct sockaddr_in);

    selectTimeout.tv_sec  = 2;
    selectTimeout.tv_usec = 0;

    while(1)
	{
        FD_ZERO(&read_fds);
        FD_SET(udpsimulatetcp.client_server_socket, &read_fds);

        ret = select(udpsimulatetcp.client_server_socket + 1, &read_fds, 0, 0, &selectTimeout);
        if(ret <= 0) 
		{
            continue;
        } 
		else 
        {
           
            memset(&vpsdparm,0,VPSD_PARM_LEN);

            ret = recvfrom(udpsimulatetcp.client_server_socket, (char *)(&vpsdparm), VPSD_PARM_LEN, 0,
                           (struct sockaddr *)&recvAddr, &addr_len);
             //EB_LOGE("recvfrom ret: %d pRecvBuf =%s \r\n", ret,pRecvBuf);
            if(ret != VPSD_PARM_LEN)
            {
				EB_LOGE("recv = %s",(char*)(&vpsdparm));
            	//if(strcmp((char*)(&vpsdparm),SETUP) == 0)
           		{
           			
                    //获取ip和端口号 保存至本地

                    strcpy(udpsimulatetcp.server_ip,inet_ntoa(recvAddr.sin_addr));
                    EB_LOGE("new connect come %s:\r\n",udpsimulatetcp.server_ip);
					sendSetupOk(&recvAddr);
           		}
                continue;
            }
            
            do_parser_parm(vpsdparm);
        }
    }
    
    if (udpsimulatetcp.client_server_socket >= 0) 
    {
        close(udpsimulatetcp.client_server_socket);
    }

    return NULL;
}


/*建立起 客户端服务器*/
int do_setup_client_server(int  client_port)
{
    struct sockaddr_in servAddr;
    int server_port = -1;
    socklen_t servaddr_len;
    fd_set read_fds;
    struct timeval selectTimeout;
    int ret,sockfd;

    /*create socket*/
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd < 0) 
    {
        EB_LOGE("wangkang socket error");
        return -1;
    }

    /*bind*/
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(client_port);
    if((ret = bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0) 
	{
        EB_LOGE("wangkang bind error: %d, %s", ret, strerror(errno));
        close(sockfd);
        return -1;
    }

//拷贝至全局
    memcpy(&udpsimulatetcp.client_server_addr,&servAddr,sizeof(struct sockaddr_in));
    udpsimulatetcp.client_server_port   =  client_port;
    //memcpy(udpsimulatetcp.client_server_ip,server_ip,MAX_CHAR_IP_LEN);
    udpsimulatetcp.client_server_socket  = sockfd;

    //启动UDP消息接收线程
    if (pthread_create(&udpsimulatetcp.tServer, NULL, MessageSocketServerThread, NULL) != 0) 
    {
        EB_LOGE("create thread fail !");
        return -1;
    }

    EB_LOGE("create udp server service port:%d \r\n",client_port);
    return 0;
}


int do_setup_connection_thread(void *arg)
{
    char  setup[] = SETUP;
    int   setupLen = strlen(setup);
    int   ret = 0, addr_len = sizeof(struct sockaddr_in);

    char  readbuf[100] = "";
    int   retryTimes = (5+1);

    fd_set read_fds,write_fds;
    struct timeval selectTimeout;
    selectTimeout.tv_sec  = 5;
    selectTimeout.tv_usec = 0;

    UdpSimulateTcp  *mArg =  (UdpSimulateTcp*)arg;

    while(!connect_flag)
    {
        //当未连接到服务器 会一直重新连接

        //告诉服务器 客户端的ip，以后就可以主动直接 将消息发送至该ip
        retryTimes--;

        if(!retryTimes)
        {
            return -1;
        }

        FD_ZERO(&write_fds);
        FD_SET(udpsimulatetcp.client_server_socket, &write_fds);

        ret = select(udpsimulatetcp.client_server_socket + 1, 0, &write_fds, 0, &selectTimeout);
        if(ret <= 0) 
		{
            goto _continue;
        } 
        ret = sendto(mArg->server_socket, setup, setupLen, 0, (struct sockaddr *)&mArg->server_addr, sizeof(struct sockaddr_in));;
        if(ret !=  setupLen)
        {
            sleep(5);
            goto _continue;
        }

        FD_ZERO(&read_fds);
        FD_SET(udpsimulatetcp.client_server_socket, &read_fds);
        ret = select(udpsimulatetcp.client_server_socket + 1, &read_fds, 0, 0, &selectTimeout);
        if(ret <= 0) 
		{
            goto _continue;
        } 


        ret = recvfrom(mArg->server_socket,readbuf,sizeof(readbuf),0,(struct sockaddr *)&mArg->server_addr,(socklen_t *)&addr_len);
        if(ret > 0)
        {
            if(strcmp(readbuf,SETUP_SUCESS) == 0)
            {
                connect_flag = 1;//已经连接到服务器上。
                break;
            }
            
        }

        _continue:
        sleep(5);
        EB_LOGE("connect to servers %s:%d,will connect 5 minutes later\r\n",udpsimulatetcp.server_ip,udpsimulatetcp.server_socket);
        //接收返回消息 若返回成功 则表明正常建立
        //将消息发送至消息队列中去，后面会接收返回的消息。
    }

    return 0;
}

/*建立起与服务器连接的通道*/
/*此时服务器接受，并获取客户端的ip 此后可以用这个ip 来进行通讯*/
int do_setup_connection(char *server_ip,int server_port)
{
    struct sockaddr_in server_addr;
    int  sockfd,ret;
    
	bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    /* 建立socket，注意必须是SOCK_DGRAM */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("socket");
        return -1;
    }

    //fcntl(sockfd,F_SETFL,O_NONBLOCK);

    memcpy(&udpsimulatetcp.server_addr,&server_addr,sizeof(struct sockaddr_in));
    udpsimulatetcp.server_port   =  server_port;
    memcpy(udpsimulatetcp.server_ip,server_ip,MAX_CHAR_IP_LEN);
    udpsimulatetcp.server_socket  = sockfd;

    return do_setup_connection_thread((void*)(&udpsimulatetcp));
}

/*将消息发送给 服务器*/
void sendMessage(int sockfd, char *buf , int len)
{
    int addr_len = sizeof(struct sockaddr_in);
    sendto(sockfd, buf, len, 0, (struct sockaddr *)&udpsimulatetcp.server_addr, sizeof(struct sockaddr_in));
}


int MessageSocketServerInit()
{  
    int ret = 0; 
    connect_flag = 0;

    memset(&udpsimulatetcp,0 ,sizeof(udpsimulatetcp));


    ret = getLocalIp(udpsimulatetcp.client_server_ip);
    if(ret)
    {
        EB_LOGE("getLocalIp err!\r\n");
        return -1;        
    }

    //创建本地服务器  用于接收消息
    ret = do_setup_client_server(SERVER_PORT);
    if(ret)
    {
        EB_LOGE("create local server failed!\r\n");
        return -1;
    }
    EB_LOGE("create local server sucess!\r\n");
    
    //与服务器进行 建立连接

/*
    ret = do_setup_connection(SERVER_IP,SERVER_PORT);
    if(ret)
    {
        EB_LOGE("connect to server %s:%d  error!\r\n",SERVER_IP,SERVER_PORT);
        return -1;
    }
*/
    return 0;
}

int main()
{
    EB_LOGE("system client test !\r\n");
    if ( MessageSocketServerInit() )
    {
        EB_LOGE("MessageSocketServerInit err\r\n");

        return -1;
    }
    
    while(1);
    return 0;
}
