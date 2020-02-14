#include <iostream>
#include "Brycore.h"

using namespace std;


struct Brycore::flags {
	bool CF, ZF;
};

void Brycore::set_register(char reg, unsigned char value) {
	Brycore::r[reg] = value;
}

unsigned char Brycore::get_register(char reg) {
	return Brycore::r[reg];
}

Brycore::Brycore(char program[]) {
	IP = program;
	begining = program;
	flag->CF = false;
	flag->ZF = false;
}

bool Brycore::render_carry(int test, flags *flag) {

	if((test > 255) || (test < 0)) {
		flag->CF = true;
		return true;
	}else {
		flag->CF = false;
	}

	return false;
}

bool Brycore::render_zero(int test, flags *flag) {

	if(!test) {
			flag->ZF = true;
	} else {
			flag->ZF = false;
	}

	return false;
}

void Brycore::MOV() {

	char s,c,r,temp;
	unsigned char args[2];
	
	args[0] = *(IP+1);
	args[1] = *(IP+2);

	switch(*IP) { // starts at opcode: 1.
		case 1: // reg, const.
			set_register(args[0],args[1]);
		break; // reg, reg.
		case 2:
			set_register(args[0],get_register(args[1]));
		break; // [reg], const
		case 3:
			
			*(begining+get_register(args[0])) = args[1];
		break;
		case 4: // reg, [const]
			set_register(args[0],*(begining+args[1]));
		break;
		case 5: // [reg], const.
			*(begining+get_register(args[0])) = args[1];
		break;
		case 6: // [reg], reg.
			*(begining+get_register(args[0])) = get_register(args[1]);
		break;
		case 7: // [reg+/-offset], const
			s = args[0]>>7;
			c = (args[0]<<1)>>4;
			r = (args[0]<<5)>>5;

			if (s != 1) {
				*(begining+get_register(r)+c) = args[1];
			}else {
				*(begining+get_register(r)-c) = args[1];
			}
		break;
		case 8: // reg, [reg+/-offset].
			s = args[1]>>7;
			c = (args[1]<<1)>>4;
			r = (args[1]<<5)>>5;
			if (s != 1) {
				set_register(args[0],*(begining+get_register(r)+c));
			}else {
				set_register(args[0],*(begining+get_register(r)-c));
			}
		break;
		case 9: // [reg+/-offset], reg.
			s = args[0]>>7;
			c = (args[0]<<1)>>4;
			r = (args[0]<<5)>>5;
			if (s != 1) {
				 *(begining+get_register(r)+c) = args[1];
			}else {
				 *(begining+get_register(r)-c) = args[1];
			}
		break;
	};
	IP += 3;
}

void Brycore::JMP() {

	char data, mode;
	bool is_reg = *IP <= 16;
	bool jumped;

	if (is_reg) { // reg or const.
		data = get_register(*(IP+1));
		mode = *IP-9;
	}else {
		data = *(IP+1);
		mode = *IP-16;
	}

	switch(mode) {
		case 1: // JMP
			IP = begining+data;
			jumped = true;
		break;
		case 2: // JC <
			if (flag->CF) {IP = begining+data;jumped = true;};
		break;
		case 3: // JZ ==
			if (flag->ZF) {IP = begining+data;jumped = true;};
		break;
		case 4: // JBE <=
			if (flag->CF || flag->ZF) {IP = begining+data;jumped = true;};
		break;
		case 5: // JNC >=
			if (!flag->CF) {IP = begining+data;jumped = true;};
		break;
		case 6: // JNZ !=
			if (!flag->ZF) {IP = begining+data;jumped = true;};
		break;
		case 7: // JNBE >
			if ((flag->CF == false) && (flag->ZF == false)) {IP = begining+data;jumped = true;};
		break;
	};

	if(!jumped) {
		IP += 2;
	};
}

void Brycore::PUSH() {
	if (*IP == 22) { // reg / const
	 *(begining+r[5]) = r[*(IP+1)];
	 r[5]--;
	} else {
	*(begining+r[5]) = *(IP+1); // I-23
	r[5]--;
	};
	IP += 2;
}

void Brycore::POP() {
	 r[5]++;
	 r[*(IP+1)] = *(begining+r[5]);
	 *(begining+r[5]) = 0;
	 IP += 2;
}

