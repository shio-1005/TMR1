#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <spi.h>
#include <MCP2515.h>
#include <fram.h>

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR21V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT   = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ 20000000
#define _CAN_BAUDRATE        2

#define RXF0SIDH 0b00000000                                                     //フィルタ設定
#define RXF0SIDL 0b00000000
#define RXF0EID8 0b00100000
//#define RXF0EID8 0b01000000
//#define RXF0EID8 0b01100000
#define RXF0EID0 0b00000000
#define RXM0SIDH 0b00000000                                                     //マスク設定
#define RXM0SIDL 0b00000000
#define RXM0EID8 0b00000000
#define RXM0EID0 0b00000000

//#define TXB1SIDH    0b00000000
//#define TXB1SIDL  

/*Prtotype*/
void CONFIG();
//void interrupt isr();

int i;

void main(void){
    
    CONFIG();
    __delay_ms(100);
    
    while(1){
        
        RC6 = 0;
        
        for(i=0; i<1; i++){                                                    //1sとちょっとのタイマー
            PIR1bits.TMR1IF = 0;                                                //TMR1 フラグクリア
            TMR1L = 0;                                                          //TMR1 ビット
            TMR1H = 0;
            T1CONbits.TMR1ON = 1;                                               //TMR1 ON
            //while(! PIR1bits.TMR1IF);                                           //TMR1のタイムアップ待ち
            while(! PIR1bits.TMR1IF);
            //PIR1bits.TMR1IF = 0;
    
        }
        
        RC6 = 1;
        __delay_ms(200);
        
    }
}

void CONFIG()
{
    OSCCON = 0b01101000;
    ANSEL  = 0b00000000;
    ANSELH = 0b00000000;
    TRISC  = 0b00000000;
    PORTC  = 0b00000000;
    
    spi_init();
    __delay_ms(100);
    
    MCP2515_init(_CAN_BAUDRATE);                                                //Ç∆ÇËÇ†Ç¶Ç∏ÅCìÆçÏÇµÇƒÇ¢ÇÈ2Ç…ÇµÇΩÅDóùâÇÕÇ‹Çæ
    Write(_TXB0DLC , 0b00001000);                                                //ÉÅÉbÉZÅ[ÉWÉTÉCÉY8byte
    Write(_RXF0SIDH , RXF0SIDH);                                                //フィルタ設定
    Write(_RXF0SIDL , RXF0SIDL);
    Write(_RXF0EID8 , RXF0EID8);
    Write(_RXF0EID0 , RXF0EID0);
    Write(_RXM0SIDH , RXM0SIDH);                                                //マスク設定
    Write(_RXM0SIDL , RXM0SIDL);
    Write(_RXM0EID8 , RXM0EID8);
    Write(_RXM0EID0 , RXM0EID0);
    MCP2515_Open(1);                                                            //割り込みあり(1)/なし(0)
}

/*
void interrupt isr(){
    
    
}
*/