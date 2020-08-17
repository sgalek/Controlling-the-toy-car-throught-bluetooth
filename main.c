#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#define FOSC 1843200// Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

static volatile int speed = 0;
static volatile int speedL = 0;
static volatile int speedP = 0;
static volatile int zmienna = 0;
static volatile int o=0;
static volatile int b=0;

void USART_Transmit( int x )
{
	unsigned char data=x;
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	UDR0 = data;
}

void USART_Transmit_String(char* stringToSend)
{
	while (*stringToSend)
	USART_Transmit(*stringToSend++);
}

void ADC_init()
{
	ADMUX = (1 << REFS0) | (1 << ADLAR);
	ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) ;
	DIDR0 = (1 << ADC0D);
	ADCSRA |= (1 << ADSC);
}

int main(void)
{
	//USART_Init
	UCSR0A |= (1 << U2X0);
	UBRR0 = 16;
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B |= (1 << TXEN0);
	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << RXCIE0);
	
	//PWM
	TCCR2A |= (1<<COM2A1) | (1<<COM2B1) | (1<<WGM21) | (1<<WGM20);
	TCCR2B |= (1<<CS22);
	 TCCR4A |= (1<<COM4A1) | (1<<COM4B1) | (1<<WGM41) | (1<<WGM40);
	 TCCR4B |=  (1<<CS40);

	//Swiatla i kierunki
	DDRJ=0x00; // czujniki
	DDRC=0x00;
	// OCA prawy
	DDRG|=(1<<PG5);
	DDRB|=(1<<PB6) | (1<<PB5);
	DDRE|=(1<<PE3);
	DDRE |= (1<<PE4) | (1<<PE5);
	DDRH|=(1<<PH3);
	DDRH |= (1<<PH6);
	DDRB |= (1<<PB4);
	//PORTB |= (1<<PB6); //prawy kierunek
	//PORTG |= (1<<PG5); //lewy kierunek
	//PORTE |= (1<<PE3); //œwiat³a stop
	//PORTH |= (1<<PH3); //œwiat³a przód
	ADC_init();
	sei();
	while (1)
	{
		if(o < 0)
		{
			while (1)
			{
				PORTG ^= (1<<PG5); //lewy kierunek
				_delay_ms(500);
				if(o >= 0)
				{
					PORTG &= ~(1<<PG5);
					break;
				}
				
			}
		}
		if (o > 0)
		{
			while (1)
			{
				PORTB ^= (1<<PB6); //prawy kierunek
				_delay_ms(500);
				if(o <= 0)
				{
					PORTB &= ~(1<<PB6);
					break;
				}
				
			}
		}
		if(o==0)
		{
			PORTB &= ~(1<<PB6);
			PORTG &= ~(1<<PG5);
		}
		
		if(b==1)
		{
			while(1)
			{
				if(b<1)break;
				if(o < 0)
				{
					while (1)
					{
						PORTG ^= (1<<PG5); //lewy kierunek
						PORTE ^= (1<<PE3); //œwiat³a stop
						PORTB ^= (1<<PB5);
						_delay_ms(500);
						if(o >= 0)
						{
							PORTG &= ~(1<<PG5);
							break;
						}
						
					}
				}
				if (o > 0)
				{
					while (1)
					{
						PORTB ^= (1<<PB6); //prawy kierunek
						PORTE ^= (1<<PE3); //œwiat³a stop
						PORTB ^= (1<<PB5);
						_delay_ms(500);
						if(o <= 0)
						{
							PORTB &= ~(1<<PB6);
							break;
						}
						
					}
				}
				if(o==0)
				{
					PORTB &= ~(1<<PB6);
					PORTG &= ~(1<<PG5);
				}
				PORTE ^= (1<<PE3); //œwiat³a stop
				PORTB ^= (1<<PB5);
				_delay_ms(500);
				if(b<1)
				{
					PORTE &= ~(1<<PE3); //œwiat³a stop
					PORTB &= ~(1<<PB5);
					break;
				}
			}
		}
		if(b<1)
		{
			PORTE &= ~(1<<PE3); //œwiat³a stop
			PORTB &= ~(1<<PB5);
		}
		if(zmienna == 0)
		{
			b=0;
		}
		if(bit_is_clear(PINJ,0))
		{
			speedL = 0;
			speedP = 0;
			OCR2A = speedP;
			OCR2B = speedL;
			PORTE |= (1<<PE3); //œwiat³a stop
			PORTB |= (1<<PB5);
			_delay_ms(2000);
			PORTE &= ~(1<<PE3); //œwiat³a stop
			PORTB &= ~(1<<PB5);
			PORTH |= (1<<PH6); // Gasienica prawa
			PORTB |= (1<<PB4); // Gasienica lewa
			PORTE &= ~(1<<PE4);
			PORTE &= ~(1<<PE5);
			speedL = 100;
			speedP = 100;
			OCR2A = speedP;
			OCR2B = speedL;
			_delay_ms(1000);
			zmienna = 0;
			speedP = 0;
			speedL = 0;
			OCR2A = speedP;
			OCR2B = speedL;
		}
		if(bit_is_clear(PINJ,1))
		{
			speedL = 0;
			speedP = 0;
			OCR2A = speedP;
			OCR2B = speedL;
			PORTE |= (1<<PE3); //œwiat³a stop
			PORTB |= (1<<PB5);
			_delay_ms(2000);
			PORTE &= ~(1<<PE3); //œwiat³a stop
			PORTB &= ~(1<<PB5);
			PORTH |= (1<<PH6); // Gasienica prawa
			PORTB |= (1<<PB4); // Gasienica lewa
			PORTE &= ~(1<<PE4);
			PORTE &= ~(1<<PE5);
			speedL = 100;
			speedP = 100;
			OCR2A = speedP;
			OCR2B = speedL;
			_delay_ms(1000);
			zmienna = 0;
			speedP = 0;
			speedL = 0;
			OCR2A = speedP;
			OCR2B = speedL;
		}
		if(bit_is_clear(PINC,1))
		{
			speedL = 0;
			speedP = 0;
			OCR2A = speedP;
			OCR2B = speedL;
			PORTE |= (1<<PE3); //œwiat³a stop
			PORTB |= (1<<PB5);
			_delay_ms(2000);
			PORTE &= ~(1<<PE3); //œwiat³a stop
			PORTB &= ~(1<<PB5);
			PORTH |= (1<<PH6); // Gasienica prawa
			PORTB |= (1<<PB4); // Gasienica lewa
			PORTE |= (1<<PE4);
			PORTE |= (1<<PE5);
			speedL = 100;
			speedP = 100;
			OCR2A = speedP;
			OCR2B = speedL;
			_delay_ms(1000);
			zmienna = 0;
			speedP = 0;
			speedL = 0;
			OCR2A = speedP;
			OCR2B = speedL;
		}
	}
}
ISR(ADC_vect)
{
	if(zmienna>0)
	{
		
		uint8_t sw = ADCH;
		OCR4A = sw;
	}else
	{
		OCR4A=0;
	}
	
}
ISR(USART0_RX_vect)
{
	char com = UDR0;
	switch(com)
	{
		case 'F':
		zmienna+=1;
		if(zmienna > 12)
		{
			speedP=255;
			speedL=255;
		}
		if(zmienna==0)
		{
			b=0;
			speedL=0;
			speedP=0;
		}
		PORTH |= (1<<PH6); // Gasienica prawa
		PORTB |= (1<<PB4); // Gasienica lewa
		//PORTE |= (1<<PE4) | (1<<PE5); // Kierunek
		if(zmienna>=0)
		{
			//PORTH |= (1<<PH3);
			
			
			
			
			
			
			b=0;
			PORTE |= (1<<PE4) | (1<<PE5);
			speedL+=20;
			speedP+=20;
			PORTH |= (1<<PH3); // Swiatlo przod
			if(zmienna > 12)
			{
				speedP=255;
				speedL=255;
			}
		}
		else
		{
			b=1;
			PORTE &= ~(1<<PE4);
			PORTE &= ~(1<<PE5);
			speedL-=20;
			speedP-=20;
			if(speedP<=0)
			{
				speedP=0;
			}
			if(speedL<=0)
			{
				speedL=0;
			}
		}
		OCR2A = speedP;
		OCR2B = speedL;
		break;
		case 'R':
		zmienna-=1;
		if(zmienna < -12)
		{
			speedL=255;
			speedP=255;
		}
		if(zmienna==0)
		{
			
			speedP=0;
			speedL=0;
		}
		PORTH |= (1<<PH6); // Gasienica prawa
		PORTB |= (1<<PB4); // Gasienica lewa
		PORTE |= (1<<PE3);
		if(zmienna<0)
		{
			b=1;
			PORTE &= ~(1<<PE4);
			PORTE &= ~(1<<PE5);
			PORTH &= ~(1<<PH3);
			speedL+=20;
			speedP+=20;
			if(zmienna < -12)
			{
				speedL=255;
				speedP=255;
			}
		}
		else
		{
			PORTE |= (1<<PE4) | (1<<PE5);
			speedL-=20;
			speedP-=20;
			if(speedL<=0)
			{
				speedL=0;
			}
			if(speedP<=0)
			{
				speedP=0;
			}
		}
		OCR2A = speedP;
		OCR2B = speedL;
		break;
		case 'P':
		
		if (speedL < speedP)
		{
			o=0;
			speedL+=20;
			OCR2B = speedL;
		}else
		{
			o+=1;
			speedP -= 20;
			if(speedP<=0)
			{
				speedP=0;
			}
			OCR2A = speedP;
		}
		break;
		case 'L':
		
		if (speedP < speedL)
		{
			o=0;
			speedP+=20;
			OCR2A = speedP;
		}else
		{
			o-=1;
			speedL -= 20;
			if(speedL<=0)
			{
				speedL=0;
			}
			OCR2B = speedL;
		}
		
		
		break;
		case 'S':
		b=0;
		o=0;
		PORTH &= ~(1<<PH3); // Swiatlo przod
		speedL = 0;
		speedP = 0;
		zmienna = 0;
		OCR2A = speedP;
		OCR2B = speedL;
		PORTE |= (1<<PE3); //œwiat³a stop
		PORTB |= (1<<PB5);
		_delay_ms(2000);
		PORTE &= ~(1<<PE3); //œwiat³a stop
		PORTB &= ~(1<<PB5);
		default:
		break;
	}
	UDR0=com;
}





