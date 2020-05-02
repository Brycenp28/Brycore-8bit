#ifndef BRYCORE_H_
#define BRYCORE_H_

class Brycore {

public:

	struct flags;
	Brycore(char program[]);
	~Brycore(void);
	bool render_carry(unsigned int test, flags *flag);
	bool render_zero(unsigned int test, flags *flag);
	bool render_zero(unsigned char test, flags *flag);
	void MOV();
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
	void SHL();
	void SHR();
	void AND();
	void OR();
	void XOR();
	void NOT();
	void CALL();
	void RET();
	void LEA();
	void IRQ();
	

	void execute();

private:

	flags *flag;
	unsigned char r[6] = {};
	char *IP, *begining;

	inline unsigned char uint8c(char a) {
		return static_cast<unsigned char>(a);
	};
};

#endif