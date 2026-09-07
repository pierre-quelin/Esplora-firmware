# Esplora-firmware

Firmware **Generic HID** (LUFA) for the [Arduino Esplora](https://docs.arduino.cc/retired/boards/arduino-esplora/) (ATmega32U4), paired with host library [`libesplora_hid`](https://github.com/pierre-quelin/libesplora_hid) / Foundation `EsploraBoard`.

## HID report contract (must match host)

| Direction | Content |
|-----------|---------|
| IN (64 bytes) | `[0]` bits0–3 = switch1–4 pressed; `[1..2]` light ADC little-endian `uint16` |
| OUT (64 bytes) | `[0]=R [1]=G [2]=B` (0–255) |

USB IDs: **VID `0x1209`** ([pid.codes](https://pid.codes)), **PID `0xE5F1`** (project PoC — [register](https://pid.codes) before public release so the PID is reserved). Same IDs in `libesplora_hid`. Not the LUFA GenericHID demo pair `03EB:204F`.

Endpoints: interrupt IN `0x81`, interrupt OUT `0x02`, size 64, poll 5 ms.

Interrupt IN is **event-driven** (low host CPU): send when the switch nibble changes, or when the host writes OUT (sync kick). The PC blocks in `libusb_interrupt_transfer` until then — no 5 ms poll storm. Light is sampled into that same report.

## Build

Requires [avr-gcc](https://gcc.gnu.org/wiki/avr-gcc) / WinAVR / Arduino AVR toolchain on `PATH`.

```bash
sudo apt update
sudo apt install build-essential gcc-avr avr-libc binutils-avr make avrdude git
git clone --recurse-submodules https://github.com/pierre-quelin/Esplora-firmware.git
cd Esplora-firmware
# or: git submodule update --init
make
```

Produces `EsploraHID.hex`. Override LUFA location with `LUFA_PATH=/path/to/LUFA` if not using the `lufa` submodule.

## Flash

Reset Esplora into bootloader (reset twice quickly), then:

```bash
make avrdude AVRDUDE_PORT=/dev/ttyACM0
```

(`avr109` @ 57600 — same as Leonardo/Esplora.) Adjust `AVRDUDE_PORT` for your OS.

## Sources

| File | Role |
|------|------|
| `EsploraHID.c` | USB main loop, IN/OUT HID reports |
| `Descriptors.c` | Device / config / HID report descriptors |
| `EsploraBoard.c` | Mux switches + light ADC, RGB PWM |
| `lufa/` | [LUFA](https://github.com/abcminiuser/lufa) submodule |

GPIO map follows [arduino-libraries/Esplora](https://github.com/arduino-libraries/Esplora).

## License

GPL-3.0 (see `LICENSE`). LUFA code under its own license (Dean Camera).
