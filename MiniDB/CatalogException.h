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
using namespace std;
class CatalogException {
private:
	string msg;
public:
	CatalogException(string _msg) :msg(_msg) {};
	virtual ~CatalogException() {};
	string getMsg() { return msg; }
};

#endif /* RECORDMANAGER_RECORDEXCEPTION_H_ */