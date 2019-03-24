//instruction groups
#define NOP 0
#define JCN 1
//subgroup 1: FIM and SRC
#define FIM_SRC 2
//subgroup 2: FIN and JIN
#define FIN_JIN 3
#define JUN 4
#define JMS 5
#define INC 6
#define ISZ 7
#define ADD 8
#define SUB 9
#define LD 10
#define XCH 11
#define BBL 12
#define LDM 13
//subgroup 3: instructions for reading/writing to memory and status
#define RW_MEM_STATUS 14
//subgroup 4: accumulator instructions group
#define ACC_GROUP 15

//instruction subgroup 1
#define FIM 0
#define SRC 1

//instruction subgroup 2
#define FIN 0
#define JIN 1

//instruction subgroup 3
#define WRM 0
#define WMP 1
#define WRR 2
//note lack of 3
#define WR0 4
#define WR1 5
#define WR2 6
#define WR3 7
#define SBM 8
#define RDM 9
#define RDR 10
#define ADM 11
#define RD0 12
#define RD1 13
#define RD2 14
#define RD3 15

//instruction subgroup 4
#define CLB 0
#define CLC 1
#define IAC 2
#define CMC 3
#define CMA 4
#define RAL 5
#define RAR 6
#define TCC 7
#define DAC 8
#define TCS 9
#define STC 10
#define DAA 11
#define KBP 12
#define DCL 13

#define RAM_SIZE 80

int emulate(int);
void fetch();
unsigned char decode();
void execute(unsigned char);
unsigned char read_RAM();
unsigned char read_RAM_status(unsigned char);
void write_RAM();
void write_RAM_status(unsigned char);