# RattusExMachina

# Outline

8-32 channel AWG to replay recorded tetrode data.
- at least 12 bit, better 16
- at least 20kHz, configurable sampling rate
- at least 8 channels, better 16, ideally 32
- using full INTAN RHD2000 series input range (+-5mV)
- compatile with open ephys and closed-loop system

## Plan

[X] Check if teensy or Arduino Due easier to use for hi-speed SPI/DMA
[X] Benchmark serial data transfer speeds
[X] Benchmark serial data transfer speeds with SPI to single DAC
[X] Expand to 4/8 DACs
[X] Modify benchmark tool to generate visible data on DAC
[X] Transfer from python
[X] Transfer numpy array from python
[X] Transfer neural data from python
[ ] Pre-condition data (compression or configuration bits)
[ ] Attenuate output to INTAN RHD2000 input range
[ ] Filter power supply, VREF, use analog ground
[ ] Filter output
