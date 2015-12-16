/*
 * sevenSeg_display.hpp
 *
 *  Created on: Nov 26, 2015
 *      Author: Tejeshwar
 */

#ifndef L5_APPLICATION_SEVENSEG_DISPLAY_HPP_
#define L5_APPLICATION_SEVENSEG_DISPLAY_HPP_

#include "uart3.hpp"
#include "uart_dev.hpp"
#include "singleton_template.hpp"

class sevenSegDispFunc
{
    public:
        void dispInit();
        void seg_display(uint8_t checkpoint_number);
    private:
        uint8_t fourthBit;
};

#endif /* L5_APPLICATION_SEVENSEG_DISPLAY_HPP_ */
