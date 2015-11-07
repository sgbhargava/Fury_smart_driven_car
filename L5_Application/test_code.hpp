/*
 * test_code.hpp
 *
 *  Created on: Oct 30, 2015
 *      Author: Abhishek Gurudutt
 *
 *  This updates predefined gps checkpoint values.
 */

#ifndef L5_APPLICATION_TEST_CODE_HPP_
#define L5_APPLICATION_TEST_CODE_HPP_

#include "scheduler_task.hpp"
#include "can_gpsCompass.hpp"

class test_code : public scheduler_task{

    public:
    test_code(uint8_t priority) :
        scheduler_task("test code", 1024, priority)
    {

    }

    bool init(void)
    {
        addChkPnts(37, 334352, 121, 883424, 1);
        addChkPnts(37, 334424, 121, 883008, 2);
        addChkPnts(37, 334571, 121, 882960, 3);
        addChkPnts(37, 334814, 121, 882382, 4);
        addChkPnts(37, 335109, 121, 881657, 5);
        return true;
    }

    bool run(void *p)
    {

        vTaskDelay(1000);

        return true;
    }

    //private:

};



#endif /* L5_APPLICATION_TEST_CODE_HPP_ */
