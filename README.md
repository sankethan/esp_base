# BTP ESP-32 Repo
## Hardware Used
1. ESP-32 S3
2. LDC1612
3. B175 Optical Mouse
## Pin Connections
1. LDC i2c sda = GPIO10,
2. LDC i2c scl = GPIO9, 
```
ESP BOARD       USB CONNECTOR (type A)
                   --
                  | || VBUS (5V)
[USB_DM]  ------> | || D-
[USB_DP]  ------> | || D+
                  | || GND
                   --
```
### USB PHY pin mapping

|              | USB_DP | USB_DM |
| ------------ | ------ | ------ |
| ESP32-S2/S3  | GPIO20 | GPIO19 |
## Installation
### Installation of ESP-IDF
https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/get-started/index.html#installation
### Installation of python Libraries
Run the below from the CWD.
```
pip3 install -r requirements.txt
```
## Build and Monitor code
To build the code run `idf.py build` from the CWD.
To flash the code run `idf.py flash` from the CWD.
To open a serial monitor run `idf.py monitor` from the CWD.
## Plot Data
1. Flash the code onto the esp32 s3. 
2. Open plot.ipynb and run the code. 
3. Interrupt the first cell when enough data has been collected.
4. Run the 2nd cell. 
## FAQ
1. `could not open port /dev/ttyUSB0:` Change the port number in the first cell in plot.ipynb
## References
1. ESP32 API Reference - https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/get-started/index.html#
2. LDC1612 Datasheet - https://www.ti.com/lit/ds/symlink/ldc1612.pdf?ts=1744304383735&ref_url=https%253A%252F%252Fwww.google.com%252F
3. B175 Mouse Technical Specifications - https://support.logi.com/hc/en-in/articles/360023467313-B175-Wireless-Mouse-Technical-Specifications
## Contact
1. Sankethan - rsankethan9rathod@gmail.com
2. Gokul     - gokulss221@gmail.com
