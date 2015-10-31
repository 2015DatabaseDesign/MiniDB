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
		///*stringstream ss;
		//ss << t;
		//string tupleStr;
		//ss >> tupleStr;
		//*/
		//stringstream ss;
		//ss << index;
		//string indexStr;
		//ss >> indexStr;
		////string output = "Tuple: " + tupleStr + ", index:" + indexStr;
		//string output = "index : " + indexStr + "is out of range of page";
		//return output.data();
		char *output = new char[30];
		sprintf(output, "Index: %d is out of range of number of tuple in the page", index);
		return output;
	}
};

class TupleFormatInconsistentWithTable : public exception {
private:
	//Tuple t;
	string tuple;
	//const Table* table;
	//string table;
public:
	TupleFormatInconsistentWithTable(string tuple)
	{
		this->tuple = tuple;
		//this->table = table;
	}
	const char *what() const throw()
	{
		/*string output = "Tuple's format is inconsistent with table";
		stringstream ss;
		ss << t;
		string tupleStr;
		ss >> tupleStr;
		cout << "Tuple: " << t << endl;
	*/
		string output ="Tuple format is inconsistent with table\n Tuple: " + tuple;
		return output.data();
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
		char* buffer = new char[10];
		sprintf(buffer, "%d", index);
		string indexstr(buffer, strlen(buffer) + 1);
		string output = "This tuple is already deleted but deletion is applied on it again.\nIndex :" + indexstr;
		delete buffer;
		return output.data();
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
		char* buffer = new char[10];
		sprintf(buffer, "%d", index);
		string indexstr(buffer, strlen(buffer) + 1);
		string output = "This tuple is already written but insertion is applied on it.\nIndex :" + indexstr;
		delete buffer;
		return output.data();
	}
};