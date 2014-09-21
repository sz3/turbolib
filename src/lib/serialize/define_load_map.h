#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

// http://efesx.com/2010/08/31/overloading-macros/
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,N,...) N

#define macro_dispatcher(func, ...) \
			macro_dispatcher_(func, VA_NUM_ARGS(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) \
			macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) \
			func ## nargs

#define DEFINE_LOAD_MAP(...) macro_dispatcher(DEFINE_LOAD_MAP, __VA_ARGS__)(__VA_ARGS__)

#define DEFINE_LOAD_MAP1(a) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
}

#define DEFINE_LOAD_MAP2(a,b) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
}

#define DEFINE_LOAD_MAP3(a,b,c) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
	{ auto it = map.find(#c); if (it != map.end()) fromString(c, it->second); } \
}

#define DEFINE_LOAD_MAP4(a,b,c,d) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
	{ auto it = map.find(#c); if (it != map.end()) fromString(c, it->second); } \
	{ auto it = map.find(#d); if (it != map.end()) fromString(d, it->second); } \
}

#define DEFINE_LOAD_MAP5(a,b,c,d,e) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
	{ auto it = map.find(#c); if (it != map.end()) fromString(c, it->second); } \
	{ auto it = map.find(#d); if (it != map.end()) fromString(d, it->second); } \
	{ auto it = map.find(#e); if (it != map.end()) fromString(e, it->second); } \
}

#define DEFINE_LOAD_MAP6(a,b,c,d,e,f) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
	{ auto it = map.find(#c); if (it != map.end()) fromString(c, it->second); } \
	{ auto it = map.find(#d); if (it != map.end()) fromString(d, it->second); } \
	{ auto it = map.find(#e); if (it != map.end()) fromString(e, it->second); } \
	{ auto it = map.find(#f); if (it != map.end()) fromString(f, it->second); } \
}

#define DEFINE_LOAD_MAP7(a,b,c,d,e,f,g) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
	{ auto it = map.find(#c); if (it != map.end()) fromString(c, it->second); } \
	{ auto it = map.find(#d); if (it != map.end()) fromString(d, it->second); } \
	{ auto it = map.find(#e); if (it != map.end()) fromString(e, it->second); } \
	{ auto it = map.find(#f); if (it != map.end()) fromString(f, it->second); } \
	{ auto it = map.find(#g); if (it != map.end()) fromString(g, it->second); } \
}

#define DEFINE_LOAD_MAP8(a,b,c,d,e,f,g,h) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
	{ auto it = map.find(#c); if (it != map.end()) fromString(c, it->second); } \
	{ auto it = map.find(#d); if (it != map.end()) fromString(d, it->second); } \
	{ auto it = map.find(#e); if (it != map.end()) fromString(e, it->second); } \
	{ auto it = map.find(#f); if (it != map.end()) fromString(f, it->second); } \
	{ auto it = map.find(#g); if (it != map.end()) fromString(g, it->second); } \
	{ auto it = map.find(#h); if (it != map.end()) fromString(h, it->second); } \
}

#define DEFINE_LOAD_MAP9(a,b,c,d,e,f,g,h,i) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
	{ auto it = map.find(#c); if (it != map.end()) fromString(c, it->second); } \
	{ auto it = map.find(#d); if (it != map.end()) fromString(d, it->second); } \
	{ auto it = map.find(#e); if (it != map.end()) fromString(e, it->second); } \
	{ auto it = map.find(#f); if (it != map.end()) fromString(f, it->second); } \
	{ auto it = map.find(#g); if (it != map.end()) fromString(g, it->second); } \
	{ auto it = map.find(#h); if (it != map.end()) fromString(h, it->second); } \
	{ auto it = map.find(#i); if (it != map.end()) fromString(i, it->second); } \
}

#define DEFINE_LOAD_MAP10(a,b,c,d,e,f,g,h,i,j) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	{ auto it = map.find(#a); if (it != map.end()) fromString(a, it->second); } \
	{ auto it = map.find(#b); if (it != map.end()) fromString(b, it->second); } \
	{ auto it = map.find(#c); if (it != map.end()) fromString(c, it->second); } \
	{ auto it = map.find(#d); if (it != map.end()) fromString(d, it->second); } \
	{ auto it = map.find(#e); if (it != map.end()) fromString(e, it->second); } \
	{ auto it = map.find(#f); if (it != map.end()) fromString(f, it->second); } \
	{ auto it = map.find(#g); if (it != map.end()) fromString(g, it->second); } \
	{ auto it = map.find(#h); if (it != map.end()) fromString(h, it->second); } \
	{ auto it = map.find(#i); if (it != map.end()) fromString(i, it->second); } \
	{ auto it = map.find(#j); if (it != map.end()) fromString(j, it->second); } \
}

template <typename T>
bool fromString(T& var, const std::string& str)
{
	if (str.empty())
		return false;
	std::stringstream ss(str);
	ss >> var;
	return !!ss;
}
