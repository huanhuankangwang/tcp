#ifndef _CLIENT_H_
#define _CLIENT_H_


typedef struct _parm{
	int gop;//i 帧
	int gop_use;
	
	int fps;//帧率
	int fps_use;
	
	int resolution;// 1 720p ,2 1080p
	int resolution_use;
	
	int bitrate;//比特率
	int bitrate_use; 
}VpsdParm;


#endif
