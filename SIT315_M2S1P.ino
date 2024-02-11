#include <Arduino.h>

extern "C" {
    void setup();
    void loop();
}

const byte LED_PIN = 13;
const byte METER_PIN = A4;
volatile double timerFrequency;

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(METER_PIN, INPUT);
        
    Serial.begin(9600);

    // Initialize timer frequency with initial value
    timerFrequency = map(analogRead(METER_PIN), 0, 1023, 1, 5); // assuming potentiometer range is from 0 to 5
    startTimer(timerFrequency);
}

void loop()
{ 
    // Read the potentiometer to update timer frequency
    timerFrequency = map(analogRead(METER_PIN), 0, 1023, 1, 5); // assuming potentiometer range is from 0 to 5
    startTimer(timerFrequency);
    delay(100); // Delay for stability
}

void startTimer(double timerFreq){
    noInterrupts(); // disable all interrupts
    
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B

    // set compare match register to desired timer count:
    OCR1A = (int)((16000000 / (1024 * timerFreq)) - 1); // assuming 16MHz Arduino clock and prescaler 1024

    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);

    // Set CS12, CS10 bits for 1024 prescaler:
    TCCR1B |= (1 << CS12) | (1 << CS10);

    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);

    interrupts(); // enable all interrupts
}

ISR(TIMER1_COMPA_vect){
    digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
}