This repository contains several parts - 
1. Code and Data folders contain the necessary files to test several data compression algorithms, such as GZIP, ZSTD, Heatshrink and LZ4.
2. The 'heatshrink-on-esp' folder contains an implementation of the Heatshrink algorithm which can run on the ESP32 microcontroller. We also wrote a ZSTD implementation, but ZSTD is unsuitable for the microcontroller, and fails to run due to memory limitations.
3. The 'esp-explore' folder has the code to blink the onboard LED of the ESP32-S3 devkit C-1.
4. The 'steval-board' folder contains the code to establish I2C communication between the ESP32 and the STEVAL-MKI 137V1 adapter board, which holds an onboard magnetometer (LIS3MDL).