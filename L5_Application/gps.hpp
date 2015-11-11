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
#include "math.h"
#include "io.hpp"
#include "hashDefine.hpp"

typedef struct {
    float_t   latitude;             // will contain latitude in deg
    float_t   longitude;            // will conatin longitude in deg
}gpsData_t;

typedef struct {
    float_t     timeUTC;            // Time parameter given by GPS
    char        formatNMEA[6];      // The format in which the GPS data is read
    char        valid[1];           // will be 'V' if it is not valid and 'A' if it is valid
    char        latDir[1];          // will contain 'N'- North or 'S'- South
    char        lonDir[1];          // will contain 'E'- East or 'W' - West
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
            //LE.on(4);
            readRawGPSData();
            formatGPSData();
            queueGPSData();

            return true;
        }


/*
 * initialize the UART for gps communication
 */
void initializeGPSComm();

/*
 * initialize all the buffers and queues that are used.
 */
bool initializeGPSBuffers();

/*
 * Read the data from gps.
 */
void readRawGPSData();

/*
 * The data read form gps will be in the form of a string.
 * The string is parsed to fetch the longitude and longitude.
 */
void formatGPSData();

/*
 * The parsed data is put on a queue for further calculations.
 */
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
