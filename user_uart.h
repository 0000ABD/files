/*
 * user_uart.h
 *
 *  Created on: Jun 16, 2024
 *      Author: win10
 */

#ifndef USER_UART_H_
#define USER_UART_H_

#include "hal_data.h"

#define UART0_BAUDRATE 230400U
#define UART2_BAUDRATE 230400U

#define READ_CMD_REQUEST    1U
#define POSITIVE_ACK        2U
#define NEGATIVE_ACK        3U

typedef enum
{
    SEND_HEADER = 0,
    WAIT_FOR_SEND_HEADER_COMPLETE,
    WAIT_FOR_HEADER_ACK,
    VALIDATE_HEADER_ACK,
    SEND_DATA,
    WAIT_FOR_SEND_DATA_COMPLETE,
    WAIT_FOR_DATA_ACK,
    VALIDATE_DATA_ACK,
    RECEIVE_HEADER,
    SEND_HEADER_ACK,
    WAIT_FOR_SEND_HEADER_ACK_COMPLETE,
    DATA_SEND_COMPLETE,
    RECEIVE_DATA,
    WAIT_FOR_DATA_RECEIVE_COMPLETE,
    SEND_DATA_ACK,
    WAIT_FOR_SEND_DATA_ACK_COMPLETE,
    DATA_RECEIVE_COMPLETE
}e_uart_comm_state_t;


typedef struct
{
    uint32_t u32_crc;
    union
    {
        uint8_t u8ar_header_buff[6];
        struct
        {
            uint32_t u32_len;
            uint8_t  u8_roll_cnt;
            uint8_t u8_type;

        };
    };
}st_uart_comm_header_t;

typedef struct
{
    st_uart_comm_header_t *pst_comm_header;
    uint8_t *pu8_data_buff;
    e_uart_comm_state_t e_curr_state;
}st_uart_comm_handle_t;


void uart0_init(void);
void uart2_init(void);
void uart0_task(st_uart_comm_handle_t *pst_handle);
void uart2_task(st_uart_comm_handle_t *pst_handle);

#endif /* USER_UART_H_ */
