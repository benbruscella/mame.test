/** M6502: portable 6502 emulator ****************************/
/**                                                         **/
/**                          Codes.h                        **/
/**                                                         **/
/** This file contains implementation for the main table of **/
/** 6502 commands. It is included from 6502.c.              **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996                      **/
/**               Alex Krasivsky  1996                      **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

case 0x10: if(P&N_FLAG) PC.W++; else { M_JR; } break; /* BPL * REL */
case 0x30: if(P&N_FLAG) { M_JR; } else PC.W++; break; /* BMI * REL */
case 0xD0: if(P&Z_FLAG) PC.W++; else { M_JR; } break; /* BNE * REL */
case 0xF0: if(P&Z_FLAG) { M_JR; } else PC.W++; break; /* BEQ * REL */
case 0x90: if(P&C_FLAG) PC.W++; else { M_JR; } break; /* BCC * REL */
case 0xB0: if(P&C_FLAG) { M_JR; } else PC.W++; break; /* BCS * REL */
case 0x50: if(P&V_FLAG) PC.W++; else { M_JR; } break; /* BVC * REL */
case 0x70: if(P&V_FLAG) { M_JR; } else PC.W++; break; /* BVS * REL */

/* RTI */
case 0x40:
  M_POP(P);P|=R_FLAG;M_POP(PC.B.l);M_POP(PC.B.h);
  break;

/* RTS */
case 0x60:
  M_POP(PC.B.l);M_POP(PC.B.h);PC.W++;break;

/* JSR $ssss ABS */
case 0x20:
  K.B.l=Op6502(PC.W++);
  K.B.h=Op6502(PC.W);
  M_PUSH(PC.B.h);
  M_PUSH(PC.B.l);
  PC=K;break;

/* JMP $ssss ABS */
case 0x4C: M_LDWORD(K);PC=K;break;

/* JMP ($ssss) ABDINDIR */
case 0x6C:
  M_LDWORD(K);
  PC.B.l=Rd6502(K.W++);
  PC.B.h=Rd6502(K.W);
  break;

/* BRK */
case 0x00:
  PC.W++;
  M_PUSH(PC.B.h);M_PUSH(PC.B.l);
  M_PUSH(P|B_FLAG);
  P=(P|I_FLAG)&~D_FLAG;
  PC.B.l=Rd6502(0xFFFE);
  PC.B.h=Rd6502(0xFFFF);
  break;

case 0x08: M_PUSH(P);break;               /* PHP */
case 0x28: M_POP(P);P|=R_FLAG;break;      /* PLP */
case 0x18: P&=~C_FLAG;break;              /* CLC */
case 0xB8: P&=~V_FLAG;break;              /* CLV */
case 0xD8: P&=~D_FLAG;break;              /* CLD */
case 0x58: P&=~I_FLAG;break;              /* CLI */
case 0x38: P|=C_FLAG;break;               /* SEC */
case 0xF8: P|=D_FLAG;break;               /* SED */
case 0x78: P|=I_FLAG;break;               /* SEI */
case 0x48: M_PUSH(A);break;               /* PHA */
case 0x68: M_POP(A);M_FL(A);break;        /* PLA */
case 0x98: A=Y;M_FL(A);break;             /* TYA */
case 0xA8: Y=A;M_FL(Y);break;             /* TAY */
case 0xC8: Y++;M_FL(Y);break;             /* INY */
case 0x88: Y--;M_FL(Y);break;             /* DEY */
case 0x8A: A=X;M_FL(A);break;             /* TXA */
case 0xAA: X=A;M_FL(X);break;             /* TAX */
case 0xE8: X++;M_FL(X);break;             /* INX */
case 0xCA: X--;M_FL(X);break;             /* DEX */
case 0xEA: break;                         /* NOP */
case 0x9A: S=X;break;                     /* TXS */
case 0xBA: X=S;break;                     /* TSX */

