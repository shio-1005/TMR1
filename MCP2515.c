/*************************************************************************
 * File         :spi.c                       				 
 * Date         :2020/8/4					 
 * Summary      :Use for spi communication			
 * CPU type     :PIC16F886						 
 * Compiler     :XC8							 
 * Author       :Akaru Oishi						 
 * Description  :		 
 *              :				 
 *              :		 
 *              :			 
 * ***********************************************************************

/*---File Include---*/
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <spi.h>
#include <MCP2515.h>

char strage[_CANRX_STRAGE_SIZE];
char id_strage[16];

/*---Initiation---*/
void MCP2515_init(char bps)
{                                                                               //コンフィギュレーションモードに設定
    Write(_CANCTRL , 0b10001000);                                               //
    Write(_BFPCTRL , 0b00000000);                                               //RXnBFÉsÉìêßå‰Ç∆èÛë‘èâä˙âª
    Write(_TXB0CTRL , 0b00000000);                                              //ëóêMÉoÉbÉtÉ@0êßå‰ÉåÉWÉXÉ^èâä˙âª
    Write(_RXB0CTRL , 0b00000000);                                              //éÛêMÉoÉbÉtÉ@0êßå‰ÉåÉWÉXÉ^èâä˙âª
    Write(_RXB1CTRL , 0b00000000);                                              //éÛêMÉoÉbÉtÉ@1êßå‰ÉåÉWÉXÉ^èâä˙âª
    Write(_CANINTE , 0b00000001);                                               //受信割り込み許可
    Write(_CANINTF , 0b00000000);                                               //äÑÇËçûÇ›ÉtÉâÉOèâä˙âª
    Write(_EFLG , 0b00000000);                                                  //ÉGÉâÅ[ÉtÉâÉOèâä˙âª
    
    if(bps == 0)                                                                //CNF1,2,3:ビットタイミング設定レジスタ
    {
        Bit_Modify(_CNF1 , 0b00111111 , 0b00000111);                            //BRP=7
        Bit_Modify(_CNF2 , 0b11111111 , 0b10110001);                            //BLTMODE=1ÅiPS2ÇÃí∑Ç≥ÇÕCLF3Ç…ÇƒÅjÅASAM=0,PHSEG1=6,PRGSEG=1
        Bit_Modify(_CNF3 , 0b11111111 , 0b00000101);                            //PHSEG2=2
    }
    if(bps == 1)
    {
        Bit_Modify(_CNF1 , 0b00111111 , 0b00000001);                            //BRP=1
        Bit_Modify(_CNF2 , 0b11111111 , 0b10110001);                            //BLTMODE=1ÅiPS2ÇÃí∑Ç≥ÇÕCLF3Ç…ÇƒÅjÅASAM=0,PHSEG1=6,PRGSEG=1
        Bit_Modify(_CNF3 , 0b11111111 , 0b00000101);                            //PHSEG2=2
    }
    if(bps == 2)
    {
        Bit_Modify(_CNF1 , 0b00111111 , 0b00000000);                                //BRP=0
        Bit_Modify(_CNF2 , 0b11111111 , 0b10110001);                                //BLTMODE=1ÅiPS2ÇÃí∑Ç≥ÇÕCLF3Ç…ÇƒÅjÅASAM=0,PHSEG1=6,PRGSEG=1
        Bit_Modify(_CNF3 , 0b11111111 , 0b00000101);                                //PHSEG2=2
    }
    
}

void MCP2515_Open(char rxint)
{
    if(rxint == 1)
    {
        Bit_Modify(_CANCTRL , 0b11100000 , 0b00000000);                         //通常動作モードに設定
        INTCONbits.GIE = 1;                                                     //割り込み可能
        INTCONbits.PEIE = 1;
        INTCONbits.INTE = 1;                                                    //RB0/INT外部割り込み可能
        INTCONbits.INTF = 0;                                                    //RB0/INT外部割り込みフラグ = 0
        PIR1bits.TMR1IF = 0;                                                    //TMR1フラグ
        PIE1bits.TMR1IE = 1;                                                    //TMR1可能
        T1CONbits.T1CKPS1 = 0;                                                  //TMR1プリスケーラ
        T1CONbits.T1CKPS0 = 0;
        T1CONbits.TMR1CS = 0;                                                   //TMR1 内部クロック使用
        T1CONbits.TMR1ON = 0;                                                   //TMR1 OFF
        TRISBbits.TRISB0 = 1;                                                   //RB0 = 入力
        OPTION_REGbits.INTEDG = 0;                                              //RB0/INTピンの立ち下がりにより割り込み
    }
    else
    {
        Bit_Modify(_CANCTRL , 0b11100000 , 0b00000000);                         //通常モードに設定
        INTCONbits.INTE = 0;
    }
};

