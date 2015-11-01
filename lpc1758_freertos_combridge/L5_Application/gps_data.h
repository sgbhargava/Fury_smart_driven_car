#ifndef GPS_DATA_H__
#define GPS_DATA_H__

#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    check,
    start,
    init,

} CMD_CAN;

typedef enum{
    heartbeat= 0x180,
    drive_mode = 0x181,
    latitude = 0x182,
    longitude = 0x183,
} CAN_MSG_ID_T;

typedef struct gps_data{

      uint8_t cmd;
      uint8_t latitude[5];
      uint8_t longitude[5];
      uint8_t checkpoint;
      uint8_t bDestination;
      uint16_t distance;

} gps_data;

void initForGPSData(void);
void getDataFromBluetooth();
void bridge_canTx();
#ifdef __cplusplus
}
#endif

#endif
