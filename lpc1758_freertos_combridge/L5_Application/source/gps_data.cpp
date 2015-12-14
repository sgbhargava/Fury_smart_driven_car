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
#include "tlm/c_tlm_comp.h"
#include "tlm/c_tlm_var.h"
#include "io.hpp"

/****************************************************************************************************************/
/* gloabal variables*/
gps_data gGpsData[20];
gps_data gGpsDataRx[10];
compass_distance_info gDistanceInfo[5];

int32_t turnDecision;
uint32_t checkpoint;
uint32_t isFinal;
uint32_t dist_finalDest;
uint32_t dist_nxtPnt;
uint16_t desired;
uint16_t actual;
float lat;
float longitude;

char gData[60];
char rxData[60];
QueueHandle_t GPSdataTxQueue = 0;
SemaphoreHandle_t GPSDataTxSem = 0;
SemaphoreHandle_t GPSDataRxSem = 0;
QueueHandle_t GPSdataRxQueue = 0;
bool sendStatusFlag = 0;/* flag to control GPS checkpoint data overwritten before being sent*/
bool initStatusFlag = 0;
static int gChrCnt = 0;
bool kalmanOFF = 0;
/****************************************************************************************************************/

void initForGPSData(void)
{

    /* queue to transmit GPS data from Bridge module to GPS module */
    GPSdataTxQueue = xQueueCreate(40, sizeof(cmd_data));
    /* Queue to Reciev data from GPS module */
    GPSdataRxQueue = xQueueCreate(40, sizeof(cmd_data));
    /* semaphore to deque the data at a specific interval*/
    GPSDataTxSem = xSemaphoreCreateBinary();
    /* init Can */

#if NODE_CAN
    if( CAN_init(can1,100,64,64,NULL,NULL))
    {
        LOG_DEBUG("can initialized\n");
        CAN_reset_bus(can1);
        delay_us(100);
        SendHeartBeat();
    }

    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1,distance_id),CAN_gen_sid(can1,reset));
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1,gps_loc),CAN_gen_sid(can1,compass_id));
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1,gps_ack),CAN_gen_sid(can1,0xffff));

#endif
}

bool getCanData(can_fullcan_msg_t *pMsg, uint16_t canMsgId)
{
    can_fullcan_msg_t *data_updated = NULL;

    data_updated = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, canMsgId));

    return CAN_fullcan_read_msg_copy(data_updated, pMsg);
}

void initTelemetry()
{

    tlm_component *pComp = NULL;
    pComp = tlm_component_add("Bridge");
    //distance
    TLM_REG_VAR(pComp, dist_finalDest, tlm_uint);
    TLM_REG_VAR(pComp, checkpoint, tlm_uint);
    TLM_REG_VAR(pComp, isFinal, tlm_uint);
    TLM_REG_VAR(pComp, dist_nxtPnt, tlm_uint);
    TLM_REG_VAR(pComp, turnDecision, tlm_char);
    TLM_REG_VAR(pComp, desired, tlm_uint);
    TLM_REG_VAR(pComp, actual, tlm_uint);
    TLM_REG_VAR(pComp, lat, tlm_float);
    TLM_REG_VAR(pComp, longitude, tlm_float);

}

bool wirelessInit()
{
#if NODE_BLUETOOTH
    mesh_set_node_address(BLUETOOTH_NODE);
#else
    mesh_set_node_address(CAN_WIRELESS_NODE);
#endif
    printf("Node address:%d\n", mesh_get_node_address());

    return 1;
}

