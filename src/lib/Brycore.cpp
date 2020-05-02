#include <iostream>
#include <ncurses.h>
#include "Brycore.h"

struct Brycore::flags {
	bool CF, ZF;
};

Brycore::Brycore(char program[]) {
	IP = program;
	begining = program;

	flag = new flags;
	flag->CF = false;
	flag->ZF = false;
	initscr();
	raw();
}

Brycore::~Brycore() {
	delete flag;
	flag = NULL;
	endwin();
}

bool Brycore::render_carry(unsigned int test, flags *flag) {

	if((test > 255) || (test < 0)) {
		flag->CF = true;
		return true;
	}else {
		flag->CF = false;
	}

	return false;
}

bool Brycore::render_zero(unsigned int test, flags *flag) {

	if(!test) {
			flag->ZF = true;
	} else {
			flag->ZF = false;
	}

	return false;
}

bool Brycore::render_zero(unsigned char test, flags *flag) {

	if(!test) {
			flag->ZF = true;
	} else {
			flag->ZF = false;
	}

	return false;
}

void Brycore::MOV() {

	unsigned char s=0, c=0, r=0, args[2];

	args[0] = *(IP+1);
	args[1] = *(IP+2);

	switch(*IP) { // starts at opcode: 1.
		case 1: // reg, const.
			this->r[args[0]] = args[1];
		break;
		case 2: // reg, [reg]
			this->r[args[0]] = *(begining+this->r[args[1]]);
		break;
		case 3: // reg, reg.
			this->r[args[0]] = this->r[args[1]];
		break;
		case 4: // [reg], const
			
			*(begining+this->r[args[0]]) = args[1];
		break;
		case 5: // reg, [const]
			this->r[args[0]] = *(begining+args[1]);
		break;
		case 6: // [reg], reg.
			*(begining+this->r[args[0]]) = this->r[args[1]];
		break;
		case 7: // [reg+/-offset], const
			s = (args[0] & 128) >> 7;
			c = (args[0] & 120) >> 3;
			r = args[0] & 7;

			if (s != 1) {
				*(begining+this->r[r]+c) = args[1];
			}else {
				*(begining+this->r[r]-c) = args[1];
			}
		break;
		case 8: // reg, [reg+/-offset].
			s = (args[1] & 128) >> 7;
			c = (args[1] & 120) >> 3;
			r = args[1] & 7;
			if (s != 1) {
				this->r[args[0]] = *(begining+(this->r[r]+c));
			}else {
				this->r[args[0]] = *(begining+(this->r[r]-c));
			}
		break;
		case 9: // [reg+/-offset], reg.
			s = (args[0] & 128) >> 7;
			c = (args[0] & 120) >> 3;
			r = args[0] & 7;
			if (s != 1) {
				 *(begining+this->r[r]+c) = this->r[args[1]];
			}else {
				 *(begining+this->r[r]-c) = this->r[args[1]];
			}
		break;
		case 10: // [const], reg
			*(begining+args[0]) = this->r[args[1]];
		break;
		case 11: // [const], const
			*(begining+args[0]) = args[1];
		break;
	};
	IP += 3;
}

