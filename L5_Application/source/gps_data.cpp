#include "stdio.h"
#include "gps_data.hpp"

/*
 * initialize all the buffers and queues that are used.
 */
void gps_data::initializeGPSBuffers()
{
    gpsDataBuffer_q = xQueueCreate(10, sizeof(uint32_t));
    addSharedObject("gps_queue", gpsDataBuffer_q);
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
void gps_data::readGPSData()
{
    gpsComm.gets(arr, 60, 10);
    printf("%s\n", arr);

}
