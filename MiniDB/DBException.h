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

class NoSuchAttributeInTable :public exception {
private:
	string AttrName;
	string TableName;
public:
	NoSuchAttributeInTable(string AttrName, string TableName)
	{
		this->AttrName = AttrName;
		this->TableName = TableName;
	}
	const char* what() const throw()
	{
		string output = "There is no attribute named " + AttrName + "in table " + TableName;
		return output.data();
	}
};