#ifndef GPS_DATA_HPP_ // gps_data.hpp
#define GPS_DATA_HPP_

#include "scheduler_task.hpp"
#include "stdio.h"
#include "uart2.hpp"
#include "uart0.hpp"

class gps_data : public scheduler_task{

        public:
        gps_data(uint8_t priority) :
            scheduler_task("gps fetch data", 1024, priority),
            gps_Comm(Uart2::getInstance())
        {
            gps_Comm.init(38400,100,100);
        }

        bool init(void)
        {

        }

        bool run(void *p)
        {
            gps_Comm.gets(arr, 60, 10);
            printf("%s\n", arr);

            vTaskDelay(500);
            return true;
        }

        private:
        const char a = 10;
        char arr[60];
        Uart2 &gps_Comm;
};

#endif  // gps_data.hpp
