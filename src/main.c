#define _XTAL_FREQ 4000000


#pragma config FOSC = INTRCIO
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config BOREN = OFF
#pragma config CP = OFF
#pragma config CPD = OFF


#include <xc.h>


#define SER GP2
#define RCK GP4
#define SCK GP5


#define u8 unsigned char
#define s8 signed char


u8 grid_a[8];
u8 grid_b[8];

u8 *current = grid_a;
u8 *next = grid_b;


inline u8 get_cell(u8 *grid, u8 x, u8 y) {
    return (grid[y] & (1 << x)) != 0;
}

inline void set_cell(u8 *grid, u8 x, u8 y, u8 val) {
    if(val)     grid[y] |= 1 << x;
    else        grid[y] &= ~(1 << x);
}


inline void write_registers(u8 col, u8 row) {
    for(u8 i = 0; i < 8; i++) {
        SER = (col & (1 << i)) != 0;
        SCK = 1;
        SCK = 0;
    }
    
    for(u8 i = 0; i < 8; i++) {
        SER = (row & (1 << i)) != 0;
        SCK = 1;
        SCK = 0;
    }
    
    RCK = 1;
    RCK = 0;
}


void interrupt ivec() {
    if(T0IF && T0IE) {
        for(u8 y = 0; y < 8; y++) {
            write_registers(~(1 << y), current[7 - y]);
            __delay_us(125);
        }
        write_registers(0x00, 0x00);
        
        TMR0 = 0;
        T0IF = 0;
    }
}


void main() {
    TRISIO              = 0b11001011;
    PIE1                = 0b00000000;
    INTCON              = 0b10100000;
    OPTION_REG          = 0b10000011;
    
    current[0]          = 0b00100000;
    current[1]          = 0b10100000;
    current[2]          = 0b01100000;
    current[3]          = 0b00000000;
    current[4]          = 0b00000000;
    current[5]          = 0b00000000;
    current[6]          = 0b00000000;
    current[7]          = 0b00000000;

    while(1) {
        for(u8 x = 0; x < 8; x++) {
            for(u8 y = 0; y < 8; y++) {
                u8 n = 0;
                
                n += get_cell(current, (x + 7) % 8, (y + 7) % 8);
                n += get_cell(current,  x,          (y + 7) % 8);
                n += get_cell(current, (x + 9) % 8, (y + 7) % 8);
                n += get_cell(current, (x + 7) % 8,  y);
                n += get_cell(current, (x + 9) % 8,  y);
                n += get_cell(current, (x + 7) % 8, (y + 9) % 8);
                n += get_cell(current,  x,          (y + 9) % 8);
                n += get_cell(current, (x + 9) % 8, (y + 9) % 8);
                
                if(get_cell(current, x, y)) set_cell(next, x, y, n >= 2 && n <= 3);
                else                        set_cell(next, x, y, n == 3);
            }
        }
        
        T0IE = 0;
        u8 *temp = current;
        current = next;
        next = temp;
        T0IE = 1;
    }
}