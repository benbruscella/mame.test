/***************************************************************************

  cpuintrf.c

  Don't you love MS-DOS 8+3 names? That stands for CPU interface.
  Functions needed to interface the CPU emulator with the other parts of
  the emulation.

***************************************************************************/

#include "driver.h"
#include "Z80.h"
/* I can't include M6502.h because it redefines some types. Here are the things I need: */
                               /* Interrupt6502() returns:   */
#define INT_NONE  0            /* No interrupt required      */
#define INT_IRQ	  1            /* Standard IRQ interrupt     */
#define INT_NMI	  2            /* Non-maskable interrupt     */


static int activecpu;

static const struct MemoryReadAddress *memoryread;
static const struct MemoryWriteAddress *memorywrite;


void cpu_run(void)
{
	int totalcpu;
	unsigned char cpucontext[MAX_CPU][64];	/* enough to accomodate the cpu status */
extern void Reset6502(void *R,int IPeriod);
extern word Run6502(void *R);


	/* count how many CPUs we have to emulate */
	totalcpu = 0;
	while (totalcpu < MAX_CPU && Machine->drv->cpu[totalcpu].cpu_type != 0) totalcpu++;

reset:
	for (activecpu = 0;activecpu < totalcpu;activecpu++)
	{
		int cycles;


		cycles = Machine->drv->cpu[activecpu].cpu_clock /
				(Machine->drv->frames_per_second * Machine->drv->cpu[activecpu].interrupts_per_frame);

		switch(Machine->drv->cpu[activecpu].cpu_type)
		{
			case CPU_Z80:
				Z80_IPeriod = cycles;
				Z80_Reset();
				break;
			case CPU_M6502:
				/* Reset6502() needs to read memory to get the PC start address */
				memoryread = Machine->drv->cpu[activecpu].memory_read;
				Reset6502(cpucontext[activecpu],cycles);
				break;
		}
	}


	do
	{
		for (activecpu = 0;activecpu < totalcpu;activecpu++)
		{
			int loops;


			memoryread = Machine->drv->cpu[activecpu].memory_read;
			memorywrite = Machine->drv->cpu[activecpu].memory_write;

			switch(Machine->drv->cpu[activecpu].cpu_type)
			{
				case CPU_Z80:
					for (loops = 0;loops < Machine->drv->cpu[activecpu].interrupts_per_frame;loops++)
						Z80_Execute();
					break;

				case CPU_M6502:
					for (loops = 0;loops < Machine->drv->cpu[activecpu].interrupts_per_frame;loops++)
						Run6502(cpucontext[activecpu]);
					break;
			}
		}

		/* if F3 is pressed, reset the machine */
		if (osd_key_pressed(OSD_KEY_F3))
			goto reset;
	} while (updatescreen() == 0);
}



/* some functions commonly used by emulators */

int readinputport(int port)
{
	int res,i;
	struct InputPort *in;


	in = &Machine->drv->input_ports[port];

	res = in->default_value;

	for (i = 7;i >= 0;i--)
	{
		int c;


		c = in->keyboard[i];
		if (c && osd_key_pressed(c))
			res ^= (1 << i);
		else
		{
			c = in->joystick[i];
			if (c && osd_joy_pressed(c))
				res ^= (1 << i);
		}
	}

	return res;
}



int input_port_0_r(int offset)
{
	return readinputport(0);
}



int input_port_1_r(int offset)
{
	return readinputport(1);
}



int input_port_2_r(int offset)
{
	return readinputport(2);
}



int input_port_3_r(int offset)
{
	return readinputport(3);
}



int input_port_4_r(int offset)
{
	return readinputport(4);
}



int input_port_5_r(int offset)
{
	return readinputport(5);
}



/* start with interrupts enabled, so the generic routine will work even if */
/* the machine doesn't have an interrupt enable port */
static int interrupt_enable = 1;
static int interrupt_vector = 0xff;

void interrupt_enable_w(int offset,int data)
{
	interrupt_enable = data;
}



void interrupt_vector_w(int offset,int data)
{
	interrupt_vector = data;
}