case 0x24: MR_Zp(I);M_BIT(I);break;       /* BIT $ss ZP */
case 0x2C: MR_Ab(I);M_BIT(I);break;       /* BIT $ssss ABS */

case 0x05: MR_Zp(I);M_ORA(I);break;       /* ORA $ss ZP */
case 0x06: MM_Zp(M_ASL);break;            /* ASL $ss ZP */
case 0x25: MR_Zp(I);M_AND(I);break;       /* AND $ss ZP */
case 0x26: MM_Zp(M_ROL);break;            /* ROL $ss ZP */
case 0x45: MR_Zp(I);M_EOR(I);break;       /* EOR $ss ZP */
case 0x46: MM_Zp(M_LSR);break;            /* LSR $ss ZP */
case 0x65: MR_Zp(I);M_ADC(I);break;       /* ADC $ss ZP */
case 0x66: MM_Zp(M_ROR);break;            /* ROR $ss ZP */
case 0x84: MW_Zp(Y);break;                /* STY $ss ZP */
case 0x85: MW_Zp(A);break;                /* STA $ss ZP */
case 0x86: MW_Zp(X);break;                /* STX $ss ZP */
case 0xA4: MR_Zp(Y);M_FL(Y);break;        /* LDY $ss ZP */
case 0xA5: MR_Zp(A);M_FL(A);break;        /* LDA $ss ZP */
case 0xA6: MR_Zp(X);M_FL(X);break;        /* LDX $ss ZP */
case 0xC4: MR_Zp(I);M_CMP(Y,I);break;     /* CPY $ss ZP */
case 0xC5: MR_Zp(I);M_CMP(A,I);break;     /* CMP $ss ZP */
case 0xC6: MM_Zp(M_DEC);break;            /* DEC $ss ZP */
case 0xE4: MR_Zp(I);M_CMP(X,I);break;     /* CPX $ss ZP */
case 0xE5: MR_Zp(I);M_SBC(I);break;       /* SBC $ss ZP */
case 0xE6: MM_Zp(M_INC);break;            /* INC $ss ZP */

case 0x0D: MR_Ab(I);M_ORA(I);break;       /* ORA $ssss ABS */
case 0x0E: MM_Ab(M_ASL);break;            /* ASL $ssss ABS */
case 0x2D: MR_Ab(I);M_AND(I);break;       /* AND $ssss ABS */
case 0x2E: MM_Ab(M_ROL);break;            /* ROL $ssss ABS */
case 0x4D: MR_Ab(I);M_EOR(I);break;       /* EOR $ssss ABS */
case 0x4E: MM_Ab(M_LSR);break;            /* LSR $ssss ABS */
case 0x6D: MR_Ab(I);M_ADC(I);break;       /* ADC $ssss ABS */
case 0x6E: MM_Ab(M_ROR);break;            /* ROR $ssss ABS */
case 0x8C: MW_Ab(Y);break;                /* STY $ssss ABS */
case 0x8D: MW_Ab(A);break;                /* STA $ssss ABS */
case 0x8E: MW_Ab(X);break;                /* STX $ssss ABS */
case 0xAC: MR_Ab(Y);M_FL(Y);break;        /* LDY $ssss ABS */
case 0xAD: MR_Ab(A);M_FL(A);break;        /* LDA $ssss ABS */
case 0xAE: MR_Ab(X);M_FL(X);break;        /* LDX $ssss ABS */
case 0xCC: MR_Ab(I);M_CMP(Y,I);break;     /* CPY $ssss ABS */
case 0xCD: MR_Ab(I);M_CMP(A,I);break;     /* CMP $ssss ABS */
case 0xCE: MM_Ab(M_DEC);break;            /* DEC $ssss ABS */
case 0xEC: MR_Ab(I);M_CMP(X,I);break;     /* CPX $ssss ABS */
case 0xED: MR_Ab(I);M_SBC(I);break;       /* SBC $ssss ABS */
case 0xEE: MM_Ab(M_INC);break;            /* INC $ssss ABS */

