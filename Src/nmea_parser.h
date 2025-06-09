
#ifndef SRC_NMEA_PARSER_H_
#define SRC_NMEA_PARSER_H_
#include <stdbool.h>
#include "gps.h"
#define LINE_BUFFER_SIZE 512

static float nmea_to_decimal(const char* nmea_coord, char direction) ;
bool GPS_ParseGNGLL(const char* sentence, gps_data_t* data);
bool GPS_ParseGNGGA(const char* sentence, gps_data_t* data);
bool GPS_ParseGNRMC(const char* sentence, gps_data_t* data);

#endif
