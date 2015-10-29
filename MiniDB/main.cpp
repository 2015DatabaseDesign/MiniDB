#include "MiniDB.h"

int main(int argc, char *argv[])
{
	MiniDB db;
	while (true) {
		try {
			db.readinput();
		}
		catch (IllegalCommand e) {
			e.DBerror();
		}
	}
}