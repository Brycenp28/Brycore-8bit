#include <iostream>
#include <fstream>
#include "Brycore.h"

using namespace std;

int main(int argc, const char *args[]) {
	char temp[255];

	ifstream infile(args[1]);

	if (!infile.is_open()) {
		cout << "file not found!" << endl;
		return 0;
	};
	
	infile.seekg(0, std::ios::end);
	size_t length = infile.tellg();
	infile.seekg(0, std::ios::beg);

	if (length > sizeof (temp))
	{
    	length = sizeof (temp);
	};


	infile.read(temp, length);

	Brycore b(temp);
	b.execute();
	
	return 0;
}