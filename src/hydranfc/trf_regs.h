
#ifndef _TRF_REGS_H_
#define _TRF_REGS_H_

// CHIP_STATE_CONTROL
// bit 7
#define CSC_STANDBY                              BIT7
#define CSC_ACTIVE                               0
// bit 6
#define CSC_DIRECT_MODE_0_1                      BIT6
#define CSC_DIRECT_MODE_2                        0
// bit 5
#define CSC_RF_ACTIVE                            BIT5
#define CSC_RF_NOT_ACTIVE                        0
// bit 4
#define CSC_HALF_POWER                           BIT4
#define CSC_FULL_POWER                           0
// bit 3
#define CSC_AUX_RX_INPUT                         BIT3
#define CSC_MAIN_RX_INPUT                        0
// bit 2
#define CSC_AGC_ON                               BIT2
#define CSC_AGC_OFF                              0
// bit 1
#define CSC_RECEIVER_ACTIVATED                   BIT1    // external field measurement
#define CSC_AUTOMATIC_ENABLE                     0
// bit 0
#define CSC_5V                                   BIT0
#define CSC_3V                                   0

// ISO_CONTROL
// bit 7
#define IC_RX_CRC                                BIT7
#define IC_NO_RX_CRC                             0
// bit 6
#define IC_DIRECT_MODE_0                         0
#define IC_DIRECT_MODE_1                         BIT6
// bit 5
#define IC_RFID_MODE                             0
#define IC_NFC_MODE                              BIT5
// bit 4
#define IC_NFC_TARGET                            0
#define IC_NFC_INITIATOR                         BIT4
// bit 3
#define IC_NFC_PASSIVE                           0
#define IC_NFC_ACTIVE                            BIT3
// bit 2
#define IC_NFC_NORMAL_MODE                       0
#define IC_NFC_CARD_EMULATION_MODE               BIT2
#define IC_CARD_EMULATION_MODE                   (IC_NFC_MODE | IC_NFC_CARD_EMULATION_MODE)

// RFID mode
#define IC_ISO15693_LOWRATE_1SUB_1OUTOF4         0
#define IC_ISO15693_LOWRATE_1SUB_1OUTOF256       BIT0
#define IC_ISO15693_HIGHRATE_1SUB_1OUTOF4        BIT1
#define IC_ISO15693_HIGHRATE_1SUB_1OUTOF256      (BIT1 | BIT0)
#define IC_ISO15693_LOWRATE_2SUB_1OUTOF4         BIT2
#define IC_ISO15693_LOWRATE_2SUB_1OUTOF256       (BIT2 | BIT0)
#define IC_ISO15693_HIGHRATE_2SUB_1OUTOF4        (BIT2 | BIT1)
#define IC_ISO15693_HIGHRATE_2SUB_1OUTOF256      (BIT2 | BIT1 | BIT0)
#define IC_ISO14443A_106_KBPS                    BIT3
#define IC_ISO14443A_212_KBPS                    (BIT3 | BIT0)
#define IC_ISO14443A_424_KBPS                    (BIT3 | BIT1)
#define IC_ISO14443A_848_KBPS                    (BIT3 | BIT1 | BIT0)
#define IC_ISO14443B_106_KBPS                    (BIT3 | BIT2)
#define IC_ISO14443B_212_KBPS                    (BIT3 | BIT2 | BIT0)
#define IC_ISO14443B_424_KBPS                    (BIT3 | BIT2 | BIT1)
#define IC_ISO14443B_848_KBPS                    (BIT3 | BIT2 | BIT1 | BIT0)
#define IC_FELICA_212_KBPS                       (BIT4 | BIT3 | BIT1)
#define IC_FELICA_424_KBPS                       (BIT4 | BIT3 | BIT1 | BIT0)

// NFC mode
#define IC_EMULATION_ISO14443A                   0
#define IC_EMULATION_ISO14443B                   BIT0
#define IC_NFC_106_KBPS                          BIT0
#define IC_NFC_212_KBPS                          BIT1
#define IC_NFC_424_KBPS                          (BIT1 | BIT0)

// MODULATOR_CONTROL
// bit 7
#define MC_27MHZ                                 BIT7
// bit 6
#define MC_EXTERNAL_ASK_OOK                      BIT6    // external selection of ASK or OOK (by pin 12)
// bit 5 / 4
#define MC_SYS_CLK_DISABLED                      0
#define MC_SYS_CLK_3_39_MHZ                      BIT4
#define MC_SYS_CLK_6_78_MHZ                      BIT5
#define MC_SYS_CLK_13_56_MHZ                     (BIT4 | BIT5)
#define MC_SYS_CLK_6_78_MHZ_WITH_27MHZ           (MC_27MHZ | BIT4)
#define MC_SYS_CLK_13_56_MHZ_WITH_27MHZ          (MC_27MHZ | BIT5)
#define MC_SYS_CLK_27_12_MHZ_WITH_27MHZ          (MC_27MHZ | BIT5 | BIT4)
// bit 3
#define MC_ENABLE_ANALOG_OUTPUT                  BIT3    // enable pin 12 as analog output of subcarrier signal
// bit 2 / 1 / 0
#define MC_ASK_10                                0
#define MC_ASK_OOK                               BIT0
#define MC_ASK_7                                 BIT1
#define MC_ASK_8_5                               (BIT1 | BIT0)
#define MC_ASK_13                                BIT2
#define MC_ASK_16                                (BIT2 | BIT0)
#define MC_ASK_22                                (BIT2 | BIT1)
#define MC_ASK_30                                (BIT2 | BIT1 | BIT0)

