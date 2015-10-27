#ifndef GPS_DATA_HPP_ // gps_data.hpp
#define GPS_DATA_HPP_

#include "uart2.hpp"
#include "scheduler_task.hpp"
#include "stdio.h"
#include "io.hpp"

#define GPSMODULE   1

typedef struct {
    float   timeUTC;
    float   latitude;
    float   longitude;
    float   speed;
    float   courseDeg;
    int     dateUTC;
    char    formatNMEA[6];
    char    gpsStatus[2];
    char    nsIndicator;
    char    ewIndicator;
}gpsData_t;

/*
 * GPS data reading task.
 * This task reads the data from the gps connected to UART2.
 */
class gps_data{
        public:
        gps_data()
        {
            //initializeGPSComm();
        }

        void initializeGPSComm();
        void initializeGPSBuffers();
        void readRawGPSData();
        void formatGPSData();

        private:
        QueueHandle_t gpsDataBuffer_q;
        gpsData_t gpsFormattedData;
        Uart2 &gpsComm = Uart2::getInstance();

        char gpsRawData[70]; // to read the raw data from gps
};

#endif  // gps_data.hpp
