#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_DIRECT_MODE__)
#pragma message "[DIRECT MODE]"
#elif(__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_INDIRECT_MODE__) /* INDIRECT MODE I/F */
#pragma message "[INDIRECT MODE]"
#elif (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_SPI_MODE__)
#pragma message "[SPI MODE]"
#else
  #error "unknown bus type"
#endif

#if defined (__AVR_ATmega2560__)
#pragma message "[SPI - ATmega2560]"
// PB3(MISO), PB2(MOSI), PB1(SCK), PB0(/SS)         // CS=1, waiting for SPI start // SPI mode 0, 4MHz
// ATMEGA 2560 Configuration!!!!
// Arbitrary Slave Select
#define SS_CUSTOM_BIT           (PORTB4)
#define SS_CUSTOM_DDR           (DDRB)
#define SS_CUSTOM_PORT          (PORTB)

// Hardware Slave Select
// Set although not needed => for correct SPI operation!
#define SPI0_SS_BIT          (PORTB0)
#define SPI0_SS_DDR          (PORTB)
#define SPI0_SS_PORT         (DDRB)

// Serial clock
#define SPI0_SCLK_BIT         (PORTB1)
#define SPI0_SCLK_DDR         (DDRB)
#define SPI0_SCLK_PORT        (PORTB)

//Master Out Slave In
#define SPI0_MOSI_BIT         (PORTB2)
#define SPI0_MOSI_DDR         (DDRB)
#define SPI0_MOSI_PORT        (PORTB)

//Master In Slave Out
#define	SPI0_MISO_BIT         (PORTB3)
#define SPI0_MISO_DDR         (DDRB)
#define SPI0_MISO_PORT        (PORTB)
#elif defined (__AVR_ATmega328P__)
#pragma message "[SPI - ATmega328p]"
// ARDUINO UNO CONFIG:
// MOSI (PB3)
// MISO (PB4)
// SS   (PB2) => Hardware ChipSelect, must be set to output, otherwise SPI doesn't work (-> Dummy!)
// SCK  (PB5)
#define SPI0_MOSI_BIT    PORTB3
#define SPI0_MOSI_DDR    DDRB
#define SPI0_MOSI_PORT   PORTB

#define MISO_BIT_HAL    PORTB4
#define MISO_DDR_HAL    DDRB
#define MISO_PORT_HAL   PORTB

// Hardware Slave Select
// Set although not needed => for correct SPI operation!
#define SPI0_SS_BIT    PORTB2
#define SPI0_SS_DDR    DDRB
#define SPI0_SS_PORT   PORTB

// Serial clock
#define SPI0_SCLK_BIT    PORTB5
#define SPI0_SCLK_DDR    DDRB
#define SPI0_SCLK_PORT   PORTB

// Slave Select for Ethernetshield (ATmega328p) is the HW Slave select!!!!!
#define SS_CUSTOM_BIT  PORTB2
#define SS_CUSTOM_DDR  DDRB
#define SS_CUSTOM_PORT PORTB

#else
  #error "[SPI - unknown microcontroller]"
#endif



#define SPI0_WaitForReceive()
#define SPI0_RxData()         (SPDR)

#define SPI0_TxData(Data)     (SPDR = Data)
#define SPI0_WaitForSend()    while( (SPSR & 0x80)==0x00 )

#define SPI0_SendByte(Data)   SPI0_TxData(Data);SPI0_WaitForSend()
#define SPI0_RecvByte()       SPI0_RxData()


// need to set AVR-SS Bit to set Output (PB0) => YESS!!!!
#define SPI0_Init()    SPI0_SS_DDR |= (1<<SPI0_SS_BIT);\
                       SPI0_SS_PORT |=(1<<SPI0_SS_BIT);\
                       SPI0_SCLK_DDR |= (1<<SPI0_SCLK_BIT);\
                       SPI0_MOSI_DDR |= (1<<SPI0_MOSI_BIT);\
                       SS_CUSTOM_DDR |= (1<<SS_CUSTOM_BIT);\
                       SPCR  = (1<<MSTR) | (1<<SPE);\
                       SPSR  = (1<<SPI2X)

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//IInChip SPI HAL
#define IINCHIP_SpiInit          SPI0_Init
#define IINCHIP_SpiSendData      SPI0_SendByte
#define IINCHIP_SpiRecvData      SPI0_RxData

#define IINCHIP_CSon()          (SS_CUSTOM_PORT |= (1<<SS_CUSTOM_BIT))
#define IINCHIP_CSoff()         (SS_CUSTOM_PORT &= ~(1<<SS_CUSTOM_BIT))
//-----------------------------------------------------------------------------