void Brycore::JMP() {

	unsigned char data, mode;
	bool is_reg = uint8c(*IP) <= 18;
	bool jumped;

	if (is_reg) { // reg or const.
		data = r[*(IP+1)];
		mode = uint8c(*IP)-11;
	}else {
		data = uint8c(*(IP+1));
		mode = uint8c(*IP)-18;
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
	if (*IP == 26) { // reg / const
		--r[5];
		*(begining+r[5]) = r[*(IP+1)];
	} else {
		--r[5];
		*(begining+r[5]) = *(IP+1); // I-23
	};
	IP += 2;
}

void Brycore::POP() {
	 r[*(IP+1)] = *(begining+r[5]);
	 ++r[5];
	 IP += 2;
}

void Brycore::ADD() {

	int test;

	if(*IP == 29) { // ADD reg, reg/const.
		test = r[*(IP+1)]+r[*(IP+2)];
		r[*(IP+1)] += r[*(IP+2)];
	}else {
		test = r[*(IP+1)]+*(IP+2);
		r[*(IP+1)] += uint8c(*(IP+2));
	};
	render_carry(test, flag);
	IP += 3;
}

void Brycore::SUB() {
	
	unsigned int test;
	
	if(*IP == 31) { // SUB reg, reg/const
		test = r[*(IP+1)]-r[*(IP+2)];
		r[*(IP+1)] -= r[*(IP+2)];
	}else {
		test = r[*(IP+1)] - uint8c(*(IP+2));
		r[*(IP+1)] -= uint8c(*(IP+2));
	};
	render_carry(test, flag);
	render_zero(test, flag);

	IP += 3;
}

void Brycore::MUL() {

	if (*IP == 33) {
	r[0] *= uint8c(*(IP+1));
	} else {
		r[0] *= r[*(IP+1)];
	};
	IP+=2;
}
	
void Brycore::DIV() {

	if (*IP == 35) {
	r[0] /= uint8c(*(IP+1));
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
	unsigned char s,c,r;
	unsigned int test;

	s = (*(IP+2) & 128) >> 7;
	c = (*(IP+2) & 120) >> 3;
	r = *(IP+2) & 7;

	if(*IP == 39) { //reg, reg/[const]/[const+/-n]/const.
		test = this->r[*(IP+1)]-this->r[*(IP+2)];
	}else if (*IP == 40) {
		test = this->r[*(IP+1)]-*(begining+uint8c(*(IP+2)));
	}else if (*IP == 41) {
		if (s != 1) {
			 test = this->r[*(IP+1)] - *(begining+this->r[r]+c);
		}else {
			 test = this->r[*(IP+1)] - *(begining+this->r[r]-c);
		};
	}else if (*IP == 42) {
		test = this->r[*(IP+1)] - uint8c(*(IP+2));
	}else {
		test = this->r[*(IP+1)] - *(begining+this->r[*(IP+2)]);
	};

	render_carry(test, flag);
	render_zero(test, flag);
	IP+=3;
}

void Brycore::SHL() {

	if (*IP == 44) {
	r[0] = (r[0] << uint8c(*(IP+1)));
	} else {
		r[0] = (r[0] << r[*(IP+1)]);
	};
	render_zero(r[*(IP+1)], flag);
	IP+=2;
}

void Brycore::SHR() {

	if (*IP == 46) {
	r[0] = (r[0] >> uint8c(*(IP+1)));
	} else {
		r[0] = (r[0] >> r[*(IP+1)]);
	};
	render_zero(r[*(IP+1)], flag);
	IP+=2;
}

void Brycore::AND() {

	if (*IP == 48) {
		r[*(IP+1)] = (r[*(IP+1)] & *(IP+2));
	}else {
		r[*(IP+1)] = (r[*(IP+1)] & r[*(IP+2)]);
	};
	render_zero(r[*(IP+1)], flag);
	IP+=3;
}

void Brycore::OR() {

	if (*IP == 50) {
		r[*(IP+1)] = (r[*(IP+1)] | *(IP+2));
	}else {
		r[*(IP+1)] = (r[*(IP+1)] | r[*(IP+2)]);
	};
	render_zero(r[*(IP+1)], flag);
	IP+=3;
}

void Brycore::XOR() {

	if (*IP == 52) {
		r[*(IP+1)] = (r[*(IP+1)] ^ *(IP+2));
	}else {
		r[*(IP+1)] = (r[*(IP+1)] ^ r[*(IP+2)]);
	};
	render_zero(r[*(IP+1)], flag);
	IP+=3;
}

void Brycore::NOT() {

	r[*(IP+1)] = ~r[*(IP+1)];
	render_zero(r[*(IP+1)], flag);
	IP+=2;
}

void Brycore::CALL() {

	if (*IP == 55) {
		--r[5];
		*(begining+r[5]) = (IP - begining)+2;
		IP = begining+r[*(IP+1)];
	}else {
		--r[5];
		*(begining+r[5]) = (IP - begining)+2;
		IP = begining+uint8c(*(IP+1));
	};
}

void Brycore::RET() {
	 unsigned char ret_addr = *(begining+r[5]);
	 IP = (begining+ret_addr);
	 ++r[5];
}

void Brycore::LEA() {
	unsigned char s,c,r;

	s = (*(IP+2) & 128) >> 7;
	c = (*(IP+2) & 120) >> 3;
	r = *(IP+2) & 7;

	if (s != 1) {
		 this->r[*(IP+1)] = this->r[r]+c;
	}else {
		 this->r[*(IP+1)] = this->r[r]-c;
	};
	IP += 3;
}

void Brycore::IRQ() {
	unsigned char call = *(IP+1);
	if (call == 0) { // get char.
		unsigned char character = getch();

		if (character == 10) {
			int x, y;
			getyx(stdscr,y, x);
			move(y+1,x);
		};

		*(begining+this->r[0]) = character;
	}
	else if (call == 1) {
		addch(*(begining+this->r[0]));
		refresh();
	};

	IP+=2;
}


void Brycore::execute() {
	while(*IP) {

		if (*IP >= 1 && *IP <= 11) {
			MOV();
		}
		else if (*IP >= 12 && *IP <= 25) {
			JMP();
		}
		else if (*IP >= 26 && *IP <= 27) {
			PUSH();
		}
		else if (*IP == 28) {
			POP();
		}
		else if (*IP >= 29 && *IP <= 30) {
			ADD();
		}else if (*IP >= 31 && *IP <= 32) {
			SUB();
		}else if (*IP >= 33 && *IP <= 34) {
			MUL();
		}else if (*IP >= 35 && *IP <= 36) {
			DIV();
		}else if (*IP == 37) {
			INC();
		}else if (*IP == 38) {
			DEC();
		}else if (*IP >= 39 && *IP <= 43) {
			CMP();
		}else if (*IP >= 44 && *IP <= 45) {
			SHL();
		}
		else if (*IP >= 46 && *IP <= 47) {
			SHR();
		}
		else if (*IP >= 48 && *IP <= 49) {
			AND();
		}
		else if (*IP >= 50 && *IP <= 51) {
			OR();
		}
		else if (*IP >= 52 && *IP <= 53) {
			XOR();
		}
		else if (*IP == 54) {
			NOT();
		}
		else if (*IP >= 55 && *IP <= 56) {
			CALL();
		}
		else if (*IP == 57) {
			RET();
		}
		else if (*IP == 58) {
			LEA();
		}else if (*IP == 59) {
			IRQ();
		}
	}
	printw("\n[system] Program halted.");
	refresh();
	getch();
}