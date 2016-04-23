//lua
#include "selene.h"
//std headers
#include <string>
#include <cassert>
#include <codecvt>

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
class LuaContext
{
public:
	static int add(const int a,const int b)
	{
		return LuaContext::instance().inner_add(a,b);
	}
	static std::wstring add(const std::wstring& a, const std::wstring& b)
	{
		//convert input from utf16 to utf8		
		const auto utf8_output = LuaContext::instance().inner_add(utf16_to_utf8(a), utf16_to_utf8(b));
		//convert output from utf8 to utf16
		return utf8_to_utf16(utf8_output);
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
	int inner_add(const int a, const int b)
	{
		return state["add"](a, b);
	}
	std::string inner_add(const std::string& a, const std::string& b)
	{
		return state["add_str"](a, b);
	}
private:
	bool load_lua_file()
	{
		return state.Load(lua_file_path);
	}
	void close_lua()
	{
		
	}
private:
	const std::string lua_file_path = R"(example.lua)";
	sel::State state;
};

int main(int, char*[])
{
	{
		const auto result = LuaContext::add(3, 4);
		assert(result == 7);
	}
	{
		const std::wstring a = L"Äã";
		const std::wstring b = L"ºÃ";
		const auto result = LuaContext::add(a, b);
		const std::wstring standard = L"Äã ºÃ";
		assert(result == standard);
	}
	std::cout << "test ok!" << std::endl;
	return 0;
}