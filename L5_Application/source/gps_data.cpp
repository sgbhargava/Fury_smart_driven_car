#include "stdio.h"
#include "gps_data.hpp"
#include "string.h"
#include "lpc_sys.h"
#include "io.hpp"

/*
 * initialize all the buffers and queues that are used.
 */
void gps_data::initializeGPSBuffers()
{
    gpsDataBuffer_q = xQueueCreate(10, sizeof(uint32_t));
    if(NULL != gpsDataBuffer_q)
    {
        puts("Error creating queue\n");
        sys_reboot_abnormal();
    }
}

/*
 * Read the data from gps
 */
void gps_data::readRawGPSData()
{
    bool ok;
    ok = gpsComm.gets(gpsRawData,70,10);
    if(!ok)
    {
        LE.toggle(4); // Toggle the 4th on board led if uart read fails.
    }
    //printf("%s\n",gpsRawData);
}

void gps_data::formatGPSData()
{
    sscanf(gpsRawData, "%6s", gpsFormattedData.formatNMEA);
    if(strcmp(gpsFormattedData.formatNMEA, "$GPRMC") == 0)
    {
        sscanf(gpsRawData, "%6s,%9f,%f,N,%f", gpsFormattedData.formatNMEA, &gpsFormattedData.timeUTC,
                   &gpsFormattedData.latitude, &gpsFormattedData.longitude);
        /*printf("lat - %f\n long - %f\n tm - %f\n st - %s\n\n",
                gpsFormattedData.latitude, gpsFormattedData.longitude, gpsFormattedData.timeUTC, gpsFormattedData.gpsStatus);
    */}
    //xQueueReceive(gpsDataBuffer_q, &gpsFormattedData, 10);
}
