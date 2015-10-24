#include <stdlib.h>
#include "mystr.h"
using namespace std;

int StrToI(const string& s) {
	int m = 0;
	for (int i = 0; i < s.size(); i++) {
		m *= 10;
		m += s[i] - '0';
	}
	return m;
}

const string IToStr(int n, int length) {
	char *s = (char *)malloc(sizeof(char) * (length + 1));
	s[length] = '\0';
	for (int i = length - 1; i >= 0; i--) {
		s[i] = n % 10;
		n /= 10;
	}
	string ss(s);
	free(s);
	return ss;
}

int FindFirstNon0(const string& s) {
	int i;
	for (i = 0; i < s.size(); i++)
		if (s[i] != '0')
			break;
	return i;
}

int IntCmp(const string& s, const string& t) {
	int i = FindFirstNon0(s);
	int j = FindFirstNon0(t);
	int n = atoi(s.substr(i, s.size() - i).c_str());
	int m = atoi(t.substr(j, t.size() - j).c_str());
	return n - m;
}

int FloatCmp(const string& s, const string& t) {
	int i = FindFirstNon0(s);
	int j = FindFirstNon0(t);
	float n = atof(s.substr(i, s.size() - i).c_str());
	float m = atof(t.substr(j, t.size() - j).c_str());
	if (n < m)
		return -1;
	else if (n > m)
		return 1;
	else
		return 0;
}

int StrCmp(const string& s, const string& t) {
	int i = FindFirstNon0(s);
	int j = FindFirstNon0(t);
	string n = s.substr(i, s.size() - i).c_str();
	string m = t.substr(j, t.size() - j).c_str();
	if (n < m)
		return -1;
	else if (s > t)
		return 1;
	else
		return 0;
}

const std::string SpanStrToLen(std::string s, int length) {
	while (s.size() < length)
		s = "0" + s;
	return s;
}