void Strage_Crea()
{
    memset(strage,'\0',_CANRX_STRAGE_SIZE);
}

/*---Communication---*/
void Write(char address , char value)
{
    PORTAbits.RA5 = 0;
    spi_putc(0b00000010);                                                       //SPI命令：書き込み
    spi_putc(address);
    spi_putc(value);
    PORTAbits.RA5 = 1;
}

char Read(char address)
{
    char data;
    
    PORTAbits.RA5 = 0;
    spi_putc(0b00000011);                                                       //SPI命令：読み込み
    spi_putc(address);
    data = spi_putc(0b00000000);
    PORTAbits.RA5 = 1;
    return data;
}

char Read_Status(void)
{
    char data;
    
    PORTAbits.RA5 = 0;
    spi_putc(0b10100000);                                                       //SPI命令:状態読み込み
    spi_putc(0b11111111);
    data = spi_putc(0b11111111);
    PORTAbits.RA5 = 1;
    return data;
}

char RX_Status(void)
{
    char data;
    
    PORTAbits.RA5 = 0;
    spi_putc(0b10110000);                                                       //SPI命令：RX状態読み込み
    spi_putc(0b11111111);
    data = spi_putc(0b11111111);
    PORTAbits.RA5 = 1;
    return data;
}

void Bit_Modify(char address , char mask , char value)                          //mask:レジスタのどのビットを変更するか
{
    PORTAbits.RA5 = 0;
    spi_putc(0b00000101);                                                       //SPI命令：ビット変更
    spi_putc(address);
    spi_putc(mask);
    spi_putc(value);
    PORTAbits.RA5 = 1;
}

char* Read_RX_Buffer(char buffer , char bytes)
{
    PORTAbits.RA5 = 0;
    spi_putc(buffer);                                                           //SPI命令：RXバッファ読み込み　0b10010nm0
    for(char i = 0 ; i < bytes ; i++){
        strage[i] = spi_putc(0);
    }
    PORTAbits.RA5 = 1;
    
    return strage;
}

char* Read_RX_ID(char buffer , char bytes)
{
    PORTAbits.RA5 = 0;
    spi_putc(buffer);                                                           
    for(char i = 0 ; i < bytes ; i++){
        id_strage[i] = spi_putc(0);
    }
    PORTAbits.RA5 = 1;
    
    return id_strage;
}

void Load_TX_Data(char buffer , char bytes , char* p)                           //どのバッファに何バイトどこから書き込むか
{
    PORTAbits.RA5 = 0;
    spi_putc(buffer);                                                           //SPI命令：TXバッファ書き込み　0b01000abc
    for(char i = 0 ; i < bytes ; i++){
        spi_putc(*p);
        p++;
    }
    PORTAbits.RA5 = 1;
}

void Load_TX_ID(char buffer , char sid_h , char sid_l , char eid_8 , char eid_0)
{
    PORTAbits.RA5 = 0;
    spi_putc(buffer);
    spi_putc(sid_h);
    spi_putc(sid_l);
    spi_putc(eid_8);
    spi_putc(eid_0);
    PORTAbits.RA5 = 1;
}

void RTS0(char time)
{
    PORTAbits.RA5 = 0;
    spi_putc(0b10000001);                                                       //SPI命令：メッセージ送信要求(TXB0)
    PORTAbits.RA5 = 1;
    if(time == 0)
    {
        __delay_us(1500);
    }
    if(time == 1)
    {
        __delay_us(400);
    }
    if(time == 2){
        __delay_us(200);
    }
};

void RTS0_CSS(char time)
{
    PORTAbits.RA5 = 0;
    spi_putc(0b10000001);                                                       //SPI命令：メッセージ送信要求(TXB0)
    PORTAbits.RA5 = 1;
    if(time == 0)
    {
        __delay_us(1000);
    }
    if(time == 1)
    {
        __delay_us(240);
    }
    if(time == 2)
    {
        __delay_us(120);
    }
};

void RTS1(void);
void RTS2(void);

