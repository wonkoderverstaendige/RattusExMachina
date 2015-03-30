/* MODIFIEd 

  MCP4922 DAC test with spi4teensy3 fast SPI library, Teensy 3.0
 
  mostly pinched from http://forum.pjrc.com/threads/24082-Teensy-3-mcp4921-SPI-Dac-anybody-tried
 
  will need the spi4teensy3 library (https://github.com/xxxajk/spi4teensy3) copying into the libraries folder
 
  at SPI_CLOCK_DIV2 (24MHz on standard Teensy 3.0)
    generates ramp wave with period of 13.8ms (72hz)
    clock period ~42ns
    also faster than the rated speed of the MCP4922 (20Mhz), but it seems to work.
    is updating the voltage out every 3.3us
    
  Teensy pin 10   - MCP pin 3 (SS - slave select)
  Teensy pin 11   - MCP pin 5 (MOSI)
  Teensy pin 13   - MCP pin 4 (SCK - clock)
  +3.3v           - MCP pin 1 (vdd), 13 (DACa vref) 
  GND             - MCP pin 8 (LDAC), 12 (Analogue ground ref)
 
 */
 
#include <spi4teensy3.h>
#define N_MCPS 4

const int CFG_A = (0 << 15) | (1 << 14) | (1<< 13) | (1 << 12);
const int CFG_B = (1 << 15) | (1 << 14) | (1<< 13) | (1 << 12);
const int CS_pins[N_MCPS] = {10, 9, 20, 21};
int i = 0;
int j = 0;
uint16_t out;

void setup() {
  // set the slaveSelectPins as an output:
  for (byte n=0; n<N_MCPS; n++) {
    pinMode (CS_pins[n], OUTPUT);
  }
  // initialize SPI:
  spi4teensy3::init();
}

void loop() {
  j = 4096 - (++i);
  write_values(i, j, i, j, CS_pins[0], CS_pins[1]);
  write_values(i, j, i, j, CS_pins[2], CS_pins[3]);
  if(i > 4095) {
    i = 0; 
  }
}

void write_values(int valueA1, int valueA2, int valueB1, int valueB2, int pin1, int pin2) {
  // CS needs to be HIGh at least 15 ns before the next transfer can occur. Instead of
  // waiting, just start transferring to next chip
  
  // channel (0 = DACA, 1 = DACB) // Vref input buffer (0 = unbuffered, 1 = buffered) // gain (1 = 1x, 0 = 2x)  // Output power down power down (0 = output buffer disabled) //  12 bits of data        
  
  // chip 1, channel A
  out = CFG_A | ( valueA1 ); 
  digitalWriteFast(pin1, LOW);
  spi4teensy3::send(out >> 8);
  spi4teensy3::send(out & 0xFF);
  digitalWriteFast(pin1, HIGH);

  // chip 2, channel A
  out = CFG_A | ( valueA2 ); 
  digitalWriteFast(pin2, LOW);
  spi4teensy3::send(out >> 8);
  spi4teensy3::send(out & 0xFF);
  digitalWriteFast(pin2, HIGH); 

  // chip 1, channel B
  out = CFG_B | ( valueB1 ); 
  digitalWriteFast(pin1, LOW);
  spi4teensy3::send(out >> 8);
  spi4teensy3::send(out & 0xFF);
  digitalWriteFast(pin1, HIGH);

  // chip 2, channel B
  out = CFG_B | ( valueB2 ); 
  digitalWriteFast(pin2, LOW);
  spi4teensy3::send(out >> 8);
  spi4teensy3::send(out & 0xFF);
  digitalWriteFast(pin2, HIGH);   
}

