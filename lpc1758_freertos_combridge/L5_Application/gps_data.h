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
    gps_init_loc
} CMD_CAN;

typedef enum{
    heartbeat= 0x180,
    drive_mode = 0x181,
    location = 0x182,
    rx_init = 0x162

} CAN_MSG_ID_T;

typedef struct {
        uint32_t lattitude_dec :8;
        uint32_t lattitude_float :20;
        uint32_t longitude_dec :8;
        uint32_t longitude_float :20;
        uint32_t checkpoint:7;
        uint32_t bIsFinal:1;
} __attribute__((packed)) long_lat_data ;

typedef union {
        long_lat_data data;
        uint64_t dWord;
} CAN_Gps_data;

typedef struct gps_data{

      uint8_t cmd;
      long_lat_data longLatdata;
      uint8_t bDestination;

} gps_data;

typedef struct cmd_data
{
        uint16_t cmd;
        uint8_t index;
} cmd_data;

void initForGPSData(void);
void getDataFromBluetooth();
void bridge_canTx();
bool SendHeartBeat();
bool GPS_SendDataToCANTx();

#ifdef __cplusplus
}
#endif

#endif
