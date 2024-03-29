/** M6502: portable 6502 emulator ****************************/
/**                                                         **/
/**                         M6502.h                         **/
/**                                                         **/
/** This file contains declarations relevant to emulation   **/
/** of 6502 CPU.                                            **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996                      **/
/**               Alex Krasivsky  1996                      **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#ifndef M6502_H
#define M6502_H

                               /* Compilation options:       */
/* #define FAST_RDOP */        /* Separate Op6502()/Rd6502() */
/* #define DEBUG */            /* Compile debugging version  */
/* #define LSB_FIRST */        /* Compile for low-endian CPU */

                               /* Interrupt6502() returns:   */
#define INT_NONE  0            /* No interrupt required      */
#define INT_IRQ	  1            /* Standard IRQ interrupt     */
#define INT_NMI	  2            /* Non-maskable interrupt     */
#define INT_QUIT  3            /* Exit the emulation         */

                               /* 6502 status flags:         */
#define	C_FLAG	  0x01         /* 1: Carry occured           */
#define	Z_FLAG	  0x02         /* 1: Result is zero          */
#define	I_FLAG	  0x04         /* 1: Interrupts disabled     */
#define	D_FLAG	  0x08         /* 1: Decimal mode            */
#define	B_FLAG	  0x10         /* Break [0 on stk after int] */
#define	R_FLAG	  0x20         /* Always 1                   */
#define	V_FLAG	  0x40         /* 1: Overflow occured        */
#define	N_FLAG	  0x80         /* 1: Result is negative      */

/** Simple Datatypes *****************************************/
/** NOTICE: sizeof(byte)=1 and sizeof(word)=2               **/
/*************************************************************/
typedef unsigned char byte;
typedef unsigned short word;
typedef signed char offset;

/** Structured Datatypes *************************************/
/** NOTICE: #define LSB_FIRST for machines where least      **/
/**         signifcant byte goes first.                     **/
/*************************************************************/
typedef union
{
#ifdef LSB_FIRST
  struct { byte l,h; } B;
#else
  struct { byte h,l; } B;
#endif
  word W;
} pair;

typedef struct
{
  byte A,P,X,Y,S;     /* CPU registers and program counter   */
  pair PC;

  int IPeriod,ICount; /* Set IPeriod to number of CPU cycles */
                      /* between calls to Interrupt6502()    */
  void *User;         /* Arbitrary user data (ID,RAM*,etc.)  */
  byte TrapBadOps;    /* Set to 1 to warn of illegal opcodes */
  word Trap;          /* Set Trap to address to trace from   */
  byte Trace;         /* Set Trace=1 to start tracing        */
} M6502;

/** Reset6502() **********************************************/
/** This function can be used to reset the registers before **/
/** starting execution with Run6502(). It sets registers to **/
/** their initial values.                                   **/
/*************************************************************/
void Reset6502(register M6502 *R,int IPeriod);

/** Run6502() ************************************************/
/** This function will run 6502 code until Interrupt6502()  **/
/** call returns INT_QUIT. It will return the PC at which   **/
/** emulation stopped, and current register values in R.    **/
/*************************************************************/
word Run6502(register M6502 *R);

/** Rd6502()/Wr6502/Op6502() *********************************/
/** These functions are called when access to RAM occurs.   **/
/** They allow to control memory access. Op6502 is the same **/
/** as Rd6502, but used to read *opcodes* only, when many   **/
/** checks can be skipped to make it fast. It is only       **/
/** required if there is a #define FAST_RDOP.               **/
/************************************ TO BE WRITTEN BY USER **/
/*void Wr6502(register word Addr,register byte Value);*/
extern void cpu_writemem(register int A,register unsigned char V);
#define Wr6502(A,V) (cpu_writemem(A,V))
/*byte Rd6502(register word Addr);*/
extern int cpu_readmem(register int A);
#define Rd6502(A) ((unsigned)cpu_readmem(A))
/*byte Op6502(register word Addr);*/
extern byte *RAM;
#define FAST_RDOP
#define Op6502(A) (RAM[A])

/** Debug6502() **********************************************/
/** This function should exist if DEBUG is #defined. When   **/
/** Trace!=0, it is called after each command executed by   **/
/** the CPU, and given the 6502 registers. Emulation exits  **/
/** if Debug6502() returns 0.                               **/
/*************************************************************/
byte Debug6502(register M6502 *R);

/** Interrupt6502() ******************************************/
/** 6502 emulation calls this function periodically to      **/
/** check if the system hardware requires any interrupts.   **/
/** This function must return one of following values:      **/
/** INT_NONE, INT_IRQ, INT_NMI. Return INT_QUIT to exit the **/
/** emulation loop.                                         **/
/************************************ TO BE WRITTEN BY USER **/
/*byte Interrupt6502(register M6502 *R);*/
extern int cpu_interrupt(void);
#define Interrupt6502(R) ((byte)cpu_interrupt())

#endif /* M6502_H */
