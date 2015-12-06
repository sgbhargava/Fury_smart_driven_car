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
#include "lpc_sys.h"
#include "semphr.h"
#include "switches.hpp"
#include "utilities.h"
#include "wireless.h"

/****************************************************************************************************************/
/* gloabal variables*/
gps_data gGpsData[20];
gps_data gGpsDataRx[10];
char gData[60];
char rxData[60];
QueueHandle_t GPSdataTxQueue =0;
SemaphoreHandle_t GPSDataTxSem = 0;
SemaphoreHandle_t GPSDataRxSem = 0;
QueueHandle_t GPSdataRxQueue =0;
bool sendStatusFlag = 0;/* flag to control GPS checkpoint data overwritten before being sent*/
bool initStatusFlag = 0;
static int  gChrCnt =0;
/****************************************************************************************************************/

void initForGPSData(void)
{
#if NODE_CAN
    const can_std_id_t slist[] = { CAN_gen_sid(can1,reset),CAN_gen_sid(can1,gps_loc),CAN_gen_sid(can1,gps_ack)};
    int count_slist = sizeof(slist);
    /* queue to transmit GPS data from Bridge module to GPS module */
#endif
    GPSdataTxQueue = xQueueCreate(20,sizeof(cmd_data));
    /* Queue to Reciev data from GPS module */
    GPSdataRxQueue = xQueueCreate(20,sizeof(cmd_data));
    /* semaphore to deque the data at a specific interval*/
    GPSDataTxSem = xSemaphoreCreateBinary();

    /* init Can */

#if NODE_CAN
    if( CAN_init(can1,100,32,32,NULL,NULL))
    {
        LOG_DEBUG("can initialized\n");
        CAN_reset_bus(can1);
        delay_us(100);
        SendHeartBeat();
    }

    if( CAN_setup_filter(slist,count_slist,0,0,0,0,0,0))
    {
        LOG_DEBUG("filter initialized\n");
    }
#endif
}

bool wirelessInit()
{
    #if NODE_BLUETOOTH
    mesh_set_node_address(BLUETOOTH_NODE);
    #else
    mesh_set_node_address(CAN_WIRELESS_NODE);
    #endif
    printf("Node address:%d\n",mesh_get_node_address());

    return 1;
}

