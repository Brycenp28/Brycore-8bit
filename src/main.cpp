#include <iostream>
#include <fstream>
#include "lib/Brycore.h"

int main(int argc, const char *args[]) {

	char temp[256];

	std::ifstream infile(args[1], std::ios_base::binary);

	if (!infile.is_open()) {
		std::cout << "file not found!" << std::endl;
		return 0;
	};

	infile.read(temp, sizeof(temp));

	Brycore b(temp);
	b.execute();
	infile.close();
	return 0;
}