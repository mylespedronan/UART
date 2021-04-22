# UART
This repository is an example for initializing UART and UART interrupt handler on the TM4C123GH6PM.

Project Components:
  - ESP32 DevKit v1  (x1)
  - TM4C123GH6PM uC  (x1)
  - Wire             (x1)

1. Connect wire from TX pin of UART1 of ESP32 (D4) to the RX pin of UART1 of TM4C123 (PB0).
2. Using the terminal/PuTTY/WebREPL, import UART, set the baudrate of 115200, and write to UART1 with a character
    - from machine import UART
    - uart = UART(1, baudrate=115200)
    - uart.write('r') - turns on red LED of TM4C123
    - uart.write('g') - turns on green LED of TM4C123
    - uart.write('b') - turns on blue LED of TM4C123
    - uart.write('[any other character]') - turns off all LEDs