bool FormPacketAndSendCAN(uint8_t len, uint8_t cmd, uint8_t index, uint8_t *p)
{
    mesh_packet_t pkt = { 0 };

    if (mesh_form_pkt(&pkt, BLUETOOTH_NODE, mesh_pkt_ack, 1, 0))
    {
        pkt.info.data_len = len;
        pkt.data[0] = cmd;

        if (len > 1 && p)
        {
            pkt.data[1] = index;
            memcpy(&pkt.data[2], p, 8);
        }

        if (!mesh_send_formed_pkt(&pkt))
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
    cmd_data nCmd = { 0 };
    mesh_packet_t pkt = { 0 };

    if (xQueueReceive(GPSdataRxQueue, &nCmd, 0))
    {
        switch (nCmd.cmd)
        {
            case data_ack:
            {
                if (FormPacketAndSendCAN(1, data_ack, 0, NULL))
                {
                    printf("packet sending successful \n");
                }

            }
                break;
            case data_loc:
            {

                if (FormPacketAndSendCAN(10, data_loc, nCmd.index,
                        gGpsDataRx[nCmd.index].longLatdata.bytes))
                {
                    //  printf("packet sending failed \n");
                }

            }
                break;
            case compass_data:
            {
                if (mesh_form_pkt(&pkt, BLUETOOTH_NODE, mesh_pkt_ack, 1, 0))
                {
                    pkt.info.data_len = 5;
                    pkt.data[0] = nCmd.cmd;
                    pkt.data[1] = desired >> 8;
                    pkt.data[2] = desired;
                    pkt.data[3] = actual >> 8;
                    pkt.data[4] = actual;

                    if (mesh_send_formed_pkt(&pkt))
                    {
                        // do nothing
                    }
                }
            }
                break;

            case distance_data:
            {
                if (FormPacketAndSendCAN(10, distance_data, nCmd.index,
                        gDistanceInfo[nCmd.index].compassData8))
                {
                    //     printf("packet sending failed \n");
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

bool wirelessReceiveCAN()
{
    mesh_packet_t pkt ={ 0 };
    cmd_data nCmd ={ 0 };

    if (wireless_get_rx_pkt(&pkt, 0))
    {

        switch (pkt.data[0])
        {
            case check:
            {
                nCmd.cmd = pkt.data[0];
                nCmd.index = pkt.data[1];

                memcpy(gGpsData[nCmd.index].longLatdata.bytes, &pkt.data[2], 8);
                gGpsData[nCmd.index].cmd = nCmd.cmd;
                gGpsData[nCmd.index].bDestination =gGpsData[nCmd.index].longLatdata.bIsFinal;

                printf("%d %d.%d\n",nCmd.cmd,gGpsData[nCmd.index].longLatdata.lattitude_dec,gGpsData[nCmd.index].longLatdata.lattitude_float);
                printf("packet Recieved\n");

                if (gGpsData[nCmd.index].bDestination)
                {
                    sendStatusFlag = 1;
                    xSemaphoreGive(GPSDataTxSem);
                }
            }
             break;

            case start:
            case stop:
            {
                nCmd.cmd = pkt.data[0];
                xQueueSend(GPSdataTxQueue, &nCmd, 0);
            }
                break;

            case reset_compass:
            {
                printf("compass reset received\n");
                nCmd.cmd = pkt.data[0];
                xQueueSend(GPSdataTxQueue, &nCmd, 0);
            }
                break;
            case kFilter_on:
            {
                printf("K Filter received\n");
                nCmd.cmd = pkt.data[0];
                xQueueSend(GPSdataTxQueue, &nCmd, 0);
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
    mesh_packet_t pkt = { 0 };
    cmd_data nCmd = { 0 };
    long_lat_data data = { 0 };

    if (wireless_get_rx_pkt(&pkt, 0))
    {
        nCmd.cmd = pkt.data[0];
       // printf("CMd:%d\n", nCmd.cmd);
        switch (nCmd.cmd)
        {
            case data_ack:
            {
                //  printf("Ack received\n");
                if(kalmanOFF)
                {
                    LE.toggle(3);
                    kalmanOFF = 0;
                }
                xQueueSend(GPSdataRxQueue, &nCmd, 0);
            }
                break;
            case data_loc:
            {

                nCmd.index = pkt.data[1];

                memcpy(data.bytes, &pkt.data[2], 8);
                lat = data.lattitude_dec + (data.lattitude_float * 0.000001);
                longitude = data.longitude_dec + (data.longitude_float * 0.000001);
              /*  printf("lat:%d.%d, long:-%d.%d\n",data.lattitude_dec,data.lattitude_float,data.longitude_dec,
                data.longitude_float);*/
                xQueueSend( GPSdataRxQueue, &nCmd, 0);
                printf("\n");
            }
                break;
            case compass_data:
            {
                printf("Desired:%d, Actual:%d\n",((pkt.data[1]<<8)|pkt.data[2]),((pkt.data[3]<<8)|pkt.data[4]));
                desired = ((pkt.data[1] << 8) | pkt.data[2]);
                actual = ((pkt.data[3] << 8) | pkt.data[4]);
            }
                break;
            case distance_data:
            {
                nCmd.index = pkt.data[1];
                //    printf("distance data INDEX :%d\n",nCmd.index);
                memcpy(gDistanceInfo[nCmd.index].compassData8, &pkt.data[2], 8);
                dist_finalDest = gDistanceInfo[nCmd.index].dist_finalDest;
                dist_nxtPnt = gDistanceInfo[nCmd.index].dist_nxtPnt;
                isFinal = gDistanceInfo[nCmd.index].isFinal;
                turnDecision = gDistanceInfo[nCmd.index].turnDecision;
                checkpoint = gDistanceInfo[nCmd.index].checkpoint;

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
    mesh_packet_t pkt = { 0 };
    cmd_data nCmd = { 0 };
    long_lat_data data ={ 0 };

    if (xQueueReceive(GPSdataTxQueue, &nCmd, 0))
    {
        switch (nCmd.cmd)
        {
            case check:
            {
                data = gGpsData[nCmd.index].longLatdata;

                if (mesh_form_pkt(&pkt, CAN_WIRELESS_NODE, mesh_pkt_ack, 1, 0))
                {
                    pkt.info.data_len = 10;
                    pkt.data[0] = nCmd.cmd;
                    pkt.data[1] = nCmd.index;
                    memcpy(&pkt.data[2], data.bytes, 8);

                    if (mesh_send_formed_pkt(&pkt))
                    {
                        //        printf("check Packet sent to %d from %d:%d\n",pkt.nwk.dst,pkt.nwk.src,nCmd.index);
                    }
                }

                if (gGpsData[nCmd.index].bDestination)
                {
                    Uart2_flushData();
                    sendStatusFlag = 0;
                }
            }
                break;

            case start:
            case stop:
            {
                if (mesh_form_pkt(&pkt, CAN_WIRELESS_NODE, mesh_pkt_ack, 1, 0))
                {
                    pkt.info.data_len = 1;
                    pkt.data[0] = nCmd.cmd;
                    if (wireless_send_formed_pkt(&pkt))
                    {
                        printf("start Packet sent\n");
                    }
                    else
                    {
                        //       printf("sending failed\n");
                    }

                }
                else
                {
                    printf("packet formation failed\n");
                }
            }
                break;

            case reset_compass:
            {
                //  FormPacketAndSendCAN(1,nCmd.cmd,0,NULL);
                if (mesh_form_pkt(&pkt, CAN_WIRELESS_NODE, mesh_pkt_ack, 1, 0))
                {
                    pkt.info.data_len = 1;
                    pkt.data[0] = nCmd.cmd;
                    if (wireless_send_formed_pkt(&pkt))
                    {
                        //         printf("reset compass\n");
                    }
                }
            }
                break;

            case kFilter_on:
            {
                //  FormPacketAndSendCAN(1,nCmd.cmd,0,NULL);
                if (mesh_form_pkt(&pkt, CAN_WIRELESS_NODE, mesh_pkt_ack, 1, 0))
                {
                    pkt.info.data_len = 1;
                    pkt.data[0] = nCmd.cmd;
                    if (wireless_send_formed_pkt(&pkt))
                    {
                        //         printf("reset compass\n");
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
    static int resetCount = 0;
    cmd_data nCmd = { 0 };

#if NODE_CAN
    can_msg_t msg = {0};
    msg.data.qword = 0;
    msg.frame_fields.data_len =0;
    msg.msg_id = heartbeat;

    if( CAN_tx(can1, &msg, 0))
    {
        resetCount = 0;
        return 1;
    }
    else
    {
        if(resetCount > 10)
        {
            sys_reboot();
        }
        resetCount++;
        //  printf("Heart Beat failed\n");
    }

#else
    if (Switches::getInstance().getSwitch(1))
    {
        nCmd.cmd = reset_compass;
        xQueueSend(GPSdataTxQueue, &nCmd, 0);
    }

    if (Switches::getInstance().getSwitch(2))
    {
        nCmd.cmd = start;
        xQueueSend(GPSdataTxQueue, &nCmd, 0);
    }
    if (Switches::getInstance().getSwitch(3))
    {
        nCmd.cmd = kFilter_on;
        kalmanOFF = 1;
        xQueueSend(GPSdataTxQueue, &nCmd, 0);
    }

    if (Switches::getInstance().getSwitch(4))
    {
        nCmd.cmd = stop;
        xQueueSend(GPSdataTxQueue, &nCmd, 0);
    }

#endif
}

bool GPS_SendDataToTxQueue()
{
    int i = 0;
    cmd_data nCmd =
    { 0 };

    if (xSemaphoreTake(GPSDataTxSem, 0))
    {
        do
        {
            nCmd.cmd = check;
            nCmd.index = i;
            xQueueSend(GPSdataTxQueue, &nCmd, 0);

        } while (!gGpsData[i++].bDestination);

        return 1;
    }
    else
    {
        return 0;
    }
}

bool ParseGPSData(char *cmdParams, gps_data *pGps, int count, int checkCount)
{
    int b1;
    uint32_t b2;

    switch (count)
    {
        case 0:
        {
            sscanf(cmdParams, "%u.%6d", &b1, &b2);
            b1 = abs(b1);

            pGps->longLatdata.lattitude_dec = b1;
            pGps->longLatdata.lattitude_float = b2;

        }
            break;

        case 1:
        {
            sscanf(cmdParams, "%u.%6d", &b1, &b2);

            b1 = abs(b1);
            pGps->longLatdata.longitude_dec = b1;
            pGps->longLatdata.longitude_float = b2;


        }
            break;

        case 2:
        {
            sscanf(cmdParams, "%d", &b1);
            pGps->longLatdata.checkpoint = b1;
        }
            break;

        case 3:
        {
            sscanf(cmdParams, "%d", &b1);
            pGps->longLatdata.bIsFinal = b1;
            pGps->bDestination = b1;

            /* checks if the checkpoints are in order and no data is missing  */
            if (pGps->longLatdata.checkpoint == (count))
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
    char *cmd = strtok(pData, " ");
    char *cmdParam;
    int i = 0;
    bool bSendMsg = 0;
    static int count = 0;
    gps_data gps ={ 0 };
    cmd_data nCmd = { 0 };

    if (!strcmp(cmd, "start"))
    {

        nCmd.cmd = start;
        xQueueSend(GPSdataTxQueue, &nCmd, 0);
        return 1;
    }

    if (!strcmp(cmd, "init"))
    {
        //not needed currently
        printf("%s ", cmd);
        return 1;
    }

    if (!strcmp(cmd, "check") && !sendStatusFlag)
    {
        while ((cmdParam = strtok(NULL, " ")) != NULL)
        {

            bSendMsg = ParseGPSData(cmdParam, &gps, i++, count);

            if (bSendMsg == 1)
            {
                memcpy(&gGpsData[count++], &gps, sizeof(gps_data));

                /* if the final destination is received send a message to transmit
                 * all the checkpoint data
                 */
                if (gps.bDestination)
                {
                    sendStatusFlag = 1;
                    count = 0;
                    xSemaphoreGive(GPSDataTxSem); // semaphore to txt all the checkpoint data from 1Khz periodic task
                    return 1;
                }

                return 1;
            }
            else if (bSendMsg == 2) // data not in order
            {
                count = 0;
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
    while (Uart2::getInstance().getChar(&c, 0));
    gChrCnt = 0;
}

void getDataFromBluetooth()
{
    static Uart2 &u2 = Uart2::getInstance();
    char c;

    if (u2.getChar(&c, 0))
    {
        //  printf("%c\n",c);
        if (c == '$')
        {
            gData[gChrCnt] = 0;
            gChrCnt = 0;
            printf("%s\n", gData);
            if (!ProcessDataAndSend(gData))
            {
                Uart2_flushData();
                printf(" data not parsed\n");
            }
        }
        else
        {
            gData[gChrCnt] = c;
            gChrCnt++;
        }
    }

}

void bridge_canTx()
{
    can_msg_t canData ={ 0 };
    cmd_data nCmd ={ 0 };
    int i = 0;

    if (xQueueReceive(GPSdataTxQueue, &nCmd, 0))
    {
        switch (nCmd.cmd)
        {
            case check:
            {
                canData.frame_fields.data_len = 8;
                canData.msg_id = location; // sending latitude  to master
                i = nCmd.index;

#if 1
                printf("lat:%u.%d,long:%u.%d,chk:%d,dest:%d \n",
                        gGpsData[i].longLatdata.lattitude_dec,
                        gGpsData[i].longLatdata.lattitude_float,
                        gGpsData[i].longLatdata.longitude_dec,
                        gGpsData[i].longLatdata.longitude_float,
                        gGpsData[i].longLatdata.checkpoint,
                        gGpsData[i].longLatdata.bIsFinal);
#endif
                canData.data.qword = gGpsData[i].longLatdata.qWord;

                if (CAN_tx(can1, &canData, 0))
                {
                    printf("CAN_Tx:data txted\n");
                }
                else
                {
                    printf("CAN_Tx failed\n");
                }

                if (gGpsData[i].bDestination)
                {
                    //Uart2_flushData();
                    sendStatusFlag = 0;
                }

            }
                break;
            case stop:
            case start:
            {
                canData.frame_fields.data_len = 1;
                if( nCmd.cmd == start)
                    canData.msg_id = drive_mode;
                else
                    canData.msg_id = stop_mode;

                canData.data.bytes[0] = 1;

                if (CAN_tx(can1, &canData, 0))
                {
                    nCmd.cmd = data_ack;
                    xQueueSend(GPSdataRxQueue, &nCmd, 0);
                }
                else
                {
                    printf("start failed:can tx\n");
                }
#if 0
                else if(!CAN_is_bus_off(can1))
                {
                    printf("sending start over can failed!!!");
                    /*retransmit*/
                    xQueueSend(GPSdataTxQueue,&nCmd,0);
                }
#endif
            }
            break;

            case reset_compass:
            {
                canData.frame_fields.data_len = 1;
                canData.msg_id = reset_compassId;
                canData.data.bytes[0] = 1;
                if (CAN_tx(can1, &canData, 0))
                {
                    // do nothing

                }
            }
            break;
            case kFilter_on:
            {
                canData.frame_fields.data_len = 1;
                canData.msg_id = kFilter_id;
                canData.data.bytes[0] = 1;
                if (CAN_tx(can1, &canData, 0))
                {

                    // do nothing

                }
            }
                break;

        }

    }

}

bool Send_Uart2(char *p)
{
    Uart2::getInstance().put(p, 100);
    return 1;
}

bool SendDataOverBluetooth(void)
{
    cmd_data nCmd = { 0 };
    char *p = rxData;

    if (xQueueReceive(GPSdataRxQueue, &nCmd, 0))
    {
        switch (nCmd.cmd)
        {
            case data_ack:
            {
                sprintf(p, "a~");
                printf("received: %s\n",p);
                Send_Uart2(p);
            }
             break;
            case data_loc:
            {
               sprintf(p, "l%f -%f~",lat,longitude);
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

void bridge_canRx10Hhz()
{
    can_fullcan_msg_t msg = { 0 };
    uint16_t can_id[5] ={ gps_ack, reset };
    cmd_data nCmd = { 0 };
    int count = 0;

    for (count = 0; count < 2; count++)
    {
        if (getCanData(&msg, can_id[count]))
        {
            switch (msg.msg_id)
            {
                case gps_ack:
                {
                    nCmd.cmd = data_ack;
                    xQueueSend(GPSdataRxQueue, &nCmd, 0);
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

            }
        }
        else
        {
            //printf("CAN RX failed!!\n");
        }
    }

}

void bridge_canRx1Hhz()
{
    static int countGps = 0;
    can_fullcan_msg_t msg ={ 0 };
    uint16_t can_id[5] ={ gps_loc, compass_id, distance_id };
    cmd_data nCmd = { 0 };
    int count = 0;

    for (count = 0; count < 3; count++)
    {
        if (getCanData(&msg, can_id[count]))
        {
            switch (msg.msg_id)
            {

                case gps_loc:
                {
                    int i = countGps % 10;
                    gGpsDataRx[i].cmd = data_loc;
                    gGpsDataRx[i].longLatdata.qWord = msg.data.qword;
                    gGpsDataRx[i].bDestination =
                    gGpsDataRx[i].longLatdata.bIsFinal;
                    nCmd.cmd = data_loc;
                    nCmd.index = i;
                    countGps++;
                    xQueueSend(GPSdataRxQueue, &nCmd, 0);

                }
                    break;

                case compass_id:
                {
                    nCmd.cmd = compass_data;
                    desired = msg.data.words[0];
                    actual = msg.data.words[1];
                    xQueueSend(GPSdataRxQueue, &nCmd, 0);
                }
                    break;

                case distance_id:
                {
                    nCmd.cmd = distance_data;
                    nCmd.index = 0;
                    gDistanceInfo[0].compassData = msg.data.qword;
                    xQueueSend(GPSdataRxQueue, &nCmd, 0);

                }
                    break;
            }
        }
        else
        {
            //printf("CAN RX failed!!\n");
        }
    }

}

