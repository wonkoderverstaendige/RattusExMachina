#define USBSERIAL Serial      // Arduino Leonardo, Teensy, Fubarino
//#define USBSERIAL SerialUSB   // Arduino Due, Maple
#define BUFSIZE 16

#include <spi4teensy3.h>
#define N_MCPS 4
#define CFG_A (0 << 15) | (1 << 14) | (1<< 13) | (1 << 12) // config bits channel A
#define CFG_B (1 << 15) | (1 << 14) | (1<< 13) | (1 << 12) // config bits channel B

const int CS_pins[N_MCPS] = {10, 9, 20, 21};
int i = 0;
int j = 0;
uint16_t out;
char buf[BUFSIZE];

#include <TimerOne.h>

void setup() {
  USBSERIAL.begin(115200);
  USBSERIAL.setTimeout(0);
  
  // set up CS pins
  for (byte n=0; n<N_MCPS; n++) {
    pinMode (CS_pins[n], OUTPUT);
  }

  spi4teensy3::init();
  
  Timer1.initialize(50);
  Timer1.attachInterrupt(refresh);
}

void loop() {
}

void refresh() {
  // prevent being called by interrupt while already running
  //int t_start = micros();
  Timer1.detachInterrupt(); 
  int count=0;
  int n;
  
  // refill buffer, take as long as needed
  while (count < BUFSIZE) {
    n = USBSERIAL.readBytes(buf+count, BUFSIZE-count);
    count = count + n;
  }

  for (byte c=0; c<N_MCPS; c++) {
    write_channel_A(buf+c*4, CS_pins[c]);
  }
  for (byte c=0; c<N_MCPS; c++) {
    write_channel_B(buf+2+c*4, CS_pins[c]);
  }
  
//  if (micros() - t_start > 50) {
//    USBSERIAL.println("Took too long: ");
//    USBSERIAL.println(micros() - t_start);
//  }
  Timer1.attachInterrupt(refresh);
}

// CS needs to be HIGH at least 15 ns before the next transfer can occur. Instead of
// waiting, just start transferring to next chip

// channel (0 = DACA, 1 = DACB) // Vref input buffer (0 = unbuffered, 1 = buffered) // gain (1 = 1x, 0 = 2x)  // Output power down power down (0 = output buffer disabled) //  12 bits of data
void write_channel_A(char subbuf[], int pin) {
  digitalWriteFast(pin, LOW);
  char cfg = (1 << 7) | (1 << 6) | (1<< 5) | (1 << 4);
  spi4teensy3::send(cfg | (subbuf[1] & 0xF));
  spi4teensy3::send(subbuf[0]);
  digitalWriteFast(pin, HIGH);
}
void write_channel_B(char subbuf[], int pin) {
  digitalWriteFast(pin, LOW);
  char cfg = (0 << 7) | (1 << 6) | (1<< 5) | (1 << 4);
  spi4teensy3::send(cfg | (subbuf[1] & 0xF));
  spi4teensy3::send(subbuf[0]);
  digitalWriteFast(pin, HIGH);
}
//void write_values(char subbuf[], int pin1, int pin2) {

//  
//          
//  
//  // chip 1, channel A
//  out = CFG_A | ( valueA1 ); 
//  digitalWriteFast(pin1, LOW);
//  spi4teensy3::send(out >> 8);
//  spi4teensy3::send(out & 0xFF);
//  digitalWriteFast(pin1, HIGH);
//
//  // chip 2, channel A
//  out = CFG_A | ( valueA2 ); 
//  digitalWriteFast(pin2, LOW);
//  spi4teensy3::send(out >> 8);
//  spi4teensy3::send(out & 0xFF);
//  digitalWriteFast(pin2, HIGH); 
//
//  // chip 1, channel B
//  out = CFG_B | ( valueB1 ); 
//  digitalWriteFast(pin1, LOW);
//  spi4teensy3::send(out >> 8);
//  spi4teensy3::send(out & 0xFF);
//  digitalWriteFast(pin1, HIGH);
//
//  // chip 2, channel B
//  out = CFG_B | ( valueB2 ); 
//  digitalWriteFast(pin2, LOW);
//  spi4teensy3::send(out >> 8);
//  spi4teensy3::send(out & 0xFF);
//  digitalWriteFast(pin2, HIGH);   
//}
