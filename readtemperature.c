#define F_CPU 16000000UL
#define SCL_CLK 16000
#define BITRATE ((F_CPU / SCL_CLK) - 16) / (2)
#define BAUD 9600
#define Baud_data ((F_CPU / 16 / BAUD) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>

int i=0;
char critialTemp = 60;
unsigned char msg[] = "Alert! High Temperature!!";

int main(void)
{
    char data1;
    UBRR0H = (Baud_data >> 8);
    UBRR0L = (Baud_data);

    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 <<  UCSZ01)|(1 << UCSZ00);
    
    TWBR = BITRATE;
    DDRB = 0b00100000;
    while (1)
    {
        I2C_Start(0x9A);
        I2C_Write(0x02);
        I2C_Start(0x9B);
        data1 = I2C_read();
        I2C_Stop();
        if (data1 > critialTemp)
        {
            PORTB |= (1 << PB5);
            printMSg();
        }
        else
        {
            PORTB &= (~(1 << PB5));
        }
    }
}

uint8_t I2C_Start(char SLA_R_W) 
{
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); 
    while (!(TWCR & (1 << TWINT)));

    TWDR = SLA_R_W;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
}

char I2C_read() 
{
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    return TWDR; 
}

uint8_t I2C_Write(char data) 
{
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
}

void I2C_Stop() 
{
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
}

void printMSg()
{
    while (msg[i]!=0)
        {
            while (!( UCSR0A & (1<<UDRE0)));
            UDR0 = msg[i];
            i++;
        }
        while (!( UCSR0A & (1<<UDRE0)));
        UDR0 = '\n';
        while (!( UCSR0A & (1<<UDRE0)));
        UDR0 = '\r';
        i=0;
}