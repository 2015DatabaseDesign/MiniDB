#pragma once
#include <exception>
#include <string>
using namespace std;
class MethodNotImplementException : public exception {
public:
	const char* what() const throw()
	{
		return "Method is not implemented";
	}
};

class DataNotComparableException : public exception {
private:
	string datatype1;
	string datatype2;

public:
	DataNotComparableException(string _datatype1, string _datatype2)
		:datatype1(_datatype1), datatype2(_datatype2){}
	const char* what() const throw()
	{
		string str = datatype1 + "and" + datatype2;
		char message[] = "Data is not comparable :";
		strcat(message, str.data());
		return message;
	}
};