#include "FreeRTOS.h"
#include "queue.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector.hpp>
#include "gps_data.h"
#include "can.h"
#include "file_logger.h"
#include "uart2.hpp"
#include "semphr.h"

gps_data gGpsData[20];
char gData[60];
QueueHandle_t GPSdataTxQueue =0;
QueueHandle_t GPSdataRxQueue =0;
bool sendStatusFlag = 0;/* flag to control GPS checkpoint data overwritten before being sent*/

void initForGPSData(void)
{

    /* queue to transmit GPS data from Bridge module to GPS module */
    GPSdataTxQueue = xQueueCreate(10,sizeof(uint8_t));
    /* Queue to Reciev data from GPS module */
    GPSdataRxQueue = xQueueCreate(10,sizeof(uint8_t));
    /* init can*/
    if( CAN_init(can1,100,10,10,NULL,NULL))
    {
        LOG_DEBUG("can initialized\n");
    }

}

bool ParseGPSData(char *cmdParams,gps_data *pGps,int count, int checkCount)
{
    int b1,b2,b3,b4,b5;

    switch(count)
    {
        case 0:
        {
            sscanf(cmdParams,"%u.%2d%2d%2d%2d",&b1,&b2,&b3,&b4,&b5);

            pGps->latitude[0] =  abs(b1);
            pGps->latitude[1] =  b2;
            pGps->latitude[2] =  b3;
            pGps->latitude[3] =  b4;
            pGps->latitude[4] =  b5;
           /* printf("lat: %d %d %d %d %d\n",
                    pGps->latitude[0],
                    pGps->latitude[1],
                    pGps->latitude[2],
                    pGps->latitude[3],
                    pGps->latitude[4]);*/

        }
        break;

        case 1:
        {
            sscanf(cmdParams,"%u.%2d%2d%2d%2d",&b1,&b2,&b3,&b4,&b5);

            pGps->longitude[0] = abs(b1);
            pGps->longitude[1] = b2;
            pGps->longitude[2] = b3;
            pGps->longitude[3] = b4;
            pGps->longitude[4] = b5;
        /*    printf("long: %d %d %d %d %d\n",pGps->longitude[0],
                    pGps->longitude[1],
                    pGps->longitude[2],
                    pGps->longitude[3],
                    pGps->longitude[4]);*/
        }
        break;

        case 2:
        {
            sscanf(cmdParams,"%d",&b1);
            pGps->checkpoint = b1;
      //      printf(" checkpoint:%d\n",pGps->checkpoint);
        }
       break;

        case 3:
        {
           sscanf(cmdParams,"%d",&b1);
           pGps->bDestination = b1;
       //    printf("bdest:%d\n",pGps->bDestination);
           if( pGps->checkpoint == (count +1) )
           {
               return 1;
           }
           else
           {
               return 2;
           }
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
    static int count = 0;
    gps_data gps={0};
    uint8_t nCmd;

    if( !strcmp(cmd,"start"))
    {
        ///TODO:send start msg to can to start sending out data
        printf("%s ",cmd);
        return 1;
    }

    if( !strcmp(cmd,"init"))
    {
        ///TODO:send msg to master to get initial co-ordinates of the car to the android app
        printf("%s ",cmd);
        return 1;
    }

    if( !strcmp(cmd,"check") && !sendStatusFlag)
    {
        while((cmdParam = strtok(NULL," "))!=NULL)
        {

           bSendMsg= ParseGPSData(cmdParam,&gps,i++,count);

           if( bSendMsg == 1)
           {
               memcpy(&gGpsData[count++],&gps,sizeof(gps_data));

               /* if the final destination is received send a message to transmit
                * all the checkpoint data
                */
               if(gps.bDestination)
               {
                   nCmd = check;
                   sendStatusFlag =1;
                   count =0;
                   xQueueSend(GPSdataTxQueue,&nCmd,0);
                   return 1;
               }

               return 1;
           }
           else if(bSendMsg == 2) // data not in order
           {
               printf("bSendMsg:%d",bSendMsg);
               //TODO:Request a retransmit to the android app
               return 1;
           }
        }
    }
    return 0;
}

/*
 * cmd format
 *
 * cmd latitude longitude checkPtno IsFinalDest$
 * eg:- check 37.45456465 -121.66765367 2 0$
 */

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
    }

}

void bridge_canTx()
{
    can_msg_t canData = {0};
    uint8_t nCmd;
    int i=0;

    if( xQueueReceive(GPSdataTxQueue,&nCmd,0) )
    {
        switch(nCmd)
        {
            case check:
            {
                do
                {
                    canData.frame_fields.data_len = 8;
                    canData.msg_id = latitude ; // sending latitude  to to master
                    canData.data.bytes[0] = gGpsData[i].latitude[0];
                    canData.data.bytes[1] = gGpsData[i].latitude[1];
                    canData.data.bytes[2] = gGpsData[i].latitude[2];
                    canData.data.bytes[3] = gGpsData[i].latitude[3];
                    canData.data.bytes[4] = (gGpsData[i].distance & 0xff00) >>8;
                    canData.data.bytes[5] = (gGpsData[i].distance & 0xff);
                    canData.data.bytes[6] = gGpsData[i].checkpoint;
                    canData.data.bytes[7] = gGpsData[i].bDestination;

                    CAN_tx(can1,&canData,0);

                    canData.frame_fields.data_len = 8;
                    canData.msg_id = longitude; // sending longitude  to to master
                    canData.data.bytes[0] = gGpsData[i].longitude[0];
                    canData.data.bytes[1] = gGpsData[i].longitude[1];
                    canData.data.bytes[2] = gGpsData[i].longitude[2];
                    canData.data.bytes[3] = gGpsData[i].longitude[3];
                    canData.data.bytes[4] = (gGpsData[i].distance & 0xff00) >>8;
                    canData.data.bytes[5] = (gGpsData[i].distance & 0xff);
                    canData.data.bytes[6] = gGpsData[i].checkpoint;
                    canData.data.bytes[7] = gGpsData[i].bDestination;
                    CAN_tx(can1,&canData,0);

                }while(!(gGpsData[i++].bDestination)); //send out all checkpoint data

                sendStatusFlag = 0; //reset flag to receive data over uart
            }
            break;

            case start:
            {   //TODO
                   printf("send start over can");
            }
            break;

            case init:
            {
                //TODO
                printf("can msg to get initial car location");
            }
           break;

            case heartbeat:
            {
                //TODO
                printf("send heartbeat");
            }
           break;
        }
    }
}


