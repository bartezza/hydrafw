# Copyright (C) 2015-2020 Guillaume VINET
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import time
from pynfcreader.devices.hydra_nfc import HydraNFC
from pynfcreader.sessions.iso14443.iso14443a import Iso14443ASession
from pynfcreader.tools import utils
import pyHydrabus

# n=pyHydrabus.NFC('/dev/hydrabus')
# >>> # Set mode to ISO 14443A
# >>> n.mode = pyHydrabus.NFC.MODE_ISO_14443A
# >>> # Set radio ON
# >>> n.rf = 1


def comment_data(msg, data):
    # self._logger.info(msg)
    print(msg)
    for hit in utils.get_pretty_print_block(data):
        # self._logger.info("\t" + hit)
        print("\t" + hit)



# nfc = HydraNFC(port="/dev/ttyACM0", debug=False)
# hn = Iso14443ASession(drv=hydra_nfc, block_size=120)
# nfc.connect()
# # nfc.set_mode_iso14443A()
# nfc.set_mode_iso14443B()
# nfc.field_on()


nfc = pyHydrabus.NFC("/dev/ttyACM0")

if 1:
    # nfc.mode = pyHydrabus.NFC.MODE_ISO_14443B
    nfc.mode = pyHydrabus.NFC.MODE_ISO_14443A
    nfc.rf = 1

    resp = nfc.write_bits(b'\x26', 7)
    if not resp:
        raise Exception("REQ A failure")
    comment_data("ATQA:", resp)

    resp = nfc.write(b'\x93\x20', 0)
    if not resp:
        raise Exception("Anticol failure")
    comment_data("Anticol:", resp)
else:
    nfc.mode = pyHydrabus.NFC.MODE_ISO_14443B
    # nfc.mode = pyHydrabus.NFC.MODE_ISO_14443A
    nfc.rf = 1

    # srix initiate
    # return chip id
    resp = nfc.write(b'\x06\x00', crc=1)
    if not resp:
        raise Exception("Initiate failure")
    comment_data("Initiate:", resp)

    chip_id = resp[0:1]
    # print(chip_id)

    # srix select
    resp = nfc.write(b"\x0E" + chip_id, crc=1)
    if not resp:
        raise Exception("Select failure")
    comment_data("Select:", resp)

    # srix get_uid
    resp = nfc.write(b"\x0B", crc=1)
    if not resp:
        raise Exception("GET_UID failure")
    comment_data("UID:", resp)
    # first should be 0xD0

    # srix read block
    for i in range(128):
        block = i
        resp = nfc.write(b"\x08" + bytes([block]), crc=1)
        if not resp:
            raise Exception(f"READ_BLOCK ({block}) failure")
        comment_data(f"Block {block}:", resp)



#hn.field_off()
#time.sleep(0.1)
#hn.field_on()
#hn.polling()

# hn.send_apdu("00 a4 04 00   0E   32 50 41 59 2E 53 59 53 2E 44 44 46 30 31   00")
# hn.send_apdu("00 a4 04 00   07   A0 00 00 00 42 10 10  00")
# hn.send_apdu("00 a4 04 00   07   A0 00 00 00 04 10 10   00")
# hn.send_apdu("00 a4 04 00   07   A0 00 00 00 03 10 10   00")
# hn.send_apdu("00 a4 04 00   05   A0 00 00 00 03    00")

# nfc.field_off()
nfc.rf = 0

