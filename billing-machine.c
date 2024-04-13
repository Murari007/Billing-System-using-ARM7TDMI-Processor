
//working 10-bit internal ADC 
//AIN0 pin is selected 
//you can change the channel by changing PINSEL1 and ADCR value	
#include <lpc214x.h>
#include <stdio.h> 
#include <string.h>
unsigned int i;
void lcd_init(void);
void delay(unsigned long int);
void extint0_ISR(void)__irq;
void cmd(unsigned int );
void hascii(int *);
unsigned  int adc_value=0,temp_adc=0;
void data(unsigned int);
void init_UART(void);
void transferdata(unsigned int);
void nextline(void);
void buzzer(void);
int temp;
int var[10];
char val[10];
char arr[20];
unsigned int x;
int price1=80;
int price2=90;
int price3=200;
int result=0;
unsigned char next[]={"\r\n"};
unsigned char msg[]={"ADC VALUE:"};
unsigned char mg[]={"TOTAL:"};
unsigned char head1[]={"PRODUCT NAME"};
unsigned char p1[]={"RICE "};
unsigned char p2[]={"WHEAT "};
unsigned char p3[]={"SUGAR "};
unsigned char r1[]={"PRICE : 80"};
unsigned char r2[]={"PRICE : 90"};
unsigned char r3[]={"PRICE : 50"};
unsigned char p4[]={"Invalid product"};
int main()
{	 
	 PINSEL1|=0x00000001;
	 EXTMODE|=0x00000001;
	 VICVectAddr0=(unsigned long)extint0_ISR;
	 VICVectCntl0=0x20|14;
	 VICIntEnable|=0x00004000;
	init_UART();	
	while(1);
 }
void lcd_init(void)
{
  unsigned int i;
  unsigned  int c[]={0x30,0x20,0x28,0x01,0x06,0x0E,0x80};
  IOCLR0=0x000000fc;
	for(i=0;i<6;i++)
  {
  cmd(c[i]);
  delay(10000);
  }
}
void init_UART(void)
{
  PINSEL0=0X00000005;
	U0LCR=0X00000083;
	U0DLL=0X00000061;
	U0DLM=0X00000000;
	U0LCR=0X00000003;
}
void transferdata(unsigned int value)
{
	unsigned int y;
	y=value;
	U0THR= y;
	while(!(U0LSR & 0X00000020));
}
void cmd(unsigned int value)
{
unsigned int y;
y=value;
y= y & 0xf0;
IOCLR0=0x000000fc;
IOCLR0=0X00000004;
IOSET0=y;
delay(1000000);
IOSET0=0x00000008;
delay(10);
IOCLR0=0x00000008;
y=value;
y=y & 0x0f;
y=y<<4;
IOCLR0=0x000000fc;
IOCLR0=0X00000004;
IOSET0=y; 
IOSET0=0x00000008;
delay(10);
IOCLR0=0x00000008;
}
void data(unsigned int dat)
{
unsigned int y;
y=dat;
y=y & 0xf0;
IOCLR0=0x000000fc;
IOSET0=0X00000004;
IOSET0=y; 
	delay(1000000);
IOSET0=0x00000008;
delay(10);
IOCLR0=0x00000008;
y=dat;
y=y & 0x0f;
y=y<<4;
IOCLR0=0x000000fc;	
IOSET0=0X00000004;
IOSET0=y;
IOSET0=0x00000008;
delay(10);
IOCLR0=0x00000008;
}
void delay(unsigned  long int r1)
{
  	unsigned  long int r;
  	for(r=0;r<r1;r++);
}
 void hascii(int var[])
 {
 char asc[20]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
 
 for (i=0;i<3;i++)
 val[i]=asc[var[i]];
 }
void nextline(void)
 {
	 int i;
	 for(i=0;i<3;i++)
	 {
		 while(!(U0LSR  & 0x20));
		 U0THR=next[i];
	 }
 }
