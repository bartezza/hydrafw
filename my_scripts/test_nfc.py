
import time
from pynfcreader.devices.hydra_nfc import HydraNFC
from pynfcreader.sessions.iso14443.iso14443a import Iso14443ASession
from pynfcreader.tools import utils
import pyHydrabus
import logging

# n=pyHydrabus.NFC('/dev/hydrabus')
# >>> # Set mode to ISO 14443A
# >>> n.mode = pyHydrabus.NFC.MODE_ISO_14443A
# >>> # Set radio ON
# >>> n.rf = 1

logging.basicConfig()
logging.root.setLevel(logging.INFO)


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


def send_command(cmd, name, crc=1, verbose: bool = True, raise_exc: bool = True):
    resp = nfc.write(cmd, crc=crc)
    if not resp:
        mex = f"Command {name} failed"
        if raise_exc:
            raise Exception(mex)
        else:
            logging.error(mex)
            return None
    if verbose:
        comment_data(name + ":", resp)
    return resp


nfc = pyHydrabus.NFC("/dev/ttyACM0")

try:

    if 0:
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
        resp = send_command(b'\x06\x00', "initiate", raise_exc=False)

        if resp is not None:
            chip_id = resp[0:1]
            # print(chip_id)

            # srix select
            resp = send_command(b"\x0E" + chip_id, "select")

            # srix get_uid
            resp = send_command(b"\x0B", "GET_UID")
            # first should be 0xD0

            # srix read block
            if 0:
                for i in range(128):
                    block = i
                    resp = send_command(b"\x08" + bytes([block]), "READ_BLOCK", verbose=False)
                    comment_data(f"Block {block}:", resp)

        if resp is None:
            # ASK CTS
            # ASK_REQT               0x10
            resp = send_command(b'\x10', "initiate", raise_exc=False)
            if resp is not None:
                print("ASK CTS INITIATED")
        
        



    #hn.field_off()
    #time.sleep(0.1)
    #hn.field_on()
    #hn.polling()

    # hn.send_apdu("00 a4 04 00   0E   32 50 41 59 2E 53 59 53 2E 44 44 46 30 31   00")
    # hn.send_apdu("00 a4 04 00   07   A0 00 00 00 42 10 10  00")
    # hn.send_apdu("00 a4 04 00   07   A0 00 00 00 04 10 10   00")
    # hn.send_apdu("00 a4 04 00   07   A0 00 00 00 03 10 10   00")
    # hn.send_apdu("00 a4 04 00   05   A0 00 00 00 03    00")

finally:
    # nfc.field_off()
    nfc.rf = 0
