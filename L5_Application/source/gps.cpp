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
#include "hashDefine.hpp"

#if TESTCODE
extern float_t latTesting, longTesting;
#endif

bool gps_data::initializeGPSBuffers()
{
    gpsDataBuffer_q = xQueueCreate(4, sizeof(gpsData_t));
    addSharedObject("gps_queue", gpsDataBuffer_q);
    return (NULL != gpsDataBuffer_q);
}


void gps_data::initializeGPSComm()
{
    gpsComm.init(gpsBaud,gpsRxQSz,gpsTxQSz);
}

void gps_data::readRawGPSData()
{
    bool ok;
    ok = gpsComm.gets(gpsRawData, 70, 10);
    if(!ok)
        LE.toggle(4);
}

void gps_data::formatGPSData()
{
    uint16_t calcLat, calcLong;
    sscanf(gpsRawData, "%6s", gpsExtendedData.formatNMEA);
    if(strcmp(gpsExtendedData.formatNMEA, "$GPRMC") == 0)
    {
        sscanf(gpsRawData, "%6s,%f,%1s,%f,%1s,%f,%1s", gpsExtendedData.formatNMEA,
                &gpsExtendedData.timeUTC, gpsExtendedData.valid, &gpsFormattedData.latitude,
                gpsExtendedData.latDir, &gpsFormattedData.longitude, gpsExtendedData.lonDir);
       calcLat = gpsFormattedData.latitude / TEN_2;
       calcLong = gpsFormattedData.longitude / TEN_2;
       gpsFormattedData.latitude = ((gpsFormattedData.latitude - (calcLat * TEN_2)) / MINUTES) + calcLat;
       gpsFormattedData.longitude = ((gpsFormattedData.longitude - (calcLong * TEN_2)) / MINUTES) + calcLong;

#if TESTCODE
       latTesting = gpsFormattedData.latitude;
       longTesting = gpsFormattedData.longitude;
#endif
     }
}

void gps_data::queueGPSData()
{
    if(gpsExtendedData.valid[0] == 'A')
    {
        if(!xQueueSend(gpsDataBuffer_q, &gpsFormattedData, 0))
        {
            LE.toggle(3);
        }
    }
    else
    {
        LE.on(3);
    }
}
