/*
 * gps.hpp
 *
 *  Created on: Oct 24, 2015
 *      Author: Abhishek Gurudutt
 *
 *  For CMPE243 project: Self drive car
 *  GPS data is fetched from this module.
 */


#ifndef L5_APPLICATION_GPS_DATA_HPP_ // gps.hpp
#define L5_APPLICATION_GPS_DATA_HPP_

#include "uart2.hpp"
#include "scheduler_task.hpp"

#define GPSMODULE   1
#define CAN_USAGE   1

typedef struct {
    float   latitude;
    float   longitude;
}gpsData_t;

typedef struct {
    float   timeUTC;
    char    formatNMEA[6];
}gpsExtendedData_t;

/*
 * GPS data reading task.
 * This task reads the data from the gps connected to UART2 and
 * transmits out the data through CAN1.
 */
class gps_data : public scheduler_task{
        public:

        gps_data(uint8_t priority) :
            scheduler_task("gps fetch data", 2048, priority),
            gpsComm(Uart2::getInstance()),
            gpsDataBuffer_q(NULL)
        {
            initializeGPSComm();
        }

        bool init(void)
        {
            bool ok;
            ok = initializeGPSBuffers();
            return ok;
        }

        bool run(void *p)
        {
            readRawGPSData();
            formatGPSData();
            queueGPSData();

            vTaskDelay(1000);
            return true;
        }

        void initializeGPSComm();
        bool initializeGPSBuffers();
        void readRawGPSData();
        void formatGPSData();
        void queueGPSData();

        private:
        QueueHandle_t gpsDataBuffer_q;
        Uart2 &gpsComm;
        gpsData_t gpsFormattedData;
        gpsExtendedData_t gpsExtendedData;

        char gpsRawData[70]; // to read the raw data from gps

        static const uint16_t gpsBaud = 38400; // Baud rate at which the GPS communicates
        static const uint8_t gpsRxQSz = 100; // Queue size of receive buffer of uart2
        static const uint8_t gpsTxQSz = 1; // Queue size of transmit buffer of uart2

};

#endif  // gps.hpp
