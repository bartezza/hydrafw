
#ifndef _TRF_PINS_H_
#define _TRF_PINS_H_

// PC4, IO5
#define SDM_BITCLK          (palReadPad(GPIOC, 4))

// PC2, IO6_MISO
#define DM1_DATA_BIT        (palReadPad(GPIOC, 2))

// PC0, IO2 (3v3 or TX SDM EN)
#define SDM_EN_1                (palWritePad(GPIOC, 0, 1))
#define SDM_EN_0                (palWritePad(GPIOC, 0, 0))

#define SDM_TXENABLE_ON         SDM_EN_1
#define SDM_TXENABLE_OFF        SDM_EN_0

// PC5, IO3 (TX SDM DATA)
#define SDM_DATA_1              (palWritePad(GPIOC, 5, 1))
#define SDM_DATA_0              (palWritePad(GPIOC, 5, 0))

#define SDM_DATA_ON             SDM_DATA_1
#define SDM_DATA_OFF            SDM_DATA_0

#define MISO_DATA_SETUP // TODO
#define MISO_SPI_SETUP // TODO

#endif /* _TRF_PINS_H_ */

