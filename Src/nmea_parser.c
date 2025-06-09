#include "nmea_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
static float nmea_to_decimal(const char* nmea_coord, char direction) {
    if (!nmea_coord || strlen(nmea_coord) < 4)
        return 0.0f;

    int deg_len = (direction == 'N' || direction == 'S') ? 2 : 3;


    char deg_str[4] = {0};
    strncpy(deg_str, nmea_coord, deg_len);
    float degrees = atof(deg_str);


    const char* min_str = nmea_coord + deg_len;
    float minutes = atof(min_str);

    float dec = degrees + minutes / 60.0f;

    if (direction == 'S' || direction == 'W')
        dec = -dec;

    return dec;
}

bool GPS_ParseGPRMC(const char* sentence, gps_data_t* data) {
    if (!sentence || !data)
        return false;



    char copy[GPS_BUFFER_SIZE];
    strncpy(copy, sentence, GPS_BUFFER_SIZE-1);
    copy[LINE_BUFFER_SIZE-1] = '\0';

    char *tokens[13];
    int i = 0;

    char *token = strtok(copy, ",");
    while (token && i < 13) {
        tokens[i++] = token;
        token = strtok(NULL, ",");
    }

    if (i < 7)
    { return false;}

    if (tokens[2][0] != 'A') {
        data->fix_status = 'V';
        data->new_data_available = false;
        return false;
    }

    data->fix_status = 'A';

    data->latitude = nmea_to_decimal(tokens[3], tokens[4][0]);
    data->longitude = nmea_to_decimal(tokens[5], tokens[6][0]);

    strncpy(data->raw_sentence, sentence, LINE_BUFFER_SIZE-1);
    data->raw_sentence[LINE_BUFFER_SIZE-1] = '\0';
    if (tokens[7] && strlen(tokens[7]) > 0)
    { data->speed_knots = atof(tokens[7]);}
    else
        {data->speed_knots = 0.0f;}

    data->new_data_available = true;

    return true;
}


bool GPS_ParseGNGLL(const char* sentence, gps_data_t* data) {
    if (!sentence || !data)
        return false;

    char copy[LINE_BUFFER_SIZE];
    strncpy(copy, sentence, LINE_BUFFER_SIZE - 1);
    copy[LINE_BUFFER_SIZE - 1] = '\0';

    char* tokens[10];
    int i = 0;

    char* token = strtok(copy, ",");
    while (token && i < 10) {
        tokens[i++] = token;
        token = strtok(NULL, ",");
    }

    // Debug print number of tokens


    if (tokens[6][0] != 'A') {

        data->fix_status = 'V';
        data->new_data_available = false;
        return false;
    }

    data->fix_status = 'A';

    data->latitude = nmea_to_decimal(tokens[1], tokens[2][0]);
    data->longitude = nmea_to_decimal(tokens[3], tokens[4][0]);

    strncpy(data->raw_sentence, sentence, LINE_BUFFER_SIZE - 1);
    data->raw_sentence[LINE_BUFFER_SIZE - 1] = '\0';

    data->speed_knots = 0.0f;
    data->new_data_available = true;

    return true;
}

bool GPS_ParseGPGGA(const char* sentence, gps_data_t* data) {
    if (!sentence || !data)
        return false;


    char copy[LINE_BUFFER_SIZE];
    strncpy(copy, sentence, LINE_BUFFER_SIZE - 1);
    copy[LINE_BUFFER_SIZE - 1] = '\0';

    char* tokens[15];
    int i = 0;

    char* token = strtok(copy, ",");
    while (token && i < 15) {
        tokens[i++] = token;
        token = strtok(NULL, ",");
    }

    if (i < 7)
        return false;

    int fix_quality = atoi(tokens[6]);
    if (fix_quality == 0) {
        data->fix_status = 'V';
        data->new_data_available = false;
        return false; // no fix
    }

    data->fix_status = 'A';

    data->latitude = nmea_to_decimal(tokens[2], tokens[3][0]);
    data->longitude = nmea_to_decimal(tokens[4], tokens[5][0]);

    strncpy(data->raw_sentence, sentence, LINE_BUFFER_SIZE - 1);
    data->raw_sentence[LINE_BUFFER_SIZE - 1] = '\0';

    data->speed_knots = 0.0f; // GGA doesn't have speed info

    data->new_data_available = true;

    return true;
}



