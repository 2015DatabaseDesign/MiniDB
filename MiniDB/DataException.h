#pragma once
#include <exception>
#include <string>
using namespace std;
class DataNotComparableException : public exception {
private:
	string datatype1;
	string datatype2;

public:
	DataNotComparableException(string _datatype1, string _datatype2)
		:datatype1(_datatype1), datatype2(_datatype2) {}
	const char* what() const throw()
	{
		string str = datatype1 + " and " + datatype2;
		string output = "Data is not comparable :" + str;
		char *message = new char[50];
		strcpy(message, output.data());
		return message;
	}
};

class InvalidInputForConstructData : public exception {
public:
	const char* what() const throw()
	{
		return "Can not convert input to data as int, float or string";
	}
};

class TableException : public exception {
private:
	string msg;
public:
	TableException(string _msg) :msg(_msg) {};
	virtual ~TableException() {};
	//string getMsg() { return msg; }
	const char *what() const
	{
		char* message = new char[50];
		strcpy(message, msg.data());
		return message;
	}
};