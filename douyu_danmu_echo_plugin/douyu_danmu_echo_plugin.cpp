// douyu_danmu_echo_plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "plugin.h"
#include "common.h"
//std headers
#include <cassert>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
//lua
//lua
#include "selene.h"
//std headers
#include <string>
#include <cassert>
#include <codecvt>
#include <thread>
#include <mutex>
#include <chrono>

#define WITH_LUA 1

template <typename T>
static std::wstring toWString(const T& src)
{
	std::wstringstream ss;
	ss << src;
	return ss.str();
}
static std::string utf16_to_utf8(const std::wstring& src)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	const std::string result = conv.to_bytes(src);
	return result;
}
static std::wstring utf8_to_utf16(const std::string& src)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	const std::wstring result = conv.from_bytes(src);
	return result;
}
class Logger
{
public:
	static void logging(const std::wstring& content)
	{
		Logger::instance().inner_logging(content);
	}
private:
	Logger()
	{
		ofs.open(LR"(danmu_echo_plugin_log.txt)");
	}
	~Logger()
	{
		ofs.flush();
	}
	static Logger& instance()
	{
		static Logger inst;
		return inst;
	}
	void inner_logging(const std::wstring& content)
	{
		const auto t = time(nullptr);
		const auto local = localtime(&t);
		ofs << L"[" << local->tm_hour << L":" << local->tm_min << L":"<<local->tm_sec<<"]";
		ofs << L" " << content << std::endl;
	}
private:
	std::wofstream ofs;
};
class LuaContext
{
public:
	static std::wstring on_chat_msg(const std::wstring& user, const std::wstring& msg, const int deserveLev)
	{
		return LuaContext::instance().inner_on_chat_msg(user, msg, deserveLev);
	}
	static std::wstring on_user_gift(const std::wstring& user, const std::wstring& gift, const int number)
	{
		return LuaContext::instance().inner_on_user_gift(user, gift, number);
	}
	static std::wstring on_user_donater(const std::wstring& user, const int sliver)
	{
		return LuaContext::instance().inner_on_user_donater(user, sliver);
	}
	static std::wstring on_user_deserve(const std::wstring& user, const int num, const int level)
	{
		return LuaContext::instance().inner_on_user_deserve(user, num, level);
	}
	static std::wstring on_user_enter(const std::wstring& user, const int level)
	{
		return LuaContext::instance().inner_on_user_enter(user, level);
	}
	static std::wstring on_automsg()
	{
		return LuaContext::instance().inner_on_automsg();
	}
	static int get_interval_time()
	{
		return LuaContext::instance().inner_get_interval_time();
	}
private:
	LuaContext()
	{
		const auto success = load_lua_file();
		assert(success);
	}
	~LuaContext()
	{
		close_lua();
	}
	static LuaContext& instance()
	{
		static LuaContext inst;
		return inst;
	}
	bool load_lua_file()
	{
		Logger::logging(L"LuaContext::load_lua_file lua_file_path = " + lua_file_path);
		return state.Load(utf16_to_utf8(lua_file_path));
	}
	void close_lua()
	{
	}
private:
	std::wstring inner_on_chat_msg(const std::wstring& user, const std::wstring& msg, const int deserveLev)
	{
		load_lua_file();
		//convert input from utf16 to utf8		
		const std::string utf8_output = state["inner_on_chat_msg"](utf16_to_utf8(user), utf16_to_utf8(msg), deserveLev);
		//convert output from utf8 to utf16
		return utf8_to_utf16(utf8_output);
	}
	std::wstring inner_on_user_gift(const std::wstring& user, const std::wstring& gift, const int number)
	{
		load_lua_file();
		//convert input from utf16 to utf8		
		const std::string utf8_output = state["inner_on_user_gift"](utf16_to_utf8(user), utf16_to_utf8(gift), number);
		//convert output from utf8 to utf16
		return utf8_to_utf16(utf8_output);
	}
	std::wstring inner_on_user_donater(const std::wstring& user, const int sliver)
	{
		load_lua_file();
		//convert input from utf16 to utf8		
		const std::string utf8_output = state["inner_on_user_donater"](utf16_to_utf8(user), sliver);
		//convert output from utf8 to utf16
		return utf8_to_utf16(utf8_output);
	}
	std::wstring inner_on_user_deserve(const std::wstring& user, const int num, const int level)
	{
		load_lua_file();
		//convert input from utf16 to utf8		
		const std::string utf8_output = state["inner_on_user_deserve"](utf16_to_utf8(user), num, level);
		//convert output from utf8 to utf16
		return utf8_to_utf16(utf8_output);
	}
	std::wstring inner_on_user_enter(const std::wstring& user, const int level)
	{
		load_lua_file();
		//convert input from utf16 to utf8		
		const std::string utf8_output = state["inner_on_user_enter"](utf16_to_utf8(user), level);
		//convert output from utf8 to utf16
		return utf8_to_utf16(utf8_output);
	}
	std::wstring inner_on_automsg()
	{
		load_lua_file();
		//convert input from utf16 to utf8		
		const std::string utf8_output = state["inner_on_automsg"]();
		//convert output from utf8 to utf16
		return utf8_to_utf16(utf8_output);
	}
	int inner_get_interval_time()
	{
		load_lua_file();
		//convert input from utf16 to utf8		
		return state["inner_get_interval_time"]();
	}
private:
	const std::wstring lua_file_path = LR"(reply.lua)";
	sel::State state;
};
class PluginContext : public IPluginElement
{
public:
	PluginContext(IPluginContext* _handler)
		:handler(_handler)
	{
		Logger::logging(L"PluginContext::PluginContext begin.");
		handler->GetUserID(&own_uid);
		Logger::logging(L"PluginContext::PluginContext own_uid = " + toWString<int>(own_uid));
		Logger::logging(L"PluginContext::PluginContext end.");
	}
	virtual ~PluginContext()
	{
		Logger::logging(L"~PluginContext::~PluginContext begin.");
		Logger::logging(L"~PluginContext::~PluginContext end.");
	}
public:
	void start_sendmsg_thread()
	{
		if (!still_active)
		{
			auto_sendmsg_thread = std::thread(&PluginContext::auto_sendmsg_cb, this);
		}
	}
	void stop_sendmsg_thread()
	{
		exit_flag = true;
		if (still_active)
		{			
			auto_sendmsg_thread.join();			
		}
	}
	void auto_sendmsg_cb()
	{
		auto last_time = std::chrono::system_clock::now();
		while (!exit_flag)
		{
			still_active = true;
			const auto current_time = std::chrono::system_clock::now();
			//ms
			const auto interval_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time).count();
			//sec
			const int interval_time = LuaContext::get_interval_time();
			if (interval_time > interval_time *1000)
			{
				//do send msg
				send_automsg();
				last_time = current_time;
			}			
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
		still_active = false;
	}
	void send_automsg()
	{
		Logger::logging(L"PluginContext::send_automsg begin.");
#if !WITH_LUA
		std::wstring response;
		response += std::wstring(L"��Һã�������������д����ing...");
		handler->SendChatMessage(response.c_str());
#else
		const std::wstring response = LuaContext::on_automsg();
		handler->SendChatMessage(response.c_str());
#endif
		Logger::logging(L"PluginContext::send_automsg end.");
	}
public:
	//////////////////////////////////////////////////////////////////////////
	virtual bool Open(char* env) 
	{
		Logger::logging(L"PluginContext::Open called.");
		start_sendmsg_thread();
		return true; 
	}
	//���ٲ��
	virtual bool Destroy() 
	{ 
		Logger::logging(L"PluginContext::Destroy called.");
		stop_sendmsg_thread();
		return true; 
	}
	//��Ļ��Ϣ����Ϣ���ݣ������û����û����ڵȼ�
	virtual void OnChatMessage(int id, wchar_t* msg, wchar_t* senderName, int deserveLev) override
	{
		Logger::logging(L"PluginContext::OnChatMessage begin.");
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�ID��") + toWString<int>(id));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�����") + toWString<wchar_t*>(senderName));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û����ڵȼ���") + toWString<int>(deserveLev));
		Logger::logging(toWString<wchar_t*>(L"��Ϣ���ݣ�") + toWString<wchar_t*>(msg));

		assert(handler);
		if (own_uid != id)
		{
#if !WITH_LUA
			std::wstring response;
			response += senderName + std::wstring(L"����ã������Ϣ�ǣ�");
			response += std::wstring(msg);
			handler->SendChatMessage(response.c_str());
#else
			const std::wstring response = LuaContext::on_chat_msg(senderName, msg, deserveLev);
			handler->SendChatMessage(response.c_str());
#endif
		}
		Logger::logging(L"PluginContext::OnChatMessage end.");
	}
	//����������Ϣ:�û�id���û��ǳƣ�����������
	virtual void OnDonaterMessage(int id, wchar_t* senderName, int sliver)  override
	{
		Logger::logging(L"PluginContext::OnDonaterMessage begin.");
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�ID��") + toWString<int>(id));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�����") + toWString<wchar_t*>(senderName));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û��������������") + toWString<int>(sliver));

		assert(handler);
		if (own_uid != id)
		{
#if !WITH_LUA
			std::wstring response;
			response += senderName + std::wstring(L"����ã�лл������裡��л��");
			handler->SendChatMessage(response.c_str());
#else
			const std::wstring response = LuaContext::on_user_donater(senderName, sliver);
			handler->SendChatMessage(response.c_str());
#endif
		}
		Logger::logging(L"PluginContext::OnDonaterMessage end.");
	}
	//������Ϣ���û�ID���û��ǳƣ��������������͵ȼ�
	virtual void OnDeserveMessage(int id, wchar_t* senderName, int num, int lev)  override
	{
		Logger::logging(L"PluginContext::OnChatMessage begin.");
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�ID��") + toWString<int>(id));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�����") + toWString<wchar_t*>(senderName));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�����������") + toWString<int>(num));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û����ڵȼ���") + toWString<int>(lev));

		assert(handler);
		if (own_uid != id)
		{
#if !WITH_LUA
			std::wstring response;
			response += senderName + std::wstring(L"����ã�лл��ĳ��ڣ�������ɶ��");
			handler->SendChatMessage(response.c_str());
#else
			const std::wstring response = LuaContext::on_user_deserve(senderName, num, lev);
			handler->SendChatMessage(response.c_str());
#endif
		}
		Logger::logging(L"PluginContext::OnDeserveMessage end.");
	}
	//������Ϣ
	//id���û�id
	//sendername:�������ǳ�
	//giftname:��������
	//giftnum:���͵���������
	virtual void OnGiftMessage(int id, wchar_t* senderName, wchar_t* giftname, int giftnum) override
	{
		Logger::logging(L"PluginContext::OnChatMessage begin.");
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�ID��") + toWString<int>(id));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�����") + toWString<wchar_t*>(senderName));
		Logger::logging(toWString<wchar_t*>(L"�������ƣ�") + toWString<wchar_t*>(giftname));
		Logger::logging(toWString<wchar_t*>(L"����������") + toWString<int>(giftnum));

		assert(handler);
		if (own_uid != id)
		{
#if !WITH_LUA
			std::wstring response;
			response += senderName + std::wstring(L"����ã�лл������");
			handler->SendChatMessage(response.c_str());
#else
			const std::wstring response = LuaContext::on_user_gift(senderName, giftname, giftnum);
			handler->SendChatMessage(response.c_str());
#endif
		}
		Logger::logging(L"PluginContext::OnGiftMessage end.");
	}
	//�û����뷿����Ϣ��
	virtual void OnUserEnterMessage(wchar_t* senderName, int lev) 
	{
		Logger::logging(L"PluginContext::OnChatMessage begin.");		
		Logger::logging(toWString<wchar_t*>(L"��Դ�û�����") + toWString<wchar_t*>(senderName));
		Logger::logging(toWString<wchar_t*>(L"��Դ�û��ȼ���") + toWString<int>(lev));

		assert(handler);
#if !WITH_LUA
		std::wstring response;
		response += senderName + std::wstring(L"����ã���ӭ���ٱ�ª�ң�");
		handler->SendChatMessage(response.c_str());
#else
		const std::wstring response = LuaContext::on_user_enter(senderName, lev);
		handler->SendChatMessage(response.c_str());
#endif
		Logger::logging(L"PluginContext::OnUserEnterMessage end.");
	}
	//�Ƿ���ʾ���
	virtual void ShowPlugin(bool show) 
	{
		Logger::logging(L"PluginContext::ShowPlugin begin.");
		Logger::logging(L"PluginContext::ShowPlugin end.");
	}
