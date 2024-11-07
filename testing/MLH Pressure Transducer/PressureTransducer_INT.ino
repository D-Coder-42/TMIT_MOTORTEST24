int adcVal;
volatile uint16_t overflowCount = 0; // Counter for timer overflows

// triggered whenever ADC conversion is complete
ISR(ADC_vect)
{
  adcVal = ADC;
  Serial.println(adcVal);

  ADCSRA |= (1<<ADSC);
}

void initADC() {
  // setting reference voltage and enabling ADC3
  ADMUX &= ~(1<<REFS1) & ~(1<<MUX3) & ~(1<<MUX2);
  ADMUX |= (1<<REFS0) | (1<<MUX1) | (1<<MUX0);

  // setting up ADC and prescaler
  ADCSRA &= ~(1<<ADPS2) & ~(1<<ADPS0);
  ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS0);
}

int main()
{
  Serial.begin(9600);

  initADC();
  sei();

  ADCSRA |= (1<<ADSC);
  while(1);
}