case 0x09: MR_Im(I);M_ORA(I);break;       /* ORA #$ss IMM */
case 0x29: MR_Im(I);M_AND(I);break;       /* AND #$ss IMM */
case 0x49: MR_Im(I);M_EOR(I);break;       /* EOR #$ss IMM */
case 0x69: MR_Im(I);M_ADC(I);break;       /* ADC #$ss IMM */
case 0xA0: MR_Im(Y);M_FL(Y);break;        /* LDY #$ss IMM */
case 0xA2: MR_Im(X);M_FL(X);break;        /* LDX #$ss IMM */
case 0xA9: MR_Im(A);M_FL(A);break;        /* LDA #$ss IMM */
case 0xC0: MR_Im(I);M_CMP(Y,I);break;     /* CPY #$ss IMM */
case 0xC9: MR_Im(I);M_CMP(A,I);break;     /* CMP #$ss IMM */
case 0xE0: MR_Im(I);M_CMP(X,I);break;     /* CPX #$ss IMM */
case 0xE9: MR_Im(I);M_SBC(I);break;       /* SBC #$ss IMM */

case 0x15: MR_Zx(I);M_ORA(I);break;       /* ORA $ss,x ZP,x */
case 0x16: MM_Zx(M_ASL);break;            /* ASL $ss,x ZP,x */
case 0x35: MR_Zx(I);M_AND(I);break;       /* AND $ss,x ZP,x */
case 0x36: MM_Zx(M_ROL);break;            /* ROL $ss,x ZP,x */
case 0x55: MR_Zx(I);M_EOR(I);break;       /* EOR $ss,x ZP,x */
case 0x56: MM_Zx(M_LSR);break;            /* LSR $ss,x ZP,x */
case 0x75: MR_Zx(I);M_ADC(I);break;       /* ADC $ss,x ZP,x */
case 0x76: MM_Zx(M_ROR);break;            /* ROR $ss,x ZP,x */
case 0x94: MW_Zx(Y);break;                /* STY $ss,x ZP,x */
case 0x95: MW_Zx(A);break;                /* STA $ss,x ZP,x */
case 0x96: MW_Zy(X);break;                /* STX $ss,y ZP,y */
case 0xB4: MR_Zx(Y);M_FL(Y);break;        /* LDY $ss,x ZP,x */
case 0xB5: MR_Zx(A);M_FL(A);break;        /* LDA $ss,x ZP,x */
case 0xB6: MR_Zy(X);M_FL(X);break;        /* LDX $ss,y ZP,y */
case 0xD5: MR_Zx(I);M_CMP(A,I);break;     /* CMP $ss,x ZP,x */
case 0xD6: MM_Zx(M_DEC);break;            /* DEC $ss,x ZP,x */
case 0xF5: MR_Zx(I);M_SBC(I);break;       /* SBC $ss,x ZP,x */
case 0xF6: MM_Zx(M_INC);break;            /* INC $ss,x ZP,x */

