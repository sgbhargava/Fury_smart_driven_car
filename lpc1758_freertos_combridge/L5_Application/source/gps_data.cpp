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
char rxData[60];
QueueHandle_t GPSdataTxQueue =0;
QueueHandle_t GPSdataRxQueue =0;
bool sendStatusFlag = 0;/* flag to control GPS checkpoint data overwritten before being sent*/
bool initStatusFlag = 0;

void initForGPSData(void)
{
    const can_std_id_t slist[] = { CAN_gen_sid(can1,rx_init)};
    int count_slist = sizeof(slist);
    /* queue to transmit GPS data from Bridge module to GPS module */
    GPSdataTxQueue = xQueueCreate(10,sizeof(uint8_t));
    /* Queue to Reciev data from GPS module */
    GPSdataRxQueue = xQueueCreate(10,sizeof(uint8_t));
    /* init Can */
    if( CAN_init(can1,100,32,32,NULL,NULL))
    {
        LOG_DEBUG("can initialized\n");
    }

    if( CAN_setup_filter(slist,count_slist,0,0,0,0,0,0))
    {
        LOG_DEBUG("filter initialized\n");
    }
}

bool ParseGPSData(char *cmdParams,gps_data *pGps,int count, int checkCount)
{
    int b1;
    uint32_t b2;

    switch(count)
    {
        case 0:
        {
            sscanf(cmdParams,"%u.%6d",&b1,(int *)&b2);
            b1= abs(b1);

            pGps->longLatdata.lattitude_dec=  b1;
            pGps->longLatdata.lattitude_float = b2;

        }
        break;

        case 1:
        {

            sscanf(cmdParams,"%u.%6d",&b1,&b2);

            b1= abs(b1);
            pGps->longLatdata.longitude_dec=  b1;
            pGps->longLatdata.longitude_float = b2;;

        }
        break;

        case 2:
        {
            sscanf(cmdParams,"%d",&b1);
            pGps->longLatdata.checkpoint = b1;
        }
        break;

        case 3:
        {
           sscanf(cmdParams,"%d",&b1);
           pGps->bDestination = b1;

           /* checks if the checkpoints are in order and no data is missing  */
           if( pGps->longLatdata.checkpoint == (count +1) )
           {
               return 1;
           }
           else
           {
               /* send message to retransmit the data */
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
               count =0;
              // printf("bSendMsg:%d",bSendMsg);
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
                    canData.msg_id = location ; // sending latitude  to to master

                    canData.data.qword =  *(uint64_t *)(&gGpsData[i].longLatdata);

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

bool SendDataOverBluetooth(void)
{
    uint8_t nCmd = 0;
    char *p = rxData;

    if(xQueueReceive(GPSdataRxQueue,&nCmd,0)){
        Uart2::getInstance().put(p,0);
        Uart2::getInstance().putChar('$',0);// to mark the end of transmission
    }
    else
    {
        return 0;
    }

}

void bridge_canRx()
{
   can_msg_t msg ={0};
   uint8_t nCmd = 0;
   long_lat_data data = {0};

   if( CAN_rx(can1,&msg,0))
   {
       switch(msg.msg_id)
       {
           case rx_init:
           {
               data = *(long_lat_data *)(&msg.data.qword);
               nCmd = gps_init_loc;
               sprintf(rxData,"init %u.%6d -%u.%6d",data.lattitude_dec,data.lattitude_float,
                       data.longitude_dec,data.longitude_float);

               if(!sendStatusFlag)
              {
                  sendStatusFlag =1;
              }
              else
              {
                  //already data tranmission in progress
                  break;
              }

               if(xQueueReceive(GPSdataRxQueue,&nCmd,0))
               {
                   printf("Sending msg to Uart Transmit ");
               }
           }
           break;
       }
   }

}
