#ifndef __LIVEPLUGIN_H__
#define __LIVEPLUGIN_H__

#include <stdint.h>
#include <Windows.h>

//插件版本号，当此版本号不等于最新版本号时，插件加载失败
#define PLUGIN_VERSION		20151125

//图像像素格式，像素内存模型如下(从左到右为内存增长方向)
enum PluginPixelFormat
{
	PPF_NONE,
	PPF_ARGB32,   //B0 B1 B2 B3 B4 B5 B6 B7 G0 G1 G2 G3 G4 G5 G6 G7 R0 R1 R2 R3 R4 R5 R6 R7 A0 A1 A2 A3 A4 A5 A6 A7
	PPF_BGRA32,   //R0 R1 R2 R3 R4 R5 R6 R7 G0 G1 G2 G3 G4 G5 G6 G7 B0 B1 B2 B3 B4 B5 B6 B7 A0 A1 A2 A3 A4 A5 A6 A7
	PPF_I420,     //YUV 420 12bit 平面格式，分为Y、U、V三个平面 Y0 Y1 Y2 Y3  U0 U1   V0 V1
	PPF_NV12,     //YUV 420 12bit 平面格式，分为Y、UV两个平面   Y0 Y1 Y2 Y3  U0V0 U1V1
	PPF_RGB565,   //B0 B1 B2 B3 B4 G0 G1 G2 G3 G4 G5 R0 R1 R2 R3 R4
	PPF_RGB555,   //B0 B1 B2 B3 B4 G0 G1 G2 G3 G4 R0 R1 R2 R3 R4 A0
	PPF_YUY2,     //YUV 422 16bit 打包格式，只有一个平面 Y0U0Y1V0 Y2U1Y3V1
	PPF_UYVY,     //YUV 422 16bit 打包格式，只有一个平面 U0Y0V0Y1 U1Y2V1Y3
	PPF_YVYU,	  //YUV 422 16bit 打包格式，只有一个平面 Y0V0Y1U0 Y2V1Y3U1
	PPF_YV12,     //YUV 420 12bit 平面格式，分为Y、U、V三个平面 Y0 Y1 Y2 Y3  V0 V1   U0 U1
};

//音频格式
enum PluginAudioFormat
{
	PAF_NONE,
	PAF_S16,
	PAF_S32,
	PAF_FLOAT,
};

//图像源类型
enum ImageSourceType
{
	IST_CAMERA,									//摄像头数据
	IST_IMAGE,									//图片数据
	IST_VIDEO,									//视频文件数据
	IST_PLUGIN,									//插件图像数据
};

typedef struct _PluginElementParam
{
	int videoWidth;								//图像宽度
	int videoHeight;							//图像高度
	PluginPixelFormat pf;						//像素格式
	int pitch[3];								//行距

	PluginAudioFormat af;		
	int audioSampleRate;						//音频采样率
	int audioChannels;							//音频声道数
	int audioFramesize;							//一次采样数
	int audioSound;								//音量值

	int pid;									//插件源id
	float dstTop;								//显示区域左顶点Y坐标值,[0,1]归一化坐标
	float dstLeft;								//显示区域左顶点X坐标值
	float dstWidth;								//显示区域宽度
	float dstHeight;							//显示区域高度
	int zorder;									//显示的先后顺序，该值越大则越在上层显示
	float rotationX;							//绕X轴旋转角度
	float rotationY;							//绕Y轴旋转角度
	float rotationZ;							//绕Z轴旋转角度
	int invalidatePicture;						//是否需要对图像进行更新，0不更新，大于0则更新，为0的时候不会调用ReadVideo函数
}PluginElementParam;

//调用者应该继承实现下列部分接口
class IPluginElement
{
public:
	virtual bool Open(char* env){return false;}

	//读取图像
	virtual bool ReadVideo(uint8_t* pixels,int bufsize){return false;}

	//读取音频
	virtual bool ReadAudio(uint8_t* pcm,int bufsize){return false;}

	//销毁插件
	virtual bool Destroy(){return false;}

	//获取参数
	virtual bool GetParameters(PluginElementParam* param){return false;}

	//进行设置
	virtual void SetConfig(){}

	//通知开始直播
	virtual void OnStartLive(){}

	//通知停止直播
	virtual void OnStopLive(){}

	//弹幕消息：消息内容，发送用户，用户酬勤等级
	virtual void OnChatMessage(int id,wchar_t* msg,wchar_t* sendername,int deserveLev){}

	//赠送鱼丸消息:用户id，用户昵称，送鱼丸数量

	virtual void OnDonaterMessage(int id,wchar_t* senderName,int sliver){}

	//酬勤消息：用户ID，用户昵称，赠送数量，赠送等级
	virtual void OnDeserveMessage(int id,wchar_t* senderName,int num,int lev){}

	//用户进入房间消息：
	virtual void OnUserEnterMessage(wchar_t* senderName,int lev){}

	//获取当前状态
	virtual char* GetCurrentStatus(){return NULL;}

	//图像源类型，可以做一些视频后处理
	//pixels:像素，如果为非YUV 平面类型，则只有pixels[0]有效
	//param:表面图像的宽高、pitch大小、以及显示坐标和Z值序
	//type:数据源类型
	virtual bool OnCaptureData(uint8_t* pixels[3],PluginElementParam* param,ImageSourceType type){return false;}

	//是否显示插件
	virtual void ShowPlugin(bool show){}

	//礼物消息
	//id：用户id
	//sendername:赠送者昵称
	//giftname:礼物名称
	//giftnum:赠送的礼物数量
	virtual void OnGiftMessage(int id,wchar_t* sendername,wchar_t* giftname,int giftnum){};

	//所有网络消息，需要自己解析
	virtual void OnNetworkMessage(const wchar_t* msg){};


	//分发windows系统消息
	virtual void OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){}

};

//调用者应该继承实现该接口
class IPlugin
{
public:
	//获取插件名称
	virtual wchar_t* GetName() = 0;

	virtual IPluginElement* CreateElement() = 0;

	//获取插件的描述
	virtual wchar_t* GetDescription(){return NULL;}

	//获取帮助文档的URL链接
	virtual wchar_t* GetHelpUrl(){return NULL;}
};

//插件上下文运行环境
class IPluginContext
{
public:
	//发送弹幕聊天信息
	//参数msg:UNICODE编码
	virtual bool SendChatMessage(const wchar_t* msg) = 0;

	//获取登陆名
	virtual bool GetUserName(char* name) = 0;

	//获取登陆的用户ID
	virtual bool GetUserID(int* id) = 0;

	//是否正在直播
	virtual bool IsBroadcasting(int* broad) = 0;

	//获取LiveEncoder指针
	virtual class ILiveEncoder* GetLiveEncoder() = 0;

	//获取网络接口指针，包含所有相关的斗鱼逻辑
	virtual class ILiveencoderNetService* GetNetService() = 0;
};


#ifndef DYPLUGIN_EXPORT
#define DYPLUGIN_LOAD extern "C" __declspec(dllimport)
#else
#define DYPLUGIN_LOAD extern "C" __declspec(dllexport)
#endif

//加载插件
DYPLUGIN_LOAD IPlugin* dyLoadPlugin(IPluginContext* context);

//卸载插件
DYPLUGIN_LOAD void dyUnloadPlugin(IPlugin* plugin);

//导出函数，确定插件的版本号，开发者不需要对此函数进行任何的修改
DYPLUGIN_LOAD inline int GetPluginVersion()
{
	return PLUGIN_VERSION;
}

#endif