/*
 * gps.cpp
 *
 *  Created on: Oct 24, 2015
 *      Author: Abhishek Gurudutt
 *
 *  For CMPE243 project: Self drive car
 *  GPS data is fetched from this module.
 */


#include "stdio.h"
#include "gps.hpp"
#include "string.h"
#include "io.hpp"

/*
 * initialize all the buffers and queues that are used.
 */
bool gps_data::initializeGPSBuffers()
{
    gpsDataBuffer_q = xQueueCreate(4, sizeof(gpsData_t));
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
 * Read the data from gps.
 */
void gps_data::readRawGPSData()
{
    bool ok;
    ok = gpsComm.gets(gpsRawData, 70, 10);
    if(!ok)
        LE.toggle(4);
    //printf("%s\n\n", gpsRawData);
}

/*
 * The data read form gps will be in the form of a string.
 * The string is parsed to fetch the longitude and longitude.
 */
void gps_data::formatGPSData()
{
    sscanf(gpsRawData, "%6s", gpsFormattedData.formatNMEA);
    if(strcmp(gpsFormattedData.formatNMEA, "$GPRMC") == 0)
    {
        sscanf(gpsRawData, "%6s,9f,,8f,N,9f,", gpsFormattedData.formatNMEA, &gpsFormattedData.timeUTC,
                    &gpsFormattedData.latitude, &gpsFormattedData.longitude);
        /*printf("latitude - %f\n longitude - %f\n time - %f\n\n",
                gpsFormattedData.latitude, gpsFormattedData.longitude, gpsFormattedData.timeUTC);
    */}
}

/*
 * The parsed data is put on a queue for further calculations.
 */
void gps_data::queueGPSData()
{
    if(!xQueueSend(gpsDataBuffer_q, &gpsFormattedData, 0))
    {
        LE.toggle(3);
    }
}
