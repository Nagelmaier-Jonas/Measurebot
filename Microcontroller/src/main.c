#include "main.h"

#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "buttons.h"
#include "lcd.h"
#include "temperature.h"
#include "uart.h"
#include "eepromData.h"
#include "crc.h"
#include "checksum.h"
#include <avr/eeprom.h>
#include "w5100.h"

#define UART_BAUD_RATE 9600

char buffer[32];
char uartBuffer[32];
char usartBuffer[32];
bool usartStarted = false;
bool status;
uint8 spiRead;

volatile uint16_t flags = 0;

EepromData eepromData;

void init();
void initInterval();
void initTimer();
void executeCommand(char c);

int main(void)
{
   unsigned int c;

   init();

   while (1)
   {
      IINCHIP_WRITE('0A-69-46-EE-C6-A5', 'test');
      spiRead = IINCHIP_READ('0A-69-46-EE-C6-A5');

      char tmpBuffer[32];
      char *tBuffer = spiRead;
      uart_puts(tBuffer);
      uart_putc(' ');

#if 0
      uint16_t tempFlags = 0;
      uint16_t updatedFlags = 0;

      ATOMIC_BLOCK(ATOMIC_FORCEON)
      {
         tempFlags = flags;
      }

      if (tempFlags & (1 << UPDATE_INTERVAL))
      {

         eepromData.TransmitInterval++;

         if (eepromData.TransmitInterval > 4)
            eepromData.TransmitInterval = 0;

         updatedFlags |= (1 << UPDATE_INTERVAL);
         lcd_clrscr();
         sprintf(buffer, "Interval: %d", eepromData.TransmitInterval);
         lcd_puts(buffer);
         _delay_ms(1500);

         eepromData.HeaderLength = sizeof(EepromData);
         eepromData.crc = crc_8((uint8_t *)&eepromData, sizeof(eepromData) - 1);
         eeprom_update_block(&eepromData, 0, sizeof(eepromData));
      }

      if (tempFlags & (1 << SET_PAUSE))
      {

         if (tempFlags & (1 << PAUSED))
         {
            updatedFlags |= (1 << PAUSED);
            lcd_clrscr();
            lcd_puts("Hello!");
            _delay_ms(1000);
         }
         else
         {
            lcd_clrscr();
            lcd_puts("Good Bye!");
            updatedFlags |= (1 << PAUSED);
            _delay_ms(1000);
            lcd_clrscr();
         }

         updatedFlags |= (1 << SET_PAUSE);

         if (tempFlags & (1 << PAUSED))
         {
            ATOMIC_BLOCK(ATOMIC_FORCEON)
            {
               flags ^= updatedFlags;
            }
            continue;
         }
      }

      if (tempFlags & (1 << READ_BUFFER))
      {
         updatedFlags |= (1 << READ_BUFFER);
         lcd_clrscr();
         lcd_puts("Checking...");
         _delay_ms(1000);
         lcd_clrscr();
         lcd_puts(uartBuffer);
         _delay_ms(1000);
      }

      if (tempFlags & (1 << PAUSED))
      {
         ATOMIC_BLOCK(ATOMIC_FORCEON)
         {
            flags ^= updatedFlags;
         }
         continue;
      }

      if (tempFlags & (1 << UPDATE))
      {
         updatedFlags |= (1 << UPDATE);
         _delay_ms(100);
         lcd_clrscr();
         char tempBuffer[32];
         lcd_puts(getTemperature(tempBuffer));
      }
#endif

      c = uart_getc();

      if (c & UART_NO_DATA)
      {
      }
      else
      {
         char tmpCharacter = (char)(c & 0x00FF);
         if (tmpCharacter == START)
         {
            lcd_clrscr();
            lcd_puts("UART enabled \n");
            _delay_ms(300);
            usartStarted = true;
         }
         else if (tmpCharacter == END)
         {
            lcd_clrscr();
            lcd_puts("UART disabled \n");
            _delay_ms(1000);
         }

         if (usartStarted)
         {
            tmpCharacter = (char)(c & 0x00FF);
            strcat(uartBuffer, &tmpCharacter);
            lcd_clrscr();
            lcd_puts(uartBuffer);
            lcd_putc(c);
            _delay_ms(1000);
            executeCommand(tmpCharacter);
         }
      }

#if 0

      ATOMIC_BLOCK(ATOMIC_FORCEON)
      {
         flags ^= updatedFlags;
      }

#endif
   }
}

void init()
{
   uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
   lcd_init(LCD_DISP_ON);
   initAdc();
   initBtn();
   initTimer();
   initInterval();
   lcd_clrscr();
   lcd_puts("Init done!");
   sei();
}

void initTimer()
{
   // Timer mit Systemtakt aktivieren(16MHz)
   TCCR1B |= (1 << CS12) | (1 << CS10);
   // Timer overflow interrupt aktivieren
   TIMSK1 |= (1 << TOIE1);
}

void initInterval()
{
   EepromData buffer;
   eeprom_read_block(&buffer, 0, sizeof(EepromData));
   if (buffer.crc == crc_8((uint8_t *)&buffer, sizeof(EepromData) - 1))
   {
      eepromData = buffer;
   }
}

void executeCommand(char c)
{
   char tmpCharacter = (char)(c & 0x00FF);
   if (tmpCharacter == 'a')
   {
      char tmpBuffer[32];
      char *tBuffer = getTemperature(tmpBuffer);
      uart_puts(tBuffer);
      uart_putc(' ');

      bool status = getDigitalStatus();
      char statusBuffer[32];
      sprintf(statusBuffer, "%d", status);
      uart_puts(statusBuffer);
   }
   else if (tmpCharacter == 't')
   {
      char tmpBuffer[32];
      char *tBuffer = getTemperature(tmpBuffer);
      uart_puts(tBuffer);
      uart_putc(' ');
   }
   else if (tmpCharacter == 's')
   {
      bool status = getDigitalStatus();
      char statusBuffer[32];
      sprintf(statusBuffer, "%d", status);
      uart_puts(statusBuffer);
      uart_putc(' ');
   }
}

ISR(TIMER1_OVF_vect)
{
   TCNT1 = 2276 * eepromData.TransmitInterval;
   if (eepromData.TransmitInterval == 0)
   {
      return;
   }
   executeCommand('t');
}