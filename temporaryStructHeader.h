struct label{
		char name[31];
		char type[10];
		int lineNum;
		struct label *next;
};

struct labelOperand{
	char name[31];
	int base;
	int offset;
	struct labelOperand *next;
};

struct entry{
	char name[31];
	struct entry *next;
};

struct external{
	char name[31];
	struct external *next;
};

struct potentialLabel{
	char name[31];
	struct potentialLabel *next;
};

struct commands{
	char *name;
	int opCode;
	int funct;
};

struct word{
	unsigned int bit19:1;
	unsigned int A:1;
	unsigned int R:1;
	unsigned int E:1;
	unsigned int values:16;
};

/*struct word2{
	unsigned int bit19:1;
	unsigned int A:1;
	unsigned int R:1;
	unsigned int E:1;
	unsigned int funct:4;
	unsigned int sourceRegister:4;
	unsigned int sourceAddressingMode:2;
	unsigned int destinationRegister:4;
	unsigned int destinationAddressingMode:2;
};

struct word3{
	unsigned int bit19:1;
	unsigned int A:1;
	unsigned int R:1;
	unsigned int E:1;
	unsigned int value:16;
};*/

