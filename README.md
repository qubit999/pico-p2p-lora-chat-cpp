# Pico P2P LoRa Chat 🚀 🚀 🚀 

This project demonstrates a half-duplex peer-to-peer (P2P) chat application using Raspberry Pi Pico 2W and Waveshare SX1262 LoRa modules.

## Hardware Used

- Raspberry Pi Pico 2W
- Waveshare SX1262 LoRa Module

For more information on the Waveshare SX1262 LoRa Module, visit the [Waveshare Wiki](https://www.waveshare.com/wiki/Pico-LoRa-SX1262).

## Prerequisites

- Arduino IDE installed
- Arduino-Core for Pico 2W (rp2350) installed ([check how to install here](https://github.com/earlephilhower/arduino-pico?tab=readme-ov-file#installation))
- [RadioLib library for Arduino-Core](https://github.com/jgromes/RadioLib) installed (search RadioLib in Arduino IDE and install it)

## Wiring

Connect the Pico 2W to the Waveshare SX1262 module. It's an expansion module that sits on top of the Pico.

## Setup

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/pico-p2p-lora-chat.git
    ```

2. Open the project in Arduino IDE.

3. Select the correct board and port:
    - Board: `Raspberry Pi Pico`
    - Port: Select the appropriate port for your Pico 2W

4. Upload the code to the Pico 2W.

## Usage

1. Power on both Pico 2W devices with the SX1262 modules connected.
2. Open the Serial Monitor in Arduino IDE for both devices.
3. Type a message in one Serial Monitor and press enter.
4. The message should appear on the other device's Serial Monitor.

Now you're ready to chat! :-)

## Examples

![Peer #1](https://i.imgur.com/Nr0fo4C.png)
![Peer #2](https://i.imgur.com/TOd5P33.png)