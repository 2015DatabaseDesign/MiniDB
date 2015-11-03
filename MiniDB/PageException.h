#pragma once
#include <exception>
//#include <sstream>
class TupleIndexOutofRange : public exception {
private:
	int index;
public:
	TupleIndexOutofRange(int index)
	{
		this->index = index;
	}

	const char* what() const throw()
	{
		char *output = new char[30];
		sprintf(output, "Index: %d is out of range of number of tuple in the page", index);
		return output;
	}
};

class TupleFormatInconsistentWithTable : public exception {
private:
	string tuple;

public:
	TupleFormatInconsistentWithTable(string tuple)
	{
		this->tuple = tuple;
	}
	const char *what() const throw()
	{
		string output ="Tuple format is inconsistent with table\n Tuple: " + tuple;
		char *message = new char[50];
		strcpy(message, output.data());
		return message;
	}

};

class TupleAlreadyDeletedButDeleteItAgain : public exception
{
private:
	int index;
public:
	TupleAlreadyDeletedButDeleteItAgain(int index)
	{
		this->index = index;
	}

	const char* what() const throw()
	{
		char* message = new char[50];
		sprintf(message, "This tuple is already deleted but deletion is applied on it again.\nIndex :%d", index);
		return message;
	}
};

class TupleAlreadyWrittenButInsertOnIt: public exception
{
private:
	int index;
public:
	TupleAlreadyWrittenButInsertOnIt(int index)
	{
		this->index = index;
	}

	const char* what() const throw()
	{
		char* message = new char[50];
		sprintf(message, "This tuple is already written but insertion is applied on it again.\nIndex :%d", index);
		return message;
	}
};