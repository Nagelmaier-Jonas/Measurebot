#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"
#include "main.h"
#include "uart.h"

extern volatile uint16_t flags;
volatile uint8_t myChar;
volatile bool digitalStatus = false;

void sendCharacter(uint8_t charToSend);
void sendString(uint8_t *string);

void initBtn()
{
   PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
   PCICR |= (1 << PCIE0);
   PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);
   // Baudrate definieren: 9600 => 103 lt. Datenblatt
   UBRR0 = 103;

   // Senden und Interrupt-basiertes Empfangen aktivieren
   UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

   // Anzahl der Datenbits mit 8 definieren
   UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

void sendCharacter(uint8_t charToSend)
{
   while (!(UCSR0A & (1 << RXC0)))
      ;
   UDR0 = charToSend;
}

void sendString(uint8_t *string)
{
   // Iteration über string
   // sendChar(Element aus string)
   while (*string)
   {
      sendCharacter(*string);
      string++;
   }
}

bool getDigitalStatus()
{
   return digitalStatus;
}

ISR(PCINT0_vect)
{
   if (!(PINB & (1 << PB0)))
   {
      flags |= (1 << SET_PAUSE);
   }
   else if (!(PINB & (1 << PB1)))
   {
      flags |= (1 << UPDATE_INTERVAL);
   }
   else if (!(PINB & (1 << PB2)))
   {
      digitalStatus = !digitalStatus;
   }
}