case 0x19: MR_Ay(I);M_ORA(I);break;       /* ORA $ssss,y ABS,y */
case 0x1D: MR_Ax(I);M_ORA(I);break;       /* ORA $ssss,x ABS,x */
case 0x1E: MM_Ax(M_ASL);break;            /* ASL $ssss,x ABS,x */
case 0x39: MR_Ay(I);M_AND(I);break;       /* AND $ssss,y ABS,y */
case 0x3D: MR_Ax(I);M_AND(I);break;       /* AND $ssss,x ABS,x */
case 0x3E: MM_Ax(M_ROL);break;            /* ROL $ssss,x ABS,x */
case 0x59: MR_Ay(I);M_EOR(I);break;       /* EOR $ssss,y ABS,y */
case 0x5D: MR_Ax(I);M_EOR(I);break;       /* EOR $ssss,x ABS,x */
case 0x5E: MM_Ax(M_LSR);break;            /* LSR $ssss,x ABS,x */
case 0x79: MR_Ay(I);M_ADC(I);break;       /* ADC $ssss,y ABS,y */
case 0x7D: MR_Ax(I);M_ADC(I);break;       /* ADC $ssss,x ABS,x */
case 0x7E: MM_Ax(M_ROR);break;            /* ROR $ssss,x ABS,x */
case 0x99: MW_Ay(A);break;                /* STA $ssss,y ABS,y */
case 0x9D: MW_Ax(A);break;                /* STA $ssss,x ABS,x */
case 0xB9: MR_Ay(A);M_FL(A);break;        /* LDA $ssss,y ABS,y */
case 0xBC: MR_Ax(Y);M_FL(Y);break;        /* LDY $ssss,x ABS,x */
case 0xBD: MR_Ax(A);M_FL(A);break;        /* LDA $ssss,x ABS,x */
case 0xBE: MR_Ay(X);M_FL(X);break;        /* LDX $ssss,y ABS,y */
case 0xD9: MR_Ay(I);M_CMP(A,I);break;     /* CMP $ssss,y ABS,y */
case 0xDD: MR_Ax(I);M_CMP(A,I);break;     /* CMP $ssss,x ABS,x */
case 0xDE: MM_Ax(M_DEC);break;            /* DEC $ssss,x ABS,x */
case 0xF9: MR_Ay(I);M_SBC(I);break;       /* SBC $ssss,y ABS,y */
case 0xFD: MR_Ax(I);M_SBC(I);break;       /* SBC $ssss,x ABS,x */
case 0xFE: MM_Ax(M_INC);break;            /* INC $ssss,x ABS,x */

case 0x01: MR_Ix(I);M_ORA(I);break;       /* ORA ($ss,x) INDEXINDIR */
case 0x11: MR_Iy(I);M_ORA(I);break;       /* ORA ($ss),y INDIRINDEX */
case 0x21: MR_Ix(I);M_AND(I);break;       /* AND ($ss,x) INDEXINDIR */
case 0x31: MR_Iy(I);M_AND(I);break;       /* AND ($ss),y INDIRINDEX */
case 0x41: MR_Ix(I);M_EOR(I);break;       /* EOR ($ss,x) INDEXINDIR */
case 0x51: MR_Iy(I);M_EOR(I);break;       /* EOR ($ss),y INDIRINDEX */
case 0x61: MR_Ix(I);M_ADC(I);break;       /* ADC ($ss,x) INDEXINDIR */
case 0x71: MR_Iy(I);M_ADC(I);break;       /* ADC ($ss),y INDIRINDEX */
case 0x81: MW_Ix(A);break;                /* STA ($ss,x) INDEXINDIR */
case 0x91: MW_Iy(A);break;                /* STA ($ss),y INDIRINDEX */
case 0xA1: MR_Ix(A);M_FL(A);break;        /* LDA ($ss,x) INDEXINDIR */
case 0xB1: MR_Iy(A);M_FL(A);break;        /* LDA ($ss),y INDIRINDEX */
case 0xC1: MR_Ix(I);M_CMP(A,I);break;     /* CMP ($ss,x) INDEXINDIR */
case 0xD1: MR_Iy(I);M_CMP(A,I);break;     /* CMP ($ss),y INDIRINDEX */
case 0xE1: MR_Ix(I);M_SBC(I);break;       /* SBC ($ss,x) INDEXINDIR */
case 0xF1: MR_Iy(I);M_SBC(I);break;       /* SBC ($ss),y INDIRINDEX */

case 0x0A: M_ASL(A);break;                /* ASL a ACC */
case 0x2A: M_ROL(A);break;                /* ROL a ACC */
case 0x4A: M_LSR(A);break;                /* LSR a ACC */
case 0x6A: M_ROR(A);break;                /* ROR a ACC */