/* If the game you are emulating doesn't have vertical blank interrupts */
/* (like Lady Bug) you'll have to provide your own interrupt function (set */
/* a flag there, and return the appropriate value from the appropriate input */
/* port when the game polls it) */
int interrupt(void)
{
	switch(Machine->drv->cpu[activecpu].cpu_type)
	{
		case CPU_Z80:
			if (interrupt_enable == 0) return Z80_IGNORE_INT;
			else return interrupt_vector;
			break;

		case CPU_M6502:
			if (interrupt_enable == 0) return INT_NONE;
			else return INT_IRQ;
			break;

		default:
			return -1;
			break;
	}
}



int nmi_interrupt(void)
{
	switch(Machine->drv->cpu[activecpu].cpu_type)
	{
		case CPU_Z80:
			if (interrupt_enable == 0) return Z80_IGNORE_INT;
			else return Z80_NMI_INT;
			break;

		case CPU_M6502:
			if (interrupt_enable == 0) return INT_NONE;
			else return INT_NMI;
			break;

		default:
			return -1;
			break;
	}
}



/***************************************************************************

  Perform a memory read. This function is called by the CPU emulation.

***************************************************************************/
int cpu_readmem(register int A)
{
	const struct MemoryReadAddress *mra;


	mra = memoryread;
	while (mra->start != -1)
	{
		if (A >= mra->start && A <= mra->end)
		{
			int (*handler)() = mra->handler;


			if (handler == MRA_NOP) return 0;
			else if (handler == MRA_RAM || handler == MRA_ROM) return RAM[A];
			else return (*handler)(A - mra->start);
		}

		mra++;
	}

	if (errorlog) fprintf(errorlog,"%04x: warning - read unmapped memory address %04x\n",Z80_GetPC(),A);
	return RAM[A];
}



/***************************************************************************

  Perform a memory write. This function is called by the CPU emulation.

***************************************************************************/
void cpu_writemem(register int A,register unsigned char V)
{
	const struct MemoryWriteAddress *mwa;


	mwa = memorywrite;
	while (mwa->start != -1)
	{
		if (A >= mwa->start && A <= mwa->end)
		{
			void (*handler)() = mwa->handler;


			if (handler == MWA_NOP) return;
			else if (handler == MWA_RAM) RAM[A] = V;
			else if (handler == MWA_ROM)
			{
				if (errorlog) fprintf(errorlog,"%04x: warning - write %02x to ROM address %04x\n",Z80_GetPC(),V,A);
			}
			else (*handler)(A - mwa->start,V);

			return;
		}

		mwa++;
	}

	if (errorlog) fprintf(errorlog,"%04x: warning - write %02x to unmapped memory address %04x\n",Z80_GetPC(),V,A);
	RAM[A] = V;
}



byte Z80_In(byte Port)
{
	const struct IOReadPort *iorp;


	iorp = Machine->drv->cpu[activecpu].port_read;
	if (iorp)
	{
		while (iorp->start != -1)
		{
			if (Port >= iorp->start && Port <= iorp->end)
			{
				int (*handler)() = iorp->handler;


				if (handler == IORP_NOP) return 0;
				else return (*handler)(Port - iorp->start);
			}

			iorp++;
		}
	}

	if (errorlog) fprintf(errorlog,"%04x: warning - read unmapped I/O port %02x\n",Z80_GetPC(),Port);
	return 0;
}



void Z80_Out(byte Port,byte Value)
{
	const struct IOWritePort *iowp;


	iowp = Machine->drv->cpu[activecpu].port_write;
	if (iowp)
	{
		while (iowp->start != -1)
		{
			if (Port >= iowp->start && Port <= iowp->end)
			{
				void (*handler)() = iowp->handler;


				if (handler == IOWP_NOP) return;
				else (*handler)(Port - iowp->start,Value);

				return;
			}

			iowp++;
		}
	}

	if (errorlog) fprintf(errorlog,"%04x: warning - write %02x to unmapped I/O port %02x\n",Z80_GetPC(),Value,Port);
}



/***************************************************************************

  Interrupt handler. This function is called at regular intervals
  (determined by IPeriod) by the CPU emulation.

***************************************************************************/

int Z80_IRQ = Z80_IGNORE_INT;	/* needed by the CPU emulation */

int cpu_interrupt(void)
{
	return (*Machine->drv->cpu[activecpu].interrupt)();
}



void Z80_Patch (Z80_Regs *Regs)
{
}



void Z80_Reti (void)
{
}



void Z80_Retn (void)
{
}
