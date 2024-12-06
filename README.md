# 8x8display
This is a project I did WAAAAYYYYY back when I was just a wee lad. Here's the original README:  

## 8x8display
This is just a little 8x8 LED display I threw together. It has an PIC12F629 microcontroller which drives two 74HC595 shift registers; one for driving the rows, one for driving the columns. The microcontrollers draws the display buffer by multiplexing the LEDs and showing one row at a time.

Currently, the microcontroller runs Conway's Game of Life and draws that to the display.