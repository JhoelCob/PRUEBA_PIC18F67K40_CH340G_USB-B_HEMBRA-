#include <xc.h>
#include "config.h"

// Definiciones
#define _XTAL_FREQ 64000000UL // Frecuencia del oscilador (64 MHz)
#define BAUD_RATE 9600        // Tasa de baudios para UART

void UART_Init() {
    // Configuración de PPS para UART1
    // RC6 (PIN31) como TX1
    TRISCbits.TRISC6 = 0; // Configura RC6 como salida
    RC6PPS = 0x0C;        // Asigna RC6 al módulo TX1 (EUSART1 TX)

    // RC7 (PIN32) como RX1
    TRISCbits.TRISC7 = 1; // Configura RC7 como entrada
    RXPPS = 0x17;         // Asigna RC7 al módulo RX1 (EUSART1 RX)

    // Desbloquea PPS
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // Desbloquear

    // Configuración de EUSART1
    TX1STAbits.BRGH = 1;  // High Baud Rate Select bit
    BAUD1CONbits.BRG16 = 1; // 16-bit Baud Rate Generator

    // Calcula el valor SPBRG1 para 9600 baudios a 64MHz
    // Fosc = 64MHz, BRGH = 1, BRG16 = 1
    // SPBRG = ((_XTAL_FREQ / BAUD_RATE) / 4) - 1
    unsigned int brg_val = (unsigned int)((_XTAL_FREQ / (4UL * BAUD_RATE)) - 1);
    SP1BRGL = (unsigned char)(brg_val & 0xFF);     // Asigna la parte baja (LSB)
    SP1BRGH = (unsigned char)((brg_val >> 8) & 0xFF); // Asigna la parte alta (MSB)

    RC1STAbits.CREN = 1; // Habilita el receptor continuo
    TX1STAbits.TXEN = 1; // Habilita el transmisor
    RC1STAbits.SPEN = 1; // Habilita el puerto serie
}

void UART_WriteChar(char data) {
    while (!TX1STAbits.TRMT); // Espera hasta que el registro de transmisión esté vacío
    TX1REG = data;           // Envía el byte
}

void UART_WriteString(const char *s) {
    while (*s) {
        UART_WriteChar(*s++);
    }
}

void main(void) {
    UART_Init(); // Inicializa la comunicación UART

    while (1) {
        UART_WriteString("Hola desde PIC18F67K40!\r\n"); // Envía el mensaje
        
        // Retardo de 5 segundos usando un bucle con retardos más pequeños
        for (int i = 0; i < 50; i++) { 
            __delay_ms(100); // Espera 100 ms, repetido 50 veces = 5000 ms (5 segundos)
        }
    }
}
