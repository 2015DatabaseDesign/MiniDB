/*
* RecordException.h
*
*  Created on: 2014��11��1��
*      Author: darren
*/

#ifndef CATALOGMANAGER_CATALOGEXCEPTION_H_
#define CATALOGMANAGER_CATALOGEXCEPTION_H_
#include <string>
#include <cstring>
#include <exception>
using namespace std;
class CatalogException : public exception {
private:
	string msg;
public:
	CatalogException(string _msg) :msg(_msg) {};
	virtual ~CatalogException() {};
	//string getMsg() { return msg; }
	const char *what() const
	{
		return msg.data();
	}
};

#endif /* RECORDMANAGER_RECORDEXCEPTION_H_ */