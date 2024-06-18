/*
 * user_uart.c
 *
 *  Created on: Jun 16, 2024
 *      Author: win10
 */

#include "hal_data.h"
#include "user_uart.h"
#include <string.h>
#include <stdlib.h>

static void uart0_appl_callback(uart_callback_args_t *pst_callback_parm);
static void uart2_appl_callback(uart_callback_args_t *pst_callback_parm);


void uart0_init(void)
{
    fsp_err_t status;
    g_uart0_cfg.p_callback = uart0_appl_callback;
    status =  R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    if(FSP_SUCCESS != status)
    {
        //TODO : handle the failure
    }
    else
    {
        status = R_SCI_UART_BaudCalculate(UART0_BAUDRATE, true, 0,&g_uart0_baud_setting);
        /*update baudrare*/
       if(  FSP_SUCCESS == status)
       {
           status = R_SCI_UART_BaudSet(&g_uart0_ctrl, &g_uart0_cfg,g_uart0_baud_setting);
           if(FSP_SUCCESS != status)
           {
               //TODO : handle the failure
           }
           else
           {

           }

       }
       else
       {
           //TODO : handle the failure
       }

    }

}


void uart2_init(void)
{
    fsp_err_t status;
    g_uart2_cfg.p_callback = uart2_appl_callback;
    status =  R_SCI_UART_Open(&g_uart2_ctrl, &g_uart2_cfg);
    if(FSP_SUCCESS != status)
    {
        //TODO : handle the failure
    }
    else
    {
        status = R_SCI_UART_BaudCalculate(UART2_BAUDRATE, true, 0,&g_uart2_baud_setting);
        /*update baudrare*/
        if(  FSP_SUCCESS == status)
        {
            status = R_SCI_UART_BaudSet(&g_uart2_ctrl, &g_uart2_cfg,g_uart2_baud_setting);
            if(FSP_SUCCESS != status)
            {
                //TODO : handle the failure
            }
            else
            {

            }

        }
        else
        {
            //TODO : handle the failure
        }

        /*do nothing*/
    }
}

static e_uart_comm_state_t *e_uart0_comm_state = NULL;
static void uart0_appl_callback(uart_callback_args_t *pst_callback_parm)
{
    switch(pst_callback_parm->event)
    {
        case UART_EVENT_RX_COMPLETE   : ///< Receive complete event

            if( WAIT_FOR_HEADER_ACK == *e_uart0_comm_state )
            {
                *e_uart0_comm_state = VALIDATE_HEADER_ACK;
            }
            else if(WAIT_FOR_DATA_ACK == *e_uart0_comm_state )
            {
                *e_uart0_comm_state = VALIDATE_DATA_ACK;
            }
            else if(RECEIVE_HEADER == *e_uart0_comm_state )
            {
                *e_uart0_comm_state = SEND_HEADER_ACK;
            }
            else if(WAIT_FOR_DATA_RECEIVE_COMPLETE == *e_uart0_comm_state)
            {
                *e_uart0_comm_state = SEND_DATA_ACK;
            }
            else
            {
                //do nothing
            }

         break;

        case   UART_EVENT_TX_COMPLETE :///< Transmit complete event
         if( WAIT_FOR_SEND_HEADER_COMPLETE == *e_uart0_comm_state )
         {
             *e_uart0_comm_state = WAIT_FOR_HEADER_ACK;
         }
         else if(WAIT_FOR_SEND_DATA_COMPLETE == *e_uart0_comm_state )
         {
             *e_uart0_comm_state = WAIT_FOR_DATA_ACK;
         }
         else if(WAIT_FOR_SEND_HEADER_ACK_COMPLETE == *e_uart0_comm_state)
         {
             *e_uart0_comm_state = RECEIVE_DATA;
         }
         else if(WAIT_FOR_SEND_DATA_ACK_COMPLETE == *e_uart0_comm_state)
         {
             *e_uart0_comm_state = DATA_RECEIVE_COMPLETE;
         }
         else
         {
             //do nothing
         }
         break;
        case  UART_EVENT_RX_CHAR      : ///< Character received
        case  UART_EVENT_ERR_PARITY  : ///< Parity error event
        case  UART_EVENT_ERR_FRAMING  :///< Mode fault error event
        case  UART_EVENT_ERR_OVERFLOW :///< FIFO Overflow error event
        case UART_EVENT_BREAK_DETECT: ///< Break detect error event
        case  UART_EVENT_TX_DATA_EMPTY : ///< Last byte is transmitting, ready for more data
    }
}