private:
	IPluginContext* handler = nullptr;
	int own_uid = -1;
	//��ʱ������Ϣ���߳�
	std::thread auto_sendmsg_thread;
	bool exit_flag = false;
	bool still_active = false;
};

class DanmuEchoPlugin : public IPlugin
{
public:
	DanmuEchoPlugin(IPluginContext* _handler)
		:handler(_handler)
	{
		Logger::logging(L"DanmuEchoPlugin::DanmuEchoPlugin begin.");
		Logger::logging(L"DanmuEchoPlugin::DanmuEchoPlugin end.");
	}
	~DanmuEchoPlugin()
	{
		Logger::logging(L"DanmuEchoPlugin::~DanmuEchoPlugin begin.");
		Logger::logging(L"DanmuEchoPlugin::~DanmuEchoPlugin end.");
	}
public:
	//////////////////////////////////////////////////////////////////////////
	virtual wchar_t* GetName() override
	{
		Logger::logging(L"DanmuEchoPlugin::GetName begin.");
		UTILITY_SCOPE_EXIT([]() {Logger::logging(L"DanmuEchoPlugin::GetName end."); });
		return L"DanmuEchoPlugin";
	}
	virtual IPluginElement* CreateElement() override
	{
		Logger::logging(L"DanmuEchoPlugin::CreateElement begin.");
		UTILITY_SCOPE_EXIT([]() {Logger::logging(L"DanmuEchoPlugin::CreateElement end."); });
		assert(handler);
		assert(!pluginElement);
		if (pluginElement)
		{
			delete pluginElement;
		}
		pluginElement = new PluginContext(handler);
		return pluginElement;
	}
	//��ȡ���������
	virtual wchar_t* GetDescription() 
	{
		Logger::logging(L"DanmuEchoPlugin::GetDescription begin.");
		UTILITY_SCOPE_EXIT([]() {Logger::logging(L"DanmuEchoPlugin::GetDescription end."); });
		return L"danmu echo plugin for douyu tv! author : �Ž�Ҷ��";
	}
	//��ȡ�����ĵ���URL����
	virtual wchar_t* GetHelpUrl() 
	{
		Logger::logging(L"DanmuEchoPlugin::GetHelpUrl begin.");
		UTILITY_SCOPE_EXIT([]() {Logger::logging(L"DanmuEchoPlugin::GetHelpUrl end."); });
		return NULL; 
	}
private:
	IPluginContext* handler = nullptr;
	IPluginElement* pluginElement = nullptr;
};

//���ز��
DYPLUGIN_LOAD IPlugin* dyLoadPlugin(IPluginContext* context)
{
	Logger::logging(L"::dyLoadPlugin begin.");
	UTILITY_SCOPE_EXIT([]() {Logger::logging(L"::dyLoadPlugin end."); });
	return new DanmuEchoPlugin(context);
}

//ж�ز��
DYPLUGIN_LOAD void dyUnloadPlugin(IPlugin* plugin)
{
	Logger::logging(L"::dyUnloadPlugin begin.");
	UTILITY_SCOPE_EXIT([]() {Logger::logging(L"::dyUnloadPlugin end."); });
	delete (DanmuEchoPlugin*)plugin;
}


