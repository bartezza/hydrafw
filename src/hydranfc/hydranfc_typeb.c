/*
 * HydraBus/HydraNFC
 *
 * Copyright (C) 2016 Andrea Bartezzaghi <andrea.bartezzaghi@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ch.h"
#include "common.h"
#include "tokenline.h"
#include "hydranfc.h"
#include "trf797x.h"
#include "bsp_spi.h"
#include <string.h>
#include "hydranfc_typeb.h"
#include "trf_regs.h"
#include "microsd.h"
#include "ff.h"
#include <stdio.h>

extern volatile int irq_count;
extern volatile int irq;
extern volatile int irq_end_rx;

void hydranfc_scan_typeb(t_hydra_console *con)
{
	uint8_t data_buf[16];
	uint8_t data_buf2[16];
	uint8_t ret;
	uint8_t i;

	/* Clear data elements */
	//memset(data, 0, sizeof(t_hydranfc_scan_iso14443A));

	/* End Test delay */
	irq_count = 0;

	/* Test ISO14443-A/Mifare read UID */
	Trf797xInitialSettings();
	Trf797xResetFIFO();

	/*
	 * Write Modulator and SYS_CLK Control Register (0x09) (13.56Mhz SYS_CLK
	 * and default Clock 13.56Mhz + ASK 10)
	 */
	data_buf[0] = MODULATOR_CONTROL;
	data_buf[1] = 0x30;
	Trf797xWriteSingle(data_buf, 2);

	/*
	 * ISO Control: ISO14443B RX bit rate, 106 kbps, no RX CRC
	 */
	data_buf[0] = ISO_CONTROL;
	data_buf[1] = IC_RFID_MODE | IC_ISO14443B_106_KBPS | IC_RX_CRC;
	Trf797xWriteSingle(data_buf, 2);

	/*
		data_buf[0] = ISO_CONTROL;
		Trf797xReadSingle(data_buf, 1);
		if (data_buf[0] != 0x88)
			cprintf(con, "Error ISO Control Register read=0x%02lX (should be 0x88)\r\n",
				(uint32_t)data_buf[0]);
	*/

	/* Turn RF ON (Chip Status Control Register (0x00)) */
	Trf797xTurnRfOn();
	
	/* Send WUPB */
	data_buf[0] = 0x00;
	data_buf[1] = 0x00;
	data_buf[2] = BIT3 | BIT2; /* WUPB, N = 16 */
	ret = Trf797x_transceive_bytes(data_buf, 3, data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
	if (ret == 0) {
	    /* Retry... */
	    ret = Trf797x_transceive_bytes(data_buf, 3, data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
	}
	if (ret == 0) {
	    cprintf(con, "Tag not found\r\n");
    	/* Turn RF OFF (Chip Status Control Register (0x00)) */
    	//Trf797xTurnRfOff();
    	//return;
    } else {
	    cprintf(con, "Response to WUPB:");
	    for (i = 0; i < ret; ++i) {
	        cprintf(con, " %02X", data_buf2[i]);
	    }
	    cprintf(con, "\r\n");
	}
	
	data_buf[0] = 0x00;
	data_buf[1] = 0x00;
	data_buf[2] = BIT3 | BIT2; /* WUPB, N = 16 */
	ret = Trf797x_transceive_bytes(data_buf, 3, data_buf2, sizeof(data_buf2), 50, 0); /* 20ms timeout, CRC enabled */
	if (ret == 0) {
	    /* Retry... */
	    ret = Trf797x_transceive_bytes(data_buf, 3, data_buf2, sizeof(data_buf2), 50, 0); /* 20ms timeout, CRC enabled */
	}
	if (ret == 0) {
	    cprintf(con, "Tag not found\r\n");
    	/* Turn RF OFF (Chip Status Control Register (0x00)) */
    	//Trf797xTurnRfOff();
    	//return;
    } else {
	    cprintf(con, "Response to WUPB:");
	    for (i = 0; i < ret; ++i) {
	        cprintf(con, " %02X", data_buf2[i]);
	    }
	    cprintf(con, "\r\n");
	}

    // TODO

    // turn off RF field
    Trf797xTurnRfOff();
}