bool FormPacketAndSendCAN(uint8_t len,uint8_t cmd,uint8_t index,uint8_t *p)
{
    mesh_packet_t pkt = {0};

    if(mesh_form_pkt(&pkt,BLUETOOTH_NODE,mesh_pkt_ack,1,0))
    {
        pkt.info.data_len = len;
        pkt.data[0] = cmd;

        if(len >1 && p)
        {
            pkt.data[1] = index;
            memcpy(&pkt.data[2],p,8);
        }

        if( !mesh_send_formed_pkt(&pkt))
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

bool wirelessTransmitCAN()
{
    cmd_data nCmd = {0};

    if( xQueueReceive(GPSdataRxQueue,&nCmd,0) )
    {
        switch(nCmd.cmd)
        {
            case data_ack:
            {
               FormPacketAndSendCAN(1,data_ack,0,NULL);

            }
            break;
            case data_loc:
            {
                int i;
                FormPacketAndSendCAN(10,data_loc,nCmd.index,gGpsDataRx[nCmd.index].longLatdata.bytes);

            }
            break;
            // TODO add case for other msgs
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

bool wirelessReceiveCAN()
{    mesh_packet_t pkt={0};
    cmd_data nCmd ={0};

    if( wireless_get_rx_pkt(&pkt,0))
    {
        printf("packet received\n");
        switch(pkt.data[0])
        {
            case check:
            {
                nCmd.cmd = pkt.data[0];
                nCmd.index = pkt.data[1];

                memcpy(gGpsData[nCmd.index].longLatdata.bytes,&pkt.data[2],8);
                gGpsData[nCmd.index].cmd = nCmd.cmd;
                gGpsData[nCmd.index].bDestination = gGpsData[nCmd.index].longLatdata.bIsFinal;

               // printf("%d %d.%d\n",nCmd.cmd,gGpsData[nCmd.index].longLatdata.lattitude_dec,gGpsData[nCmd.index].longLatdata.lattitude_float);
               // printf("packet Recieved\n");

                if( gGpsData[nCmd.index].bDestination)
                {
                    sendStatusFlag =1;
                    xSemaphoreGive(GPSDataTxSem);
                }
            }
            break;

            case start:
            {
                nCmd.cmd = pkt.data[0];
                xQueueSend(GPSdataTxQueue,&nCmd,0);
            }
            break;
        }
    }
    else
    {
        return 0;
      //  printf("not Receiving\n");
    }

    return 1;
}


bool wirelessReceiveBT()
{
    mesh_packet_t pkt={0};
    cmd_data nCmd ={0};
    long_lat_data data ={0};

    if( wireless_get_rx_pkt(&pkt,0))
    {
        nCmd.cmd = pkt.data[0];
        switch(nCmd.cmd)
        {
            case data_ack:
            {
                printf("Ack received\n");
                xQueueSend(GPSdataRxQueue,&nCmd,0);
            }
            break;
            case data_loc:
            {

                nCmd.index = pkt.data[1];

                memcpy(data.bytes,&pkt.data[2],8);
                printf("lat:%d.%d, long:-%d.%d\n",data.lattitude_dec,data.lattitude_float,data.longitude_dec,
                        data.longitude_float);
                printf("\n");
            }
            break;
            // TODO handle msgs for other incoming data to display on App
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

bool wirelessTransmitBT()
{
    mesh_packet_t pkt = {0};
    cmd_data nCmd = {0};
    long_lat_data data ={0};

    if( xQueueReceive(GPSdataTxQueue,&nCmd,0) )
    {
       switch(nCmd.cmd)
       {
           case check:
           {
               data = gGpsData[nCmd.index].longLatdata;

               if(mesh_form_pkt(&pkt,CAN_WIRELESS_NODE,mesh_pkt_ack,1,0))
               {
                   pkt.info.data_len = 10;
                   pkt.data[0] = nCmd.cmd;
                   pkt.data[1] = nCmd.index;
                   memcpy(&pkt.data[2],data.bytes,8);

                   if( mesh_send_formed_pkt(&pkt))
                   {
                       printf("check Packet sent to %d from %d:%d\n",pkt.nwk.dst,pkt.nwk.src,nCmd.index);
                   }
               }

               if( gGpsData[nCmd.index].bDestination)
               {
                   Uart2_flushData();
                   sendStatusFlag =0;
               }
           }
           break;

           case start:
           {
               if( mesh_form_pkt(&pkt,CAN_WIRELESS_NODE,mesh_pkt_ack,1,1,nCmd.cmd))
               {
                   pkt.info.data_len = 1;
                   pkt.data[0] = nCmd.cmd;
                   if( wireless_send_formed_pkt(&pkt))
                    {
                      printf("start Packet sent\n");
                    }
               }
           }
           break;

       }

    }
    else
   {
       return 0;
   }

   return 1;
}

bool SendHeartBeat()
{
    can_msg_t msg ={0};
    cmd_data nCmd = {0};
    msg.data.qword = 0;
    msg.frame_fields.data_len =0;
    msg.msg_id = heartbeat;

    if(!Switches::getInstance().getSwitch(1))
    {
        if( CAN_tx(can1, &msg, 0))
        {
          //  printf("Heart beat Txted\n");
            return 1;
        }
        else
        {
          //  printf("Heart Beat failed\n");
        }
    }
    else
    {
        nCmd.cmd = data_ack;
        xQueueSend(GPSdataRxQueue,&nCmd,0);
        printf(" Sent data_ack \n");
        return 0;
    }

}

bool GPS_SendDataToTxQueue()
{
    int i = 0;
    cmd_data nCmd = {0};

    if( xSemaphoreTake(GPSDataTxSem,0))
    {
        do
        {
          nCmd.cmd = check;
          nCmd.index = i;
          xQueueSend(GPSdataTxQueue,&nCmd,0);

        }while(!gGpsData[i++].bDestination);

        return 1;
    }
    else
    {
        return 0;
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
            sscanf(cmdParams,"%u.%6d",&b1,&b2);
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
           pGps->longLatdata.bIsFinal = b1;
           pGps->bDestination = b1;

           /* checks if the checkpoints are in order and no data is missing  */
           if( pGps->longLatdata.checkpoint == (count) )
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
    cmd_data nCmd = {0};

    if( !strcmp(cmd,"start"))
    {

        nCmd.cmd = start;
        xQueueSend(GPSdataTxQueue,&nCmd,0);
        return 1;
    }

    if( !strcmp(cmd,"init"))
    {
        //not needed currently
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
                   sendStatusFlag =1;
                   count =0;
                   xSemaphoreGive(GPSDataTxSem);// semaphore to txt all the checkpoint data from 1Khz periodic task
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

void Uart2_flushData()
{
    char c;
    while(Uart2::getInstance().getChar(&c,0));
    gChrCnt = 0;
}

void getDataFromBluetooth()
{
    static Uart2 &u2 = Uart2::getInstance();
    char c;

    if( u2.getChar(&c,0))
    {
      //  printf("%c\n",c);
        if(c == '$')
        {
            gData[gChrCnt]=0;
            gChrCnt =0;
            printf( "%s\n",gData);
            if( !ProcessDataAndSend(gData) )
            {
                Uart2_flushData();
                printf( " data not parsed\n");
            }
        }
        else
        {
           gData[gChrCnt]=c;
           gChrCnt++;
        }
    }

}

void bridge_canTx()
{
    can_msg_t canData ={0};
    cmd_data nCmd  ={0};
    int i=0;

    if( xQueueReceive(GPSdataTxQueue,&nCmd,0) )
    {
        switch(nCmd.cmd)
        {
            case check:
            {
                    canData.frame_fields.data_len = 8;
                    canData.msg_id = location ; // sending latitude  to master
                    i = nCmd.index;

#if 1
                   printf("lat:%u.%d,long:%u.%d,chk:%d,dest:%d \n",gGpsData[i].longLatdata.lattitude_dec,
                           gGpsData[i].longLatdata.lattitude_float,
                           gGpsData[i].longLatdata.longitude_dec,
                           gGpsData[i].longLatdata.longitude_float,
                           gGpsData[i].longLatdata.checkpoint,
                           gGpsData[i].longLatdata.bIsFinal);
#endif
                   canData.data.qword = gGpsData[i].longLatdata.qWord;

                   if( CAN_tx(can1,&canData,0))
                   {
                       printf("CAN_Tx:data txted\n");
                   }
                   else
                   {
                       printf("CAN_Tx failed\n");
                   }

                    if( gGpsData[i].bDestination)
                    {
                      //Uart2_flushData();
                      sendStatusFlag =0;
                    }

            }
            break;

            case start:
            {
                   canData.frame_fields.data_len = 1;
                   canData.msg_id = drive_mode;
                   canData.data.bytes[0]= 1;

                   if( CAN_tx(can1,&canData,0))
                   {
                       printf("sending start over can");
                   }
                   else if(!CAN_is_bus_off(can1))
                   {
                       printf("sending start over can failed!!!");
                       /*retransmit*/
                       xQueueSend(GPSdataTxQueue,&nCmd,0);
                   }

            }
            break;

            case init:
            {
                //not using currently as its not needed
                printf("can msg to get initial car location");
            }
            break;

        }

    }

}

bool Send_Uart2(char *p)
{
    Uart2::getInstance().put(p,100);
    return 1;
}

bool SendDataOverBluetooth(void)
{
    cmd_data nCmd = {0};
    char *p = rxData;

    if(xQueueReceive(GPSdataRxQueue,&nCmd,0))
    {
        switch(nCmd.cmd)
        {
            case data_ack:
            {
                sprintf(p,"a~");
                printf("received: %s\n",p);
                Send_Uart2(p);
            }
            break;
        }
    }
    else
    {
        return 0;
    }

}

void bridge_canRx()
{
   can_msg_t msg ={0};
   cmd_data nCmd = {0};
   static int count = 0;

   if( CAN_rx(can1,&msg,0))
   {
       switch(msg.msg_id)
       {
           case gps_ack:
           {
               nCmd.cmd = data_ack;
               xQueueSend(GPSdataRxQueue,&nCmd,0);
               printf("gps ack received\n");
           }
           break;

           case reset:
           {
               printf("Heart beat failed!! Reset!!\n");
               LOG_DEBUG("Heart beat failed!! Reset!!\n");
               sys_reboot();
           }
           break;

           case gps_loc:
           {
             //  printf("gps loc received\n");
               int i = count%10;
               gGpsDataRx[i].cmd = data_loc;
               gGpsDataRx[i].longLatdata.qWord = msg.data.qword;
               gGpsDataRx[i].bDestination = gGpsDataRx[i].longLatdata.bIsFinal;
               nCmd.cmd = data_loc;
               nCmd.index = i;
               count++;
               xQueueSend(GPSdataRxQueue,&nCmd,0);

           }
           break;
       }
   }
   else
   {
     //printf("CAN RX failed!!\n");
   }

}
