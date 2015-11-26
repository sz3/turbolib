#pragma once

#include "str.h"
#include <string>
#include <unordered_map>

// http://efesx.com/2010/08/31/overloading-macros/
// http://ptspts.blogspot.com/2013/11/how-to-apply-macro-to-all-arguments-of.html
#define APPLY0(t, dummy)
#define APPLY1(t, a) t(a)
#define APPLY2(t, a, b) t(a) t(b)
#define APPLY3(t, a, b, c) t(a) t(b) t(c)
#define APPLY4(t, a, b, c, d) t(a) t(b) t(c) t(d)
#define APPLY5(t, a, b, c, d, e) t(a) t(b) t(c) t(d) t(e)
#define APPLY6(t, a, b, c, d, e, f) t(a) t(b) t(c) t(d) t(e) t(f)
#define APPLY7(t, a, b, c, d, e, f, g) t(a) t(b) t(c) t(d) t(e) t(f) t(g)

#define NUM_ARGS_H1(dummy, x7, x6, x5, x4, x3, x2, x1, x0, ...) x0
#define NUM_ARGS(...) NUM_ARGS_H1(dummy, ##__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0)
#define APPLY_ALL_H3(t, n, ...) APPLY##n(t, __VA_ARGS__)
#define APPLY_ALL_H2(t, n, ...) APPLY_ALL_H3(t, n, __VA_ARGS__)
#define APPLY_ALL(t, ...) APPLY_ALL_H2(t, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define LOAD_MAP_INTERNAL(...) APPLY_ALL(LOAD_MAP_ELEM, __VA_ARGS__)
#define LOAD_MAP_ELEM(n) {auto it = map.find(#n); if (it != map.end()) turbo::str::fromStr(n, it->second);}

#define SAVE_MAP_INTERNAL(...) APPLY_ALL(SAVE_MAP_ELEM, __VA_ARGS__)
#define SAVE_MAP_ELEM(n) {map[#n] = turbo::str::str(n);}

#define DEFINE_SERIALIZE_MAP(...) \
void load(const std::unordered_map<std::string,std::string>& map) \
{ \
	LOAD_MAP_INTERNAL(__VA_ARGS__) \
} \
void save(std::unordered_map<std::string,std::string>& map) \
{ \
	SAVE_MAP_INTERNAL(__VA_ARGS__) \
}
