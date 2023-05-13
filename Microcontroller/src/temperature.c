#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "math.h"

extern volatile uint16_t flags;
static volatile double adcValue = 0;

double AdcToTemp(double adcVal)
{
    double tempK = 0.00;
    double tempC = 0.00;
    tempK = log(10000.0 * ((1024.0 / adcVal - 1)));
    tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK)) * tempK);
    tempC = tempK - 273.15;
    return tempC;
}

char *getTemperature(char *buffer)
{
    char tempBuffer[8];
    sprintf(buffer, "Temp: %sC", dtostrf(AdcToTemp(adcValue), 4, 2, tempBuffer));
    return buffer;
}

void initAdc()
{
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); // Enable ADC, Interrupt, Auto Trigger, Prescaler 128
    ADCSRA |= (1 << ADSC);                                                                           // Start conversion
    ADMUX |= ((1 << MUX2) | (1 << MUX0));                                                            // Ausgang 5
}

ISR(ADC_vect)
{
    adcValue = ADCW;        // Read ADC value
    flags |= (1 << UPDATE); // Set flag
}