void Brycore::ADD() {

	int test;

	if(*IP == 25) { // ADD reg, reg/const.
		test = r[*(IP+1)]+r[*(IP+2)];
		r[*(IP+1)] += r[*(IP+2)];
	}else {
		test = r[*(IP+1)]+*(IP+2);
		r[*(IP+1)] += *(IP+2);
	};
	render_carry(test, flag);
	IP += 3;
}

void Brycore::SUB() {
	
	int test;
	
	if(*IP == 27) { // SUB reg, const/reg
		test = r[*(IP+1)]-r[*(IP+2)];
		r[*(IP+1)] -= r[*(IP+2)];
	}else {
		test = r[*(IP+1)] - *(IP+2);
		r[*(IP+1)] -= *(IP+2);
	};
	render_carry(test, flag);
	render_zero(test, flag);

	IP += 3;
}

void Brycore::MUL() {

	if (*IP == 29) {
	r[0] *= *(IP+1);
	} else {
		r[0] *= r[*(IP+1)];
	};
	IP+=2;
}
	
void Brycore::DIV() {

	if (*IP == 31) {
	r[0] /= *(IP+1);
	} else {
		r[0] /= r[*(IP+1)];
	};
	IP+=2;
}

void Brycore::INC() {
	++r[*(IP+1)];
	IP+=2;
}

void Brycore::DEC() {
	--r[*(IP+1)];
	IP+=2;
}

void Brycore::CMP() {
	char s,c,r;
	int test;
	s = *(IP+1)>>7;
	c = (*(IP+1)<<1)>>4;
	r = (*(IP+1)<<5)>>5;

	if(*IP == 35) { //reg, reg/[const]/[const+/-n]/const.
		test = get_register(*(IP+1))-get_register(*(IP+2));
	}else if (*IP == 36) {
		test = get_register(*(IP+1))-*(begining+*IP+2);
	}else if (*IP == 37) {
		if (s != 1) {
			 test = get_register(*(IP+1)) - *(begining+get_register(r)+c);
		}else {
			 test = get_register(*(IP+1)) - *(begining+get_register(r)-c);
		};
	}else {
		test = get_register(*(IP+1)) - *(IP+2);
	};

	render_carry(test, flag);
	render_zero(test, flag);
	IP+=3;
}

void Brycore::execute() {
	while(*IP) {
		if (*IP >= 1 && *IP <= 9) {
			MOV();
			cout << "MOV" << endl;
		}
		else if (*IP >= 10 && *IP <= 21) {
			JMP();
			cout << "JMP" << endl;
		}
		else if (*IP >= 22 && *IP <= 23) {
			PUSH();
			cout << "PUSH" << endl;
		}
		else if (*IP == 24) {
			POP();
			cout << "POP" << endl;
		}
		else if (*IP >= 25 && *IP <= 26) {
			ADD();
			cout << "ADD" << endl;
		}else if (*IP >= 27 && *IP <= 28) {
			SUB();
			cout << "SUB" << endl;
		}else if (*IP >= 29 && *IP <= 30) {
			MUL();
			cout << "MUL" << endl;
		}else if (*IP >= 31 && *IP <= 32) {
			DIV();
			cout << "DIV" << endl;
		}else if (*IP == 33) {
			INC();
			cout << "INC" << endl;
		}else if (*IP == 34) {
			DEC();
			cout << "DEC" << endl;
		}else if (*IP >= 35 && *IP <= 38) {
			CMP();
			cout << "CMP" << endl;
		}
		else if (*IP == 97) {
			cout << "A: " << static_cast<unsigned>(r[0]) << endl;
			cout << "B: " << static_cast<unsigned>(r[1]) << endl;
			cout << "C: " << static_cast<unsigned>(r[2]) << endl;
			cout << "D: " << static_cast<unsigned>(r[3]) << endl;
			cout << "BP: " << static_cast<unsigned>(r[4]) << endl;
			cout << "SP: " << static_cast<unsigned>(r[5]) << endl;
			IP++;
		}
		else if (*IP == 98) {
			cout << std::boolalpha;
			cout << flag->CF << ":" << flag->ZF << endl;
			cout << std::boolalpha;
			IP++;
		}
	}
	cout << "[system] Program halted." << endl;
}