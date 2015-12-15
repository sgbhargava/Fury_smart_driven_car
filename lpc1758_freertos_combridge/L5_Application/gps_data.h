#ifndef GPS_DATA_H__
#define GPS_DATA_H__

#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

#define NODE_BLUETOOTH 1
#define NODE_CAN    0
#define BLUETOOTH_NODE 102
#define CAN_WIRELESS_NODE 103

typedef enum {
    check,
    start,
    init,
    gps_init_loc,
    data_ack,
    data_nack,
    data_loc,
    compass_data,
    distance_data,
    reset_compass,
    kFilter_on
} CMD_CAN;

typedef enum{
    heartbeat= 0x180,
    drive_mode = 0x181,
    location = 0x382,
    rx_init = 0x162,
    reset = 0x420,
    gps_ack =0x47F,
    gps_loc = 0x461,
    compass_id = 0x465,
    distance_id =  0x162,
    reset_compassId = 0x385,
    kFilter_id=0x387
} CAN_MSG_ID_T;

typedef union{

    struct {
            uint32_t lattitude_dec :8;
            uint32_t lattitude_float :20;
            uint32_t longitude_dec :8;
            uint32_t longitude_float :20;
            uint32_t checkpoint:7;
            uint32_t bIsFinal:1;
    } __attribute__((packed));

    uint8_t  bytes[8];
    uint64_t qWord;
} long_lat_data ;

typedef union
{
        struct
        {
                int32_t turnDecision     : 8;
                uint32_t checkpoint      : 7;
                uint32_t isFinal         : 1;
                uint32_t dist_finalDest : 16;
                uint32_t dist_nxtPnt    : 16;
        }__attribute__((packed));
        uint64_t compassData;
        uint8_t compassData8[8];
} compass_distance_info;

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

void Uart2_flushData();
bool wirelessTransmitBT();
bool wirelessReceiveCAN();
bool wirelessInit();
void initForGPSData(void);
void getDataFromBluetooth();
void bridge_canTx();
bool SendHeartBeat();
bool GPS_SendDataToTxQueue();
void bridge_canRx1Hhz();
void bridge_canRx10Hhz();
bool wirelessTransmitCAN();
bool SendDataOverBluetooth();
bool wirelessReceiveBT();
void initTelemetry();

#ifdef __cplusplus
}
#endif

#endif
