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

static unsigned char g_srix_buffer[128 * 4];
static char g_srix_blockStatus[128];
static unsigned char g_srix_uid[8];

int hydranfc_srix_dump_to_file(t_hydra_console *con, const char *filename)
{
	FRESULT err;
	FIL fp;
	uint32_t i;

	if (filename) {
		// mount
		if (is_fs_ready() == FALSE) {
			if (mount() != 0) {
				cprintf(con, "Error mounting FS\r\n");
				return -5;
			}
		} else {
			umount();
			if (mount() != 0) {
				return -5;
			}
		}

		/* Save data in file */
	#if 0
		char filename[20];
		for (i = 0; i < 999; ++i) {
			sprintf(filename, "0:typeb_%ld.txt", i);
			err = f_open(&fp, filename, FA_WRITE | FA_CREATE_NEW);
			if (err == FR_OK) {
				break;
			}
		}
	#else
		err = f_open(&fp, filename, FA_WRITE | FA_CREATE_NEW);
	#endif
		if (err != FR_OK) {
			cprintf(con, "Error opening file\r\n");
			umount();
			return -2;
		}
	}

	// read all blocks
	uint8_t data_buf[16];
	uint8_t data_buf2[16];
	uint8_t j, ret;
	for (j = 0; j < 128; ++j) {
		// read block
		data_buf[0] = 0x08;
		data_buf[1] = j;
		ret = Trf797x_transceive_bytes(data_buf, 2, data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
		if (ret == 0) {
			cprintf(con, "No resp reading block %u\r\n", j);
			if (filename)
				f_printf(&fp, "No resp reading block %u\r\n", j);
		} else {
			cprintf(con, "[%02X]", j);
			if (filename)
				f_printf(&fp, "[%02X]", j);
			for (i = 0; i < ret; ++i) {
				cprintf(con, " %02X", data_buf2[i]);
				if (filename)
					f_printf(&fp, " %02X", data_buf2[i]);
			}
			cprintf(con, "\r\n");
			if (filename)
				f_printf(&fp, "\r\n");
		}
	}
	
	// read system area
	data_buf[0] = 0x08;
	data_buf[1] = 0xFF;
	ret = Trf797x_transceive_bytes(data_buf, 2, data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
	if (ret == 0) {
		cprintf(con, "No resp reading system area\r\n");
		if (filename)
			f_printf(&fp, "No resp reading system area\r\n");
	} else {
		cprintf(con, "[%02X]", 0xFF);
		if (filename)
			f_printf(&fp, "[%02X]", 0xFF);
		for (i = 0; i < ret; ++i) {
			cprintf(con, " %02X", data_buf2[i]);
			if (filename)
				f_printf(&fp, " %02X", data_buf2[i]);
		}
		cprintf(con, "\r\n");
		if (filename)
			f_printf(&fp, "\r\n");
	}

	if (filename) {
		// close file
		f_close(&fp);
		// umount
		umount();
	}
	
	/*filename_t lastFilename;
	write_file_get_last_filename(&lastFilename);
	cprintf(con, "Tag dumped to file '%s'\r\n", &lastFilename.filename[2]);*/
	return 0;
}

int hydranfc_srix_read_from_file(t_hydra_console *con, const char *filename, unsigned char *outBuff, char *blockStatus)
{
	FRESULT err;
	// mount
	if (!is_fs_ready()) {
		err = mount();
		if (err) {
			cprintf(con, "[Error] Mount failed (error %d)\r\n", err);
			return -1;
		}
	}
	// open file
	FIL fp;
	err = f_open(&fp, filename, FA_READ | FA_OPEN_EXISTING);
	if (err != FR_OK) {
		cprintf(con, "[Error] Could not open file '%s'\r\n", filename);
		umount();
		return -1;
	}

	char line[32];
	uint8_t cur = 0;
	uint32_t bytesRead = 0, ptr = 0, t0, t1, t2, t3, t4;
	int ret = 0;
	while (1) {
		// read byte
		err = f_read(&fp, &line[cur], 1, (void *)&bytesRead);
		if (err != FR_OK) {
			cprintf(con, "[Error] Could not read byte\r\n");
			f_close(&fp);
			umount();
			return -1;
		}
		// check it
		if (line[cur] == '\r' || line[cur] == '\n' || cur >= (sizeof(line) - 1)) { // TODO: check better for line too long
			if (cur > 0) {
				// scan line
				line[cur] = '\0';
				if (sscanf(line, "%lx %lx %lx %lx %lx", &t0, &t1, &t2, &t3, &t4) != 5) {
					cprintf(con, "[Error] Could not read block %u (str = '%s')\r\n", ptr, line);
					// fill block anyway
					outBuff[ptr * 4] = 0xAA;
					outBuff[ptr * 4 + 1] = 0xAA;
					outBuff[ptr * 4 + 2] = 0xAA;
					outBuff[ptr * 4 + 3] = 0xAA;
					blockStatus[ptr] = 0;
					ret = -2;
				} else {
					// fill block
					outBuff[ptr * 4] = t1;
					outBuff[ptr * 4 + 1] = t2;
					outBuff[ptr * 4 + 2] = t3;
					outBuff[ptr * 4 + 3] = t4;
					// check block
					if (t0 != ptr) {
						cprintf(con, "[Error] Expected block %u, read block %u\r\n", ptr, t0);
						blockStatus[ptr] = 0;
						ret = -2;
					} else {
						blockStatus[ptr] = 1;
					}
				}
				// advance block
				++ptr;
				cprintf(con, ".");
				// reset cur pointer
				cur = 0;
			}
		} else if (line[cur] != '[' && line[cur] != ']') { // skip parenthesis
			// advance cur pointer
			++cur;
		}
		// check if we finished reading
		if (ptr >= 128)
			break;
	}

	//sscanf(test, "%u", &asd);

	//err = f_scanf(&fp, "%s", test);

	//err = f_read(&fp, mf_ultralight_data, cnt, (void *)&cnt);

	// close file
	f_close(&fp);
	umount();
	return ret;
}

int hydranfc_srix_initiate_tag(t_hydra_console *con)
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
	data_buf[1] = IC_ISO14443B_106_KBPS | IC_RX_CRC;
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

	// NOTE: the SRIX4k do not respond to standard ISO 14443B commands (like WUPB, etc.)
	
	// initiate command
	data_buf[0] = 0x06;
	data_buf[1] = 0x00;
	ret = Trf797x_transceive_bytes(data_buf, 2, data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
	if (ret == 0) {
		cprintf(con, "No resp to initiate\r\n");
		return -1;
	} else {
		cprintf(con, "Resp to initiate: ");
		for (i = 0; i < ret; ++i) {
			cprintf(con, " %02X", data_buf2[i]);
		}
		cprintf(con, "\r\n");
	}

#if 0
	data_buf[0] = 0x06;
	data_buf[1] = 0x00;
	ret = Trf797x_transceive_bytes(data_buf, 2, data_buf2, sizeof(data_buf2), 50, 0); /* 20ms timeout, CRC enabled */
	if (ret == 0) {
		cprintf(con, "No resp to initiate (no CRC)\r\n");
		return -1;
	} else {
		cprintf(con, "Resp to initiate (no CRC): ");
		for (i = 0; i < ret; ++i) {
			cprintf(con, " %02X", data_buf2[i]);
		}
		cprintf(con, "\r\n");
	}*/
#endif

	// select command
	uint8_t chipId = data_buf2[0];
	data_buf[0] = 0x0E;
	data_buf[1] = chipId;
	ret = Trf797x_transceive_bytes(data_buf, 2, data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
	if (ret == 0) {
		cprintf(con, "No resp to select\r\n");
		return -1;
	} else {
		cprintf(con, "Resp to select: ");
		for (i = 0; i < ret; ++i) {
			cprintf(con, " %02X", data_buf2[i]);
		}
		cprintf(con, "\r\n");
	}
	
	// get UID command
	data_buf[0] = 0x0B;
	ret = Trf797x_transceive_bytes(data_buf, 1, data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
	if (ret == 0) {
		cprintf(con, "No resp to get UID\r\n");
		return -1;
	} else {
		cprintf(con, "Resp to get UID: ");
		for (i = 0; i < ret; ++i) {
			cprintf(con, " %02X", data_buf2[i]);
			g_srix_uid[i] = data_buf2[i];
		}
		cprintf(con, "\r\n");
	}
	return 0;
}

int hydranfc_srix_write_block(t_hydra_console *con, uint8_t block, const uint8_t *values)
{
	// safety check!
#if 1
	if (block < 7 || block > 127) {
		cprintf(con, "[Error] Writing to block 0x%02X can be dangerous!\r\n", block);
		return -1;
	}
#endif
	// write block
	uint8_t data_buf[] = { 0x09, block, values[0], values[1], values[2], values[3] };
	uint8_t data_buf2[16];
	uint8_t ret;
	ret = Trf797x_transceive_bytes(data_buf, sizeof(data_buf), data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
	// read block, to check
	data_buf[0] = 0x08;
	data_buf[1] = block;
	ret = Trf797x_transceive_bytes(data_buf, 2, data_buf2, sizeof(data_buf2), 50, 1); /* 20ms timeout, CRC enabled */
	if (ret == 0) {
		cprintf(con, "[Error] Could not read block %u to check\r\n", block);
		return -1;
	}
	// check values
	if (data_buf2[0] != values[0] || data_buf2[1] != values[1] || data_buf2[2] != values[2] || data_buf2[3] != values[3]) {
		cprintf(con, "[Error] Write to block %u failed (writing '%02X %02X %02X %02X', block reads '%02X %02X %02X %02X')\r\n",
				block, values[0], values[1], values[2], values[3], data_buf2[0], data_buf2[1], data_buf2[2], data_buf2[3]);
		return -1;
	}
	// all ok
	return 0;
}

//============================================================================//

void hydranfc_srix_clone(t_hydra_console *con, const char *filename)
{
	// read from file
	if (hydranfc_srix_read_from_file(con, filename, g_srix_buffer, g_srix_blockStatus) < 0)
		return;

	// TEMP: dump it
#if 0
	uint32_t i;
	for (i = 0; i < 128; ++i) {
		cprintf(con, "[%02X] %02X %02X %02X %02X\r\n", i, g_srix_buffer[i * 4], g_srix_buffer[i * 4 + 1], g_srix_buffer[i * 4 + 2], g_srix_buffer[i * 4 + 3]);
	}
#endif
	
	// initiate
	if (hydranfc_srix_initiate_tag(con) < 0) {
		Trf797xTurnRfOff();
		return;
	}

	// write all blocks (from 7 to 127)
	uint8_t j, cnt = 0;
	for (j = 7; j <= 127; ++j) {
		// skip block?
		if (g_srix_blockStatus[j] == 1) {
			// write block
			if (hydranfc_srix_write_block(con, j, &g_srix_buffer[j * 4]) >= 0)
				++cnt;
		} else {
			cprintf(con, "Skipping block %u\r\n", j);
		}
	}
	cprintf(con, "%u blocks written\r\n", cnt);

	// turn RF off
	Trf797xTurnRfOff();
}


void hydranfc_srix_fill_all(t_hydra_console *con, uint32_t byteValue)
{
	// buffer
	uint8_t ch = (uint8_t) byteValue;
	uint8_t buff[] = { ch, ch, ch, ch };

	cprintf(con, "Filling tag with 0x%02X\r\n", ch);
	
	// initiate
	if (hydranfc_srix_initiate_tag(con) < 0) {
		Trf797xTurnRfOff();
		return;
	}

	// write all blocks (from 7 to 127)
	uint8_t j, cnt = 0;
	for (j = 7; j <= 127; ++j) {
		// write block
		if (hydranfc_srix_write_block(con, j, buff) >= 0)
			++cnt;
	}
	cprintf(con, "%u blocks written\r\n", cnt);

	// turn RF off
	Trf797xTurnRfOff();
}

void hydranfc_srix_scan(t_hydra_console *con)
{
	// initiate
	if (hydranfc_srix_initiate_tag(con) < 0) {
		Trf797xTurnRfOff();
		return;
	}

	// turn RF off
	Trf797xTurnRfOff();
}

void hydranfc_srix_dump(t_hydra_console *con, const char *filename)
{
	// initiate
	if (hydranfc_srix_initiate_tag(con) < 0) {
		Trf797xTurnRfOff();
		return;
	}
	
	// dump to file
	hydranfc_srix_dump_to_file(con, filename);
		
	/* Turn RF OFF (Chip Status Control Register (0x00)) */
	Trf797xTurnRfOff();
}
