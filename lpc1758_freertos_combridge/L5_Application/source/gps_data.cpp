#include "FreeRTOS.h"
#include "queue.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "gps_data.h"
#include "can.h"
#include "file_logger.h"
#include "uart2.hpp"


gps_data gGpsData;
char gData[120];
QueueHandle_t GPSdataTxQueue =0;
QueueHandle_t GPSdataRxQueue =0;

typedef enum {
    check,
    start
} CMD_CAN;

void initForGPSData(void)
{

    /* queue to transmit GPS data from Bridge module to GPS module */
    GPSdataTxQueue = xQueueCreate(10,sizeof(gps_data));
    /* Queue to Reciev data from GPS module */
    GPSdataRxQueue = xQueueCreate(10,sizeof(gps_data));
    /* init can*/
    if( CAN_init(can1,100,10,10,NULL,NULL))
    {
        LOG_DEBUG("can initialized\n");
    }

}

/*TODO: to check,received data integrity and requesting a retransmit if there is an error*/
bool ParseGPSData(char *cmdParams,int count)
{
    int b1,b2,b3,b4,b5;

    switch(count)
    {
        case 0:
        {
            sscanf(cmdParams,"%u.%2d%2d%2d%2d",&b1,&b2,&b3,&b4,&b5);
            printf("lat: %d %d %d %d %d\n",b1,b2,b3,b4,b5);
            gGpsData.latitude[0] =  abs(b1);
            gGpsData.latitude[1] =  b2;
            gGpsData.latitude[2] =  b3;
            gGpsData.latitude[3] =  b4;
            gGpsData.latitude[4] =  b5;
        }
        break;

        case 1:
        {
            sscanf(cmdParams,"%u.%2d%2d%2d%2d",&b1,&b2,&b3,&b4,&b5);

            printf("long: %d %d %d %d %d\n",b1,b2,b3,b4,b5);
            gGpsData.longitude[0] = abs(b1);
            gGpsData.longitude[1] = b2;
            gGpsData.longitude[2] = b3;
            gGpsData.longitude[3] = b4;
            gGpsData.longitude[4] = b5;
        }
        break;

        case 2:
        {
            sscanf(cmdParams,"%d",&b1);
            gGpsData.checkpoint = b1;
        }
       break;

        case 3:
        {
           sscanf(cmdParams,"%d",&b1);
           gGpsData.bDestination = b1;
           return 1;
        }
        break;
    }
    return 0;
}

bool ProcessDataAndSend(char * pData)
{
    char *cmd = strtok(pData," ");
    char *cmdParam;
    int i=0;
    bool bSendMsg = 0;

    if( !strcmp(cmd,"start"))
    {
        //send start msg to can
        printf("%s ",cmd);
        return 1;
    }

    if( !strcmp(cmd,"check"))
    {

        while((cmdParam = strtok(NULL," "))!=NULL)
        {
           printf("%s\n",cmdParam);
           bSendMsg= ParseGPSData(cmdParam,i++);

           if( bSendMsg)
           {
               // Send msg over can
               return 1;
           }
        }
    }
    return 0;
}

void getDataFromBluetooth()
{
    static Uart2 &u2 = Uart2::getInstance();
    char c;

    static int i = 0;
    if( u2.getChar(&c,0))
    {

        if(c == '$')
        {
            gData[i]=0;
            i =0;
            ProcessDataAndSend(gData);
        }
        else
        {
           gData[i]=c;
           i++;
        }
      //  printf("c: %x ",c);

    }
    // process data to send over uart

}

void bridge_canTx()
{
    gps_data gps ={0};

    if( xQueueReceive(GPSdataTxQueue,(gps_data *)&gps,0) )
    {
        switch(gps.cmd)
        {
            case check:
            {
                   printf( "send checkpoints over can\n");
            }
            break;

            case start:
            {
                   printf("send start over can");
            }
            break;
        }
    }
}

