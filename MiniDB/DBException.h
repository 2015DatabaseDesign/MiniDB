#pragma once
#include <exception>
#include <string>
//#include <sstream>

using namespace std;
class MethodNotImplementException : public exception {
public:
	const char* what() const throw()
	{
		return "Method is not implemented";
	}
};