void buzzer()
 {
	 IODIR0|=0x00004000;
	 IOSET0=0x00004000;
	 delay(5000000);
	 IOCLR0=0x00004000;
	 delay(5000000);
	 delay(5000000);
	 IODIR0=0x000000FC;
}
void extint0_ISR(void)__irq
{
	 PINSEL1  |=0X00040000;	//AD0.4 pin is selected(P0.25)
	 PINSEL0 |=0x00000000;
   IODIR0 |= 0x000000FC;   //configure o/p lines for lcd  
	 lcd_init();
		IODIR0 |= 0x000000FC;
		//CONTROL register for ADC
		buzzer();
		AD0CR = 0x09200010;				  	//command register for ADC-AD0.4
	  delay(100);
		while((temp_adc = AD0GDR) == 0x80000000);	//to check the interrupt bit
		adc_value = AD0GDR;				//reading the ADC value
		adc_value >>=6;
		adc_value &= 0x000003ff;
		x=adc_value;
 		temp=adc_value & 0x00000f00;
		var[0]= temp>>8;
		temp=adc_value & 0x000000f0;
		var[1]= temp>>4;
		temp=adc_value & 0x0000000f; 
		var[2]=temp;
	  hascii(var);
    nextline();		 
		for(i=0;i<=9;i++)
		{
			delay(65000);
			delay(65000);
			delay(65000);
			data(msg[i]);
			transferdata(msg[i]);
		}
		delay(65000);
		delay(65000);
		cmd(0xc0);
		for(i=0;i<3;i++)
    {
		data(val[i]);
	  transferdata(val[i]);
    delay(32000); 
	  delay(32000);
		delay(65000);
    }    
	    if(x >= 0x00000300 && x<0x000003ff)
	    {
				   cmd(0x01);
		       nextline();
		       result = result + price1;
		       cmd(0x80);
		       for(i=0;i<=11;i++)
		       {
				   delay(65000);
				   delay(65000);
				   data(head1[i]);
		       }
					 delay(6500000);
		       cmd(0x01);
		       cmd(0x80);
		       for(i=0;i<=4;i++)
		       {
			     delay(65000);
			     data(p1[i]);
		       }
	       	 cmd(0xc0);
		       for(i=0;i<=9;i++)
		      {
			     data(r1[i]);
		      }
					nextline();
			    for(i=0;i<=11;i++)
			    {
		      transferdata(head1[i]);
			    }
					for(i=0;i<=4;i++)
					{
					transferdata(p1[i]);
					}
				  nextline();
					for(i=0;i<=9;i++)
					{
						transferdata(r1[i]);
					}
					sprintf(arr,"%d",result);
		      nextline();
					for(i=0;i<6;i++)
		      {
			    transferdata(mg[i]);
		      }
		      for(i=0;i<5;i++)
		      {
		      transferdata(arr[i]);
		      }
					
		 }
			else if(x >=0x00000200 && x<0x000002ff)
	    {
				   cmd(0x01);
		       nextline();
		       result = result + price2;
		       cmd(0x80);
		       for(i=0;i<=11;i++)
		       {
				   delay(65000);
				   delay(65000);
				   data(head1[i]);
		       }
					 delay(6500000);
		       cmd(0x01);
		       cmd(0x80);
		       for(i=0;i<=5;i++)
		       {
			     delay(65000);
			     data(p2[i]);
		       }
	       	cmd(0xc0);
		      for(i=0;i<=9;i++)
		      {
			     data(r2[i]);
		      }
					nextline();
			    for(i=0;i<=11;i++)
			    {
		      transferdata(head1[i]);
			    }
					for(i=0;i<=5;i++)
					{
					transferdata(p2[i]);
					}
					nextline();
					for(i=0;i<=9;i++)
					{
						transferdata(r2[i]);
					}
					sprintf(arr,"%d",result);
		      nextline();
					for(i=0;i<6;i++)
		      {
			    transferdata(mg[i]);
		       }
		      for(i=0;i<5;i++)
		      {
		      transferdata(arr[i]);
		      }
		 }
		else if(x >=0x00000100 && x<0x000001ff)
	    {
				   cmd(0x01);
		       nextline();
		       result = result + price3;
		       cmd(0x80);
		       for(i=0;i<=11;i++)
		       {
				   delay(65000);
				   delay(65000);
				   data(head1[i]);
		       }
					 delay(6500000);
		       cmd(0x01);
		       cmd(0x80);
		       for(i=0;i<=5;i++)
		       {
			     delay(65000);
			     data(p3[i]);
		       }
	       	cmd(0xc0);
		      for(i=0;i<=9;i++)
		      {
			     data(r3[i]);
		      }
					nextline();
			    for(i=0;i<=11;i++)
			    {
		      transferdata(head1[i]);
			    }
					for(i=0;i<=5;i++)
					{
					transferdata(p3[i]);
					}
					nextline();
					for(i=0;i<=9;i++)
					{
						transferdata(r3[i]);
					}
					sprintf(arr,"%d",result);
		      nextline();
					for(i=0;i<6;i++)
		      {
			    transferdata(mg[i]);
		      }
		      for(i=0;i<5;i++)
		      {
		      transferdata(arr[i]);
		      }
		 }
			else
	    {  
				  
				   cmd(0x01);
		       nextline();
		       result = result + 0;
		       cmd(0x80);
		       for(i=0;i<=11;i++)
		       {
				   delay(65000);
				   delay(65000);
				   data(head1[i]);
		       }
					 delay(6500000);
		       cmd(0x01);
		       cmd(0x80);
		       for(i=0;i<=15;i++)
		       {
			     delay(65000);
			     data(p4[i]);
		       }
			     nextline();
					 for(i=0;i<=15;i++)
					 {
						 transferdata(p4[i]);
					 }
					 sprintf(arr,"%d",result);
		       nextline();
					 for(i=0;i<6;i++)
		       {
			     transferdata(mg[i]);
		       }
		       for(i=0;i<5;i++)
		       {
		       transferdata(arr[i]);
		       }
		 }
	  cmd(0x01);
		delay(120000); 
		delay(120000);
	EXTINT|=0X00000001;	//Clear interrupt flag
	VICVectAddr=0;		// End of interrupt execution
}