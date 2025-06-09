#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "gps.h"
#include "nmea_parser.h"

extern UART_HandleTypeDef huart1;
#define GPS_BUFFER_SIZE 512
extern UART_HandleTypeDef huart2;

static uint8_t gps_dma_buffer[GPS_BUFFER_SIZE];
static uint8_t line_buffer[GPS_BUFFER_SIZE];
static uint8_t valid_sentence_buffer[LINE_BUFFER_SIZE];
 volatile uint16_t dma_len = 0;
 volatile bool gps_data_ready = false;
static uint16_t line_index = 0;
static uint16_t last_pos = 0;

void start_gps_dma(void)
{
    HAL_UART_Receive_DMA(&huart1, gps_dma_buffer, GPS_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}
bool nmea_checksum_valid(const char *sentence)
{
    if (sentence[0] != '$') {
        return false; // invalid start
    }

    // Find '*' position
    const char *checksum_ptr = NULL;
    for (int i = 1; sentence[i] != '\0'; i++) {
        if (sentence[i] == '*') {
            checksum_ptr = &sentence[i + 1];
            break;
        }
    }

    if (checksum_ptr == NULL || checksum_ptr[0] == '\0' || checksum_ptr[1] == '\0') {
        return false; // no checksum found or incomplete
    }

    // Compute XOR of chars between $ and *
    uint8_t calc_cksum = 0;
    for (int i = 1; sentence[i] != '*' && sentence[i] != '\0'; i++) {
        calc_cksum ^= (uint8_t)sentence[i];
    }

    // Parse checksum from string (two hex digits)
    unsigned int sentence_cksum_int;
    if (sscanf(checksum_ptr, "%2X", &sentence_cksum_int) != 1) {
        return false; // failed to parse checksum
    }
    uint8_t sentence_cksum = (uint8_t)sentence_cksum_int;


    return (calc_cksum == sentence_cksum);
}










gps_data_t current_gps_data = {0};
void process_uart_dma_data(void)
{ if (!gps_data_ready) {return ; }
    uint16_t current_pos = GPS_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);

    while (last_pos != current_pos)
    {
        uint8_t c = gps_dma_buffer[last_pos];

        if (c == '\r') {
            last_pos = (last_pos + 1) % GPS_BUFFER_SIZE;
            continue;
        }

        if (c == '$') {
            line_index = 0;
            memset(line_buffer, 0, sizeof(line_buffer));
            line_buffer[line_index++] = c;
        }
        else if (line_index > 0 && line_index < sizeof(line_buffer) - 1) {
            line_buffer[line_index++] = c;

            if (c == '\n') {
                line_buffer[line_index - 1] = '\0';


                if (nmea_checksum_valid(line_buffer)) {


                    if (strncmp(line_buffer, "$GPRMC", 6) == 0) {
                        if (GPS_ParseGPRMC(line_buffer, &current_gps_data)) {
                            current_gps_data.new_data_available = true;
                        }
                    }
                    else if (strncmp(line_buffer, "$GNGLL", 6) == 0) {
                        if (GPS_ParseGNGLL(line_buffer, &current_gps_data)) {
                            current_gps_data.new_data_available = true;
                        }
                    }
                    else if (strncmp(line_buffer, "$GPGGA", 6) == 0) {
                        if (GPS_ParseGPGGA(line_buffer, &current_gps_data)) {
                            current_gps_data.new_data_available = true;
                        }
                    }
                }

                line_index = 0;
                memset(line_buffer, 0, sizeof(line_buffer));
            }
        }
        else {
            line_index = 0;
            memset(line_buffer, 0, sizeof(line_buffer));
        }

        last_pos = (last_pos + 1) % GPS_BUFFER_SIZE;
    }
    gps_data_ready = false ;
}


float GPS_GetLatitude(void) {
    return current_gps_data.latitude;
}

float GPS_GetLongitude(void) {
    return current_gps_data.longitude;
}

char GPS_GetFixStatus(void) {
    return current_gps_data.fix_status;
}

const char* GPS_GetRawSentence(void) {
    return current_gps_data.raw_sentence;
}
float GPS_GetSpeedKnots(void) {
    return current_gps_data.speed_knots;
}
bool GPS_DataReady(void) {
	return current_gps_data.new_data_available; }

void GPS_ClearDataReady(void) {
	current_gps_data.new_data_available = false;
}