static e_uart_comm_state_t *e_uart2_comm_state = NULL;
static void uart2_appl_callback(uart_callback_args_t *pst_callback_parm)
{

}

void uart0_task(st_uart_comm_handle_t *pst_handle)
{

    static st_uart_comm_header_t st_header_for_ack;

    if(NULL != pst_handle && NULL != pst_handle->pst_comm_header && NULL != pst_handle->pu8_data_buff)
    {
        e_uart0_comm_state =  &pst_handle->e_curr_state;

        switch(pst_handle->e_curr_state)
        {
            case SEND_HEADER:

                if(FSP_SUCCESS ==  R_SCI_UART_Write(&g_uart0_ctrl,pst_handle->pst_comm_header,sizeof(st_uart_comm_header_t)))
                {
                    pst_handle->e_curr_state = WAIT_FOR_SEND_HEADER_COMPLETE;
                }
                else
                {
                    /*do nothing*/
                }
                break;
            case WAIT_FOR_SEND_HEADER_COMPLETE:
                break;

            case WAIT_FOR_HEADER_ACK:
                if(  0 == g_uart0_ctrl.rx_transfer_in_progress)
                {
                    memset(&st_header_for_ack,0x00, sizeof(st_header_for_ack));
                    if(  FSP_SUCCESS != R_SCI_UART_Read(&g_uart0_ctrl, &st_header_for_ack,sizeof(st_header_for_ack)))
                    {
                        //TODO :  Handle failure
                    }
                    else
                    {
                        //do nothing
                    }
                }
                else
                {
                    //do nothing
                }
                break;

            case VALIDATE_HEADER_ACK:
                if( st_header_for_ack.u32_crc == get_crc(st_header_for_ack.u8ar_header_buff,sizeof(st_header_for_ack.u8ar_header_buff)) && POSITIVE_ACK == st_header_for_ack.u8_type )
                {
                    pst_handle->e_curr_state = SEND_DATA;
                }
                else
                {
                    pst_handle->e_curr_state = WAIT_FOR_HEADER_ACK;
                }
                break;
            case SEND_DATA:
                if( FSP_SUCCESS != R_SCI_UART_Read(&g_uart0_ctrl,pst_handle->pu8_data_buff,pst_handle->pst_comm_header->u32_len))
                {
                    //TODO : Handle failure
                }
                else
                {
                    pst_handle->e_curr_state = WAIT_FOR_SEND_DATA_COMPLETE;
                }
                break;
            case WAIT_FOR_SEND_DATA_COMPLETE:
                break;
            case WAIT_FOR_DATA_ACK:
                if(  0 == g_uart0_ctrl.rx_transfer_in_progress)
                {
                    memset(&st_header_for_ack,0x00, sizeof(st_header_for_ack));
                    if(  FSP_SUCCESS != R_SCI_UART_Read(&g_uart0_ctrl, &st_header_for_ack,sizeof(st_header_for_ack)))
                    {
                        //TODO :  Handle failure
                    }
                    else
                    {
                        //do nothing
                    }
                }
                else
                {
                    //do nothing
                }
                break;
            case VALIDATE_DATA_ACK:
                if( st_header_for_ack.u32_crc == get_crc(st_header_for_ack.u8ar_header_buff,sizeof(st_header_for_ack.u8ar_header_buff)) && POSITIVE_ACK == st_header_for_ack.u8_type )
                {
                    //mark data transmit completed
                   pst_handle->e_curr_state = DATA_SEND_COMPLETE;
                }
                else
                {
                    pst_handle->e_curr_state = WAIT_FOR_DATA_ACK;
                }
                break;
            case RECEIVE_HEADER:
                if(  0 == g_uart0_ctrl.rx_transfer_in_progress)
                {
                    memset(pst_handle->pst_comm_header,0x00,sizeof(st_uart_comm_header_t));
                    /*get the header and retain until full data is validated*/
                    if(  FSP_SUCCESS != R_SCI_UART_Read(&g_uart0_ctrl,pst_handle->pst_comm_header,sizeof(st_uart_comm_header_t)))
                    {
                        //TODO :  Handle failure
                    }
                    else
                    {

                    }
                }
                else
                {

                }
                break;
            case SEND_HEADER_ACK:
                memset(&st_header_for_ack,0x00,sizeof(st_header_for_ack));
                st_header_for_ack.u32_len = 2;
                if(READ_CMD_REQUEST == pst_handle->pst_comm_header->u8_type )
                {

                    st_header_for_ack.u8_type = POSITIVE_ACK;
                }
                else
                {
                    st_header_for_ack.u8_type = NEGATIVE_ACK;
                }
                st_header_for_ack.u32_crc = get_crc(st_header_for_ack.u8ar_header_buff,sizeof(st_header_for_ack.u8ar_header_buff));
                if(FSP_SUCCESS != R_SCI_UART_Write(&g_uart0_ctrl,&st_header_for_ack,sizeof(st_header_for_ack)))
                {
                    pst_handle->e_curr_state = RECEIVE_HEADER;
                }
                else
                {
                    pst_handle->e_curr_state = WAIT_FOR_SEND_HEADER_ACK_COMPLETE;
                }
                break;
            case WAIT_FOR_SEND_HEADER_ACK_COMPLETE:
                break;
            case RECEIVE_DATA:
                if(  0 == g_uart0_ctrl.rx_transfer_in_progress)
                {
                    memset(pst_handle->pu8_data_buff,0x00,sizeof(pst_handle->pu8_data_buff));
                    if(FSP_SUCCESS != R_SCI_UART_Read(&g_uart0_ctrl,pst_handle->pu8_data_buff,sizeof(pst_handle->pst_comm_header->u32_len)))
                    {
                        //TODO : handle failure
                    }
                    else
                    {
                        pst_handle->e_curr_state = WAIT_FOR_DATA_RECEIVE_COMPLETE;
                    }
                }
                else
                {
                    //do nothing
                }
                break;
            case WAIT_FOR_DATA_RECEIVE_COMPLETE:
                break;
            case SEND_DATA_ACK:
                st_header_for_ack.u32_len = 2;
                if(pst_handle->pst_comm_header->u32_crc == get_crc(pst_handle->pu8_data_buff,pst_handle->pst_comm_header->u32_len))
                {
                    st_header_for_ack.u8_type = POSITIVE_ACK;
                }
                else
                {
                    st_header_for_ack.u8_type = NEGATIVE_ACK;
                }
                st_header_for_ack.u32_crc = get_crc(st_header_for_ack.u8ar_header_buff,sizeof(st_header_for_ack.u8ar_header_buff));

                if(FSP_SUCCESS != R_SCI_UART_Write(&g_uart0_ctrl,&st_header_for_ack,sizeof(st_header_for_ack)))
                {
                    //TODO : handle failure
                }
                else
                {
                    pst_handle->e_curr_state = WAIT_FOR_SEND_DATA_ACK_COMPLETE;
                }
                break;
            case WAIT_FOR_SEND_DATA_ACK_COMPLETE:
                break;
            case DATA_RECEIVE_COMPLETE:
            default:;


        }
    }
    else
    {

    }
}


void uart2_task(st_uart_comm_handle_t *pst_handle)
{

    if(NULL != pst_handle && NULL != pst_handle->pst_comm_header && NULL != pst_handle->pu8_data_buff)
    {

    }
    else
    {

    }
}
