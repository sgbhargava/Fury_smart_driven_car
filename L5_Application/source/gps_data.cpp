#include "stdio.h"
#include "gps_data.hpp"

/*
 * initialize all the buffers and queues that are used.
 */
bool gps_data::initializeGPSBuffers()
{
    gpsDataBuffer_q = xQueueCreate(10, sizeof(uint32_t));
    addSharedObject("gps_queue", gpsDataBuffer_q);
    return (NULL != gpsDataBuffer_q);
}

/*
 * initialize the UART for gps communication
 */
void gps_data::initializeGPSComm()
{
    gpsComm.init(gpsBaud,gpsRxQSz,gpsTxQSz);
}

/*
 * Read the data from gps
 */
void gps_data::readRawGPSData()
{
    gpsComm.gets(gpsRawData, 70, 10);
    printf("%s\n", gpsRawData);
}

void gps_data::formatGPSData()
{
    sscanf(gpsRawData, "%s %f %f %c %f %c %d, %d", gpsFormattedData.formatNMEA, &gpsFormattedData.timeUTC,
            &gpsFormattedData.latitude, &gpsFormattedData.nsIndicator, &gpsFormattedData.longitude,
            &gpsFormattedData.ewIndicator, &gpsFormattedData.gpsIndicator, &gpsFormattedData.satInUse);
    //printf("%s %f %f %c %f %c %d, %d\n", gpsFormattedData.formatNMEA, gpsFormattedData.timeUTC,
    //        gpsFormattedData.latitude, gpsFormattedData.nsIndicator, gpsFormattedData.longitude,
    //        gpsFormattedData.ewIndicator, gpsFormattedData.gpsIndicator, gpsFormattedData.satInUse);
    //xQueueReceive(gpsDataBuffer_q, &gpsFormattedData, 10);
}
