#ifndef __MYSTR_H_
#define __MYSTR_H_
#include <string>

int StrToI(const std::string& s);
const std::string IToStr(int n, int length);
int IntCmp(const std::string& s, const std::string& t);
int FloatCmp(const std::string& s, const std::string& t);
int StrCmp(const std::string& s, const std::string& t);
const std::string SpanStrToLen(std::string s, int length);

#endif

