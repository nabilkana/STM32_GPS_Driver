GPS Parser (STM32 + UART DMA)

Overview

This embedded project processes GPS data on an STM32 microcontroller. It receives raw NMEA sentences over UART using DMA, parses them, and extracts GPS coordinates (latitude, longitude) and fix status. Parsed results are output via a second UART port for monitoring.

Features

UART with DMA and IDLE detection for efficient reception

NMEA sentence parsing: GNGLL, GNRMC, and GNGGA

Coordinate extraction in decimal degrees

Fix status detection

Output via HAL_UART_Transmit for easy serial monitoring

Supported NMEA Sentences

$GNGLL – Geographic Position

Latitude/Longitude with direction and time

$GNRMC – Recommended Minimum GNSS Data

Includes time, status, speed, and date

$GNGGA – GPS Fix Data

Includes number of satellites and altitude

Each sentence is parsed using a shared gps_data_t struct.

Coordinate Format & Conversion

NMEA format:

Latitude: DDMM.MMMM with direction N or S

Longitude: DDDMM.MMMM with direction E or W

Converted to decimal degrees:

int deg_len = (direction == 'N' || direction == 'S') ? 2 : 3;
char deg_str[4] = {0};
strncpy(deg_str, nmea_coord, deg_len);
float degrees = atof(deg_str);
float minutes = atof(nmea_coord + deg_len);
float decimal = degrees + (minutes / 60.0f);
if (direction == 'S' || direction == 'W') decimal = -decimal;

##################################################Usage

####  Hardware

STM32 board (tested on STM32F4/STM32L4 series)

GPS module (e.g., NEO-6M, u-blox)

UART wired to GPS TX

###### Setup

USART1: receive GPS data

DMA in circular mode

IDLE interrupt enabled

USART2: send debug info to PC/terminal (via HAL_UART_Transmit)

Define buffers and DMA handlers in main.c

Parse GPS data inside main loop or DMA complete callback

###### Example Output

Latitude: 38.945759
Longitude: -94.593071

######  STM32 Function Flow

DMA receives data into buffer

IDLE interrupt triggers end of sentence processing

Sentence passed to parser (e.g., GPS_ParseGNGLL)

Parsed values saved to gps_data_t struct

Main loop checks new_data_available, then transmits values

File Structure

Core/
├── Inc/
│   ├── gps.h
│   └── nmea_parser.h
├── Src/
│   ├── gps.c
│   └── nmea_parser.c
├── main.c        # UART setup, DMA handling, main loop

Build & Flash

Use STM32CubeIDE or Makefile depending on setup

Configure UARTs and DMA in CubeMX

Flash to board and monitor serial output on PC terminal (e.g., PuTTY, minicom)