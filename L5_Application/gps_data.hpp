#ifndef GPS_DATA_HPP_ // gps_data.hpp
#define GPS_DATA_HPP_

#include "uart2.hpp"
#include "scheduler_task.hpp"

typedef struct {
    float timeUTC;
    float latitude;
    float longitude;
    char formatNMEA[6];
    char nsIndicator;
    char ewIndicator;
    uint8_t gpsIndicator;
    uint8_t satInUse;
}gpsData_t;

/*
 * GPS data reading task.
 * This task reads the data from the gps connected to UART2 and
 * transmits out the data through CAN1.
 */
class gps_data : public scheduler_task{
        public:

        gps_data(uint8_t priority) :
            scheduler_task("gps fetch data", 1024, priority),
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

            vTaskDelay(500);
            return true;
        }

        void initializeGPSComm();
        bool initializeGPSBuffers();
        void readRawGPSData();
        void formatGPSData();

        private:
        QueueHandle_t gpsDataBuffer_q;
        Uart2 &gpsComm;
        gpsData_t gpsFormattedData;

        char gpsRawData[70]; // to read the raw data from gps

        static const uint16_t gpsBaud = 38400; // Baud rate at which the GPS communicates
        static const uint8_t gpsRxQSz = 100; // Queue size of receive buffer of uart2
        static const uint8_t gpsTxQSz = 1; // Queue size of transmit buffer of uart2

};

#endif  // gps_data.hpp
