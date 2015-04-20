#define NOP __asm__ __volatile__ ("nop\n\t")
#define USBSERIAL Serial      // Arduino Leonardo, Teensy, Fubarino
//#define USBSERIAL SerialUSB   // Arduino Due, Maple

#include <spi4teensy3.h>
// number of MCP4922s in use. Number of channels = 2*N_MCPS
#define N_MCPS 8
const int CS_pins[] = {10, 9, 20, 21, 19, 18, 17, 16}; // for easy switching to pretend 16 channels
// channel (0 = DACA, 1 = DACB) // Vref input buffer (0 = unbuffered, 1 = buffered) // gain (1 = 1x, 0 = 2x)  // Output power down power down (0 = output buffer disabled) //  12 bits of data
#define CFG_A (0 << 7) | (1 << 6) | (1<< 5) | (1 << 4) // config bits channel A
#define CFG_B (1 << 7) | (1 << 6) | (1<< 5) | (1 << 4) // config bits channel B

#define BUFSIZE 4*N_MCPS
char buf[BUFSIZE]; // Data buffer

#include <TimerOne.h>

void setup() {
  USBSERIAL.begin(115200);
  USBSERIAL.setTimeout(0);
  
  // set up CS pins
  for (byte n=0; n<N_MCPS; n++) {
    pinMode (CS_pins[n], OUTPUT);
  }

  spi4teensy3::init();
  
  Timer1.initialize(50); // 50 microseconds =~ 20KHz
  Timer1.attachInterrupt(refresh);
}

void loop() {
  // nothing happening here... yet!
}

void refresh() {
  // prevent being called by interrupt while already running
  Timer1.detachInterrupt(); 
  //int t_start = micros();

  // refill buffer, take as long as needed
  int count=0;
  int n;
  while (count < BUFSIZE) {
    n = USBSERIAL.readBytes(buf+count, BUFSIZE-count);
    count = count + n;
  }

  for (byte c=0; c<N_MCPS; c++) {
    write_dac(buf+c*4, CS_pins[c]);
  }
  
//  if (micros() - t_start > 50) {
//    USBSERIAL.println("Took too long: ");
//    USBSERIAL.println(micros() - t_start);
//  }
  Timer1.attachInterrupt(refresh);
}

// CS needs to be HIGH at least 15 ns before the next transfer can occur
void write_dac(char subbuf[], int pin) {
  digitalWriteFast(pin, LOW);
  spi4teensy3::send(CFG_A | subbuf[1]);  // TODO: the 0xF shouldn't be needed!
  spi4teensy3::send(subbuf[0]);
  digitalWriteFast(pin, HIGH);
  
  NOP; // delay by ~20 ns (on 48MHz Teensy);
  NOP; // one seems enough, but better be safe + allow overclocking
  
  digitalWriteFast(pin, LOW);
  spi4teensy3::send(CFG_B | subbuf[3]);  // TODO: the 0xF shouldn't be needed!
  spi4teensy3::send(subbuf[2]);
  digitalWriteFast(pin, HIGH);  
}

