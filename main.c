#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

#define SSD_DATA PORTA
#define SSD_DATA_DDR DDRA
#define SSD_DIG PORTC
#define SSD_DIG_DDR DDRC
#define Button_DDR DDRD
#define Button_PORT PORTD

#define DIG0 0 
#define DIG1 1
#define DIG2 2
#define DIG3 3
#define DIG4 4
#define DIG5 5
#define DIG6 6
#define DIG7 7

volatile unsigned char data =0xff;
volatile unsigned int minitsecond=0,second=0,min=0,h=0,btn=0;          

unsigned char ssd[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char ssd_dig[]={0,0,0,0};

void ssd_split(void);             //prototype
void ssd_count(void);
void ssd_display(unsigned int c);
void init_ssd(void);



int main(void){
 SSD_DATA_DDR=0xff; 
 
 TCNT0 = 0;
 OCR0 = 124;            //get prescaler > 8
 TIMSK = 1<<OCIE0;
 
 
 GICR |= 1<<INT0|1<<INT1;
 MCUCR |=1<<ISC01 | 1<<ISC00|1<<ISC11|1<<ISC10;
 
 sei();
 init_ssd();
 
 Button_DDR&=~(1<<DIG2); //DDR=0 input the button
 PIND&=~(1<<DIG2);         //de
 
 

     while(1){
		     ssd_split();
		     ssd_display(1);
	 }
	 return 0;
}

void init_ssd(void){
     SSD_DATA_DDR=0xff;  
	 SSD_DIG &= ~(1<<DIG0 | 1<<DIG1 | 1<<DIG2 | 1<<DIG3|1<<DIG4|1<<DIG5|1<<DIG6);      // PORTC 0
	 SSD_DIG_DDR |=(1<<DIG0 | 1<<DIG1 | 1<<DIG2 | 1<<DIG3|1<<DIG4|1<<DIG5|1<<DIG6);    //DDRC 
     SSD_DATA=0x00;
}

void ssd_split(void) {                     // minitsecond=0,second=0,min=0,h=0 
     if(min<10){                          //first if loop
   
      	 ssd_dig[0]=(min);            
	     ssd_dig[1]=(second/10);
		 ssd_dig[2]=(second%10);
		 ssd_dig[3]=(minitsecond)/100;
     }
	 
	 if(min>10){                          //second if loop
	 
	     ssd_dig[0]=(h);
	     ssd_dig[1]=(min/10);
		 ssd_dig[2]=(min%10);
		 ssd_dig[3]=(second)/10;
	 }
}	 
	 
void ssd_display(unsigned int c){
	     unsigned int i=0;
		 
	     for(i=0;i<=c;i++){
		 
			 SSD_DATA=ssd[ssd_dig[0]]| (1<<DIG7);
			 SSD_DIG=1<<DIG0;
			 _delay_ms(1);
			 SSD_DIG&=~(1<<DIG0);
			 SSD_DATA=0x00;
			 SSD_DIG=0x00;
			 
			 SSD_DATA=ssd[ssd_dig[1]];
			 SSD_DIG=1<<DIG1;
			 _delay_ms(1);
			 SSD_DIG&=~(1<<DIG1);
			 SSD_DATA=0x00;
			 SSD_DIG=0x00;
			 
			 SSD_DATA=ssd[ssd_dig[2]]|(1<<DIG7);
			 SSD_DIG=1<<DIG2;
			 _delay_ms(1);
			 SSD_DIG&=~(1<<DIG2);
			 SSD_DATA=0x00;
			 SSD_DIG=0x00;
			 
			 SSD_DATA=ssd[ssd_dig[3]];
			 SSD_DIG=1<<DIG3;
			 _delay_ms(1);
			 SSD_DIG&=~(1<<DIG3);
			 SSD_DATA=0x00;
			 SSD_DIG=0x00;
	     }
	 }	

ISR(TIMER0_COMP_vect){                       // minitsecond=0,second=0,min=0,h=0

	     if(minitsecond==1000){
		     minitsecond=0;
			 second++;
			   
			     if(second==60){
				     second=0;
					 min++;
					 
					     if(min==60){
						     min=0;
							 h++;
						 }
				 }
		 }
		 else{
		      minitsecond++;
		 }
} 

ISR(INT0_vect){               //start-stop

     if(btn==1){
	      TCCR0 |= 1<<WGM01| 1<<CS01;
		  btn=0;
	 }
	 else{
	     btn=1;
		 TCCR0 &=~(1<<CS01);
	 }
}	

ISR(INT1_vect){              //reset
	  minitsecond=0;
	  second=0;
	  min=0;
	  h=0;
	  TCCR0 &=~(1<<CS01);
} 

