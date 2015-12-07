/*
 * sevenSeg_display.cpp
 *
 *  Created on: Nov 26, 2015
 *      Author: Tejeshwar
 */


#include "sevenSeg_display.hpp"
#include "uart_dev.hpp"
#include "uart3.hpp"
#include "utilities.h"
#include "io.hpp"

Uart3 &display = Uart3::getInstance();

void sevenSegDispFunc::dispInit()
{
    display.init(9600,10,10);
    display.putChar(0x76,10);
}

void sevenSegDispFunc::seg_display(uint8_t checkpoint_number)
{
    /*Displays Team*/
    display.putChar('7',10);
    display.putChar('E',10);
    display.putChar('A',10);
    display.putChar('n',10);

    delay_ms(250);
    display.putChar(0x76,10);
    display.putChar(0x76,10);

    /*Displays Fury*/
    display.putChar('F',10);
    display.putChar('U',10);
    display.putChar('r',10);
    display.putChar('Y',10);

    delay_ms(250);

    fourthBit = checkpoint_number;
    display.putChar(0x77,10);
    display.putChar(0x40,10);
    display.putChar('C',10);
    display.putChar('H',10);
    display.putChar('P',10);
    display.putChar(fourthBit,10);

    delay_ms(500);
}


