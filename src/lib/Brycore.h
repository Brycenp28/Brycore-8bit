#ifndef BRYCORE_H_
#define BRYCORE_H_

class Brycore {

public:
	struct flags;
	Brycore(char program[]);
	bool render_carry(int test, flags *flag);
	bool render_zero(int test, flags *flag);
	void set_register(char reg, unsigned char value);
	unsigned char get_register(char reg);
	void MOV(); // needs testing.
	void JMP();
	void PUSH();
	void POP();
	void ADD();
	void SUB();
	void MUL();
	void DIV();
	void INC();
	void DEC();
	void CMP();

	void SHL(); // todo
	void SHR();
	void AND();
	void OR();
	void XOR();
	void NOT();
	void CALL();
	void RET();
	void IRQ();


	void execute();
private:
	flags *flag;
	unsigned char r[6] = {'\x00'};
	char *IP, *begining;
};

#endif