// RX_SPECIAL_SETTINGS
// bit 7
#define RSS_BANDPASS_110_TO_570                  BIT7    // appropriate for 212 KHz subcarrier - FeliCa
// bit 6
#define RSS_BANDPASS_200_TO_900                  BIT6    // appropriate for 424 KHz subcarrier - ISO15693
// bit 5
#define RSS_BANDPASS_450_TO_1500                 BIT5    // appropriate for 848 KHz subcarrier - ISO14443A/B
// bit 4
#define RSS_BANDPASS_100_TO_1500                 BIT4    // appropriate for 848 kbps for high-bit-rate ISO14443
                                                         // gain reduced for 18 dB
// misc
#define RSS_BANDPASS_240_TO_1400                 0       // appropriate for ISO14443B, FeliCa and ISO14443A high
                                                         // rates 212 kbps and 424 kbps
// bit 3 / 2
#define RSS_GAIN_REDUCTION_0_DB                  0
#define RSS_GAIN_REDUCTION_5_DB                  BIT2
#define RSS_GAIN_REDUCTION_10_DB                 BIT3
#define RSS_GAIN_REDUCTION_15_DB                 (BIT3 | BIT2)
// bit 1
#define RSS_AGC_ACTIVATION_3X                    BIT1    // activation level to 3 times the digitizing level
#define RSS_AGC_ACTIVATION_5X                    0       // activation level to 5 times the digitizing level
// bit 0
#define RSS_AGC_NO_LIMIT                         BIT0    // AGC not limited in time
                                                         // otherwise limited by 8 subcarrier pulses

// REGULATOR_CONTROL
// bit 7
#define RC_AUTO                                  BIT7
// bit 6
#define RC_EXTERNAL_AMPLIFIER                    BIT6
// bit 5
#define RC_IO_LOW_VOLTAGE                        BIT5
// bit 4 / 3 reserved
// bit 2 / 1 / 0 - 5V system
#define RC_5_V                                   (BIT2 | BIT1 | BIT0)
#define RC_4_9_V                                 (BIT2 | BIT1)
#define RC_4_8_V                                 (BIT2 | BIT0)
#define RC_4_7_V                                 BIT2
#define RC_4_6_V                                 (BIT1 | BIT0)
#define RC_4_5_V                                 BIT1
#define RC_4_4_V                                 BIT0
#define RC_4_3_V                                 0
// bit 2 / 1 / 0 - 3V system
#define RC_3_4_V                                 (BIT2 | BIT1 | BIT0)
#define RC_3_3_V                                 (BIT2 | BIT1)
#define RC_3_2_V                                 (BIT2 | BIT0)
#define RC_3_1_V                                 BIT2
#define RC_3_0_V                                 (BIT1 | BIT0)
#define RC_2_9_V                                 BIT1
#define RC_2_8_V                                 BIT0
#define RC_2_7_V                                 0

// IRQ_STATUS
#define IRQ_TX                                   BIT7
#define IRQ_RX                                   BIT6
#define IRQ_FIFO                                 BIT5
#define IRQ_CRC_ERROR                            BIT4
#define IRQ_PARITY_ERROR                         BIT3
#define IRQ_FRAMING_ERROR                        BIT2
#define IRQ_COLLISION_ERROR                      BIT1
#define IRQ_NO_RESPONSE_TIMEOUT                  BIT0

// TEST_SETTINGS_1
#define TS1_SUBCARRIER_INPUT                     BIT7    // OOK pin becomes decoder digital input
#define TS1_SUBCARRIER_OUTPUT                    BIT6    // MOD pin becomes receiver digitized subcarrier output
#define TS1_MOD_DIRECT                           BIT5    // MOD pin input for TX modulation control
#define TS1_FIRST_STAGE_OUTPUT                   BIT4    // second stage output used for analog out and digit.
#define TS1_LOW2                                 BIT3    // second stage gain -6 dB, HP corner frequency / 2
#define TS1_LOW1                                 BIT2    // first stage gain -6 dB, HP corner frequency / 2
#define TS1_INPUT_FOLLOWERS_TEST                 BIT1
#define TS1_AGC_TEST                             BIT0    // AGC leve seen on RSSI_210 bits

// create byte for command
#define MAKE_COMMAND(v)            (0x9F & (0x80 | (v)))

// create byte for single write
#define MAKE_WRITE(v)              (0x1F & (v))

// create byte for write continued
#define MAKE_WRITE_CONT(v)         (0x3F & (0x20 | (v)))

// create byte for single read
#define MAKE_READ(v)               (0x5F & (0x40 | (v)))

// create byte for read continued
#define MAKE_READ_CONT(v)          (0x7F & (0x60 | (v)))

#endif /* _TRF_REGS_H_ */

