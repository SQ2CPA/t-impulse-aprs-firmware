<h1 align = "center">🔥 APRS LoRa Tracker using T-Impulse Wristband 💪</h1>

<center><img src="https://i.imgur.com/PXhmSjW.png" width="80%"></center>

## What is this?

This is the first **APRS LoRa Tracker** software for **T-Impulse Wristband** from Lilygo. Currently, it is a simple project, it doesn't use the smart beacon mechanism known from other tracker softwares. Despite this, I'm publishing it now so that everyone can use it, test it and possibly develop the project

## Supported devices

-   T-Impulse LoRa Wristband `S78G` `433 MHz` Vibration (K156)

Probably other versions like `S76G` or `868 MHz` and `915 MHz` versions will also works but I haven't tested it.

Versions without the "vibration" probably just don't have vibration.

Currently, it is the only wristband that supports the LoRa module and also has a GPS module, which is necessary but unfortunately, we don't have WiFi or Bluetooth which would be useful. There are other wristbands and watches, but they don't have both a LoRa radio and GPS module at once.

We are waiting for any new wristbands and watches with LoRa and GPS that should appear on sale someday...

You can buy the tested version with `S78G` for `433 MHz` band here:

-   [From Aliexpress](https://aliexpress.com/i/1005003308747726.html "From Aliexpress")
-   [From Lilygo.cc](https://www.lilygo.cc/products/t-impulse-lora-wristband "From Lilygo.cc")

## Installation

<center><img src="https://i.imgur.com/8qtfzWG.png" width="50%"></center>

#### **Please use the included USB cable! You should press it as much as possible to the wristband to ensure a good connection. Don't move it side to side too much because you will broke the USB connector!**

1. Download latest archive which includes installer from releases tab from this repo (you can find it [here](https://github.com/SQ2CPA/t-impulse-aprs-firmware/releases "here"))

2. Plug in your wristband into USB

3. Hold **BOOT** button, then press **RESET** once and then release **BOOT** button

4. Run `install.bat` to flash your firmware

5. Open configurator (you can find it [here](https://sq2cpa.github.io/t-impulse-aprs-firmware/configurator/ "here")) to configure your wristband

If your PC doesn't recognise the board, you should change the driver using Zadig: <https://github.com/Xinyuan-LilyGO/T-Impulse?tab=readme-ov-file#4-my-computer-cannot-recognize-the-port-of-the-board->

## How to use the wristband after installation

After the first installation you will see "PLEASE CONFIGURE" screen. You have to use the configurator that you can find [here](https://sq2cpa.github.io/t-impulse-aprs-firmware/configurator/ "here").

For now, you have 3 main screens:

-   APRS with battery info
-   Time
-   GPS location

You change the screen by touching the button.

If you will touch the button for more than 3 seconds your wristband will shutdown. To power on you need to press the **RESET** button on the back (this will be improved in the future).

You can also disable only the screen by pressing the button about 1 second.

<center><img src="https://i.imgur.com/DIqkxmz.png" width="80%"></center>

## TODO

-   Smart beacon
-   Energy saving (deep sleep)

## Sources used in this project

-   <https://github.com/richonguzman/LoRa_APRS_Tracker>
-   <https://github.com/Xinyuan-LilyGO/T-Impulse>
-   <https://github.com/mikalhart/TinyGPS>
-   <https://github.com/olikraus/u8g2>
-   <https://github.com/sandeepmistry/arduino-LoRa>

### I hope you enjoy this project, 73! SQ2CPA

<center><img src="https://i.imgur.com/40Iee6a.jpg" width="50%"></center>
