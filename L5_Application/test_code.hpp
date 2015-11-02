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
        addChkPnts(3289, 12145, 1);
        addChkPnts(3289, 12145, 2);
        addChkPnts(3289, 12145, 3);
        addChkPnts(3289, 12145, 4);
        addChkPnts(3289, 12145, 5);
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
