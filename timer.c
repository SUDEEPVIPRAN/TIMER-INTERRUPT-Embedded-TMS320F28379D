#include "F28x_Project.h"
void GPIO(void);
void Timer(void);
void PIE(void);
void Clock(void);
interrupt void ISR(void);
int i;
int main()
{
 EALLOW;
 DINT;
 Clock();
 GPIO();
 Timer();
 PIE();
 IER=0xffff;
 EINT;
 EDIS;
 while(1)
 {
 }
 return 0;
}
void Clock(void)
{
 ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL=0; // primary oscillator select
 ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN=0; // bypass pll
 for(i=0;i<120;i++);
 ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV=0; //wait for 120 oscillator cycle
 for(i=0;i<=4;i++) // lock PLL 5 times
 {
 ClkCfgRegs.SYSPLLCTL1.bit.PLLEN= 0; // lock PLL
 ClkCfgRegs.SYSPLLMULT.bit.IMULT=12; // 120Mhz PLL raw clock
 ClkCfgRegs.SYSPLLMULT.bit.FMULT=0; //0.0 fractional multiplier
 while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1);
 }
 ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV=1; //Desired+1
 ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN=1; // switch to PLL
 ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV=0; //divides by 1, 120Mhz clock
 ClkCfgRegs.LOSPCP.bit.LSPCLKDIV=1; // 80Mhz LSP clock
 CpuSysRegs.PCLKCR7.bit.SCI_A=1; // Enable clock for SCIA
}
void GPIO(void)
{
 GpioCtrlRegs.GPAPUD.bit.GPIO31=1;//Pull up is disabled
 GpioCtrlRegs.GPADIR.bit.GPIO31=1;//output->GPIO
 GpioCtrlRegs.GPAINV.bit.GPIO31=0;//output is not inverted
}
void Timer(void)
{
 CpuTimer0Regs.TCR.bit.TSS=1; // stop timer 0
 CpuTimer0Regs.TCR.bit.TIF=1;//clear flag
 CpuTimer0Regs.PRD.all=100;//period=10kHz
 CpuTimer0Regs.TPR.bit.TDDR=120;//prescalar =120
 CpuTimer0Regs.TCR.bit.FREE=1;//run free
 CpuTimer0Regs.TCR.bit.TRB=1;//reload
 CpuTimer0Regs.TCR.bit.TIE=1;//enable interrupt
 CpuTimer0Regs.TCR.bit.TSS=0; //start the timer 0
}
void PIE(void)
{
 PieCtrlRegs.PIECTRL.bit.ENPIE=1;//enable interrupt
 PieVectTable.TIMER0_INT = &ISR;//address for interrupt
 PieCtrlRegs.PIEIER1.bit.INTx7=1;//timer 0 interrupt is enabled
 PieCtrlRegs.PIEACK.bit.ACK1=1;//group 1 for timer 0
}
interrupt void ISR(void)
{
 CpuTimer0Regs.TCR.bit.TIF=1;//clear flag
 GpioDataRegs.GPATOGGLE.bit.GPIO31=1;
 PieCtrlRegs.PIEACK.bit.ACK1=1;//group 1 for timer 0
}
