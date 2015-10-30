#ifndef GPS_DATA_H__
#define GPS_DATA_H__

#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct gps_data{

      uint8_t cmd;
      uint8_t latitude[5];
      uint8_t longitude[5];
      uint8_t checkpoint;
      uint8_t bDestination;

} gps_data;

void getDataFromBluetooth();

#ifdef __cplusplus
}
#endif

#endif
