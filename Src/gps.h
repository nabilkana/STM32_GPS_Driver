#ifndef GPS_H
#define GPS_H
#include "stm32f1xx_hal.h"

#include <stdbool.h>

// Size of your DMA buffer (adjust if needed)
#define GPS_BUFFER_SIZE 512


typedef struct {
    float latitude;
    float longitude;
    char fix_status;         // 'A' = valid fix, 'V' = invalid fix
    float speed_knots;
    bool new_data_available;
    char raw_sentence[GPS_BUFFER_SIZE];
} gps_data_t;


void start_gps_dma(void);
void process_uart_dma_data(void);


float GPS_GetLatitude(void);
float GPS_GetLongitude(void);
char GPS_GetFixStatus(void);
float GPS_GetSpeedKnots(void);
const char* GPS_GetRawSentence(void);


bool nmea_checksum_valid(const char *sentence);
bool GPS_DataReady(void);
void GPS_ClearDataReady(void);
extern volatile uint16_t dma_len;
extern volatile bool gps_data_ready;
bool GPS_IsDataReady(void);

#endif // GPS_H


