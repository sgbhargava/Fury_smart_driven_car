#include "stdio.h"
#include "gps_data.hpp"
#include "string.h"

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
    printf("%s\n\n", gpsRawData);
}

void gps_data::formatGPSData()
{
    /*sscanf(gpsRawData, "%6s", gpsFormattedData.formatNMEA);
    if(strcmp(gpsFormattedData.formatNMEA, "$GPRMC") == 0)
    {
        sscanf(gpsRawData, "%6s,%[^'.']9f,%c,%[^'.']8f,%c,%[^'.']9f,%c", gpsFormattedData.formatNMEA, &gpsFormattedData.timeUTC,
                    &gpsFormattedData.gpsStatus, &gpsFormattedData.latitude, &gpsFormattedData.nsIndicator,
                    &gpsFormattedData.longitude, &gpsFormattedData.ewIndicator);
        printf("latitude - %f%c\n longitude - %f%c\n time - %f\n status - %c\n\n",
                gpsFormattedData.latitude, gpsFormattedData.nsIndicator, gpsFormattedData.longitude,
                gpsFormattedData.ewIndicator, gpsFormattedData.timeUTC, gpsFormattedData.gpsStatus);
    }*/
    //xQueueReceive(gpsDataBuffer_q, &gpsFormattedData, 10);
}
