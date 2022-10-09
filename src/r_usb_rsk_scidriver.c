/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014(2020) Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/*******************************************************************************
* File Name    : r_usb_rsk_scidriver.c
* Description  : RX64M RSK SCI sample driver
******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 31.03.2018 1.23 Using Pin setting API.
 *         : 01.03.2020 1.30 RX72N/RX66N is added and uITRON is supported.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_pcdc_apl.h"
#include "r_usb_pcdc_apl_config.h"

#if OPERATION_MODE == USB_UART
#include "r_sci_rx_if.h"
#include "r_usb_rsk_sci.h"
#include "r_sci_rx_pinset.h"

/******************************************************************************
Constant macro definitions
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* global variable prototype definition */
extern sci_hdl_t   g_console;
static usb_serial_state_bitmap_t  cdc_serial_state;

static void         cdc_uart_callback(void *pArgs);
static void R_SCI_PinClear_SCI(void);

/*=== Serial port ===========================================================*/

/******************************************************************************
Renesas Abstracted RSK functions
******************************************************************************/

/******************************************************************************
Serial Port function
******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_Sci_Init
Description     : SCI Pin function setting
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_Sci_Init(void)
{
    usb_pcdc_linecoding_t com_para;
    uint8_t *data_rate;
    
    data_rate = (uint8_t *)&com_para.dw_dte_rate;
    data_rate[0] = 0x80;      /* 9600bps = 0x2580 */
    data_rate[1] = 0x25;
    data_rate[2] = 0x00;
    data_rate[3] = 0x00;
    com_para.b_data_bits   = USB_SCI_DATA_BIT_8;
    com_para.b_parity_type = USB_SCI_PARITY_BIT_NONE;
    com_para.b_char_format = USB_SCI_STOP_BIT_1;
    usb_cpu_Sci_Set1((usb_pcdc_linecoding_t *)&com_para);
}   /* eof usb_cpu_Sci_Init() */


/******************************************************************************
Function Name   : usb_cpu_Sci_Set1
Description     : Line coding set
Arguments       : data_rate   : Data terminal rate in bits per second
                : stop_bit    : Stop bits 0-1Stopbit 1-1.5Stopbit 2-2Stopbit
                : parity      : Parity 0-None 1-Odd 2-Even 3-Mark 4-Spase
                : data_bit    : Data bits(5,6,7,8 or 16)
Return value    : Error code  : Error code
******************************************************************************/
uint16_t usb_cpu_Sci_Set1(usb_pcdc_linecoding_t *p_com_para)
{
    uint16_t    ret_code = USB_OK;
    uint32_t    data_rate;
    sci_uart_t  config;
    sci_err_t   sci_err;
    uint8_t     *data_rate_top;

    if( USB_NULL != g_console )
    {
        R_SCI_PinClear_SCI();
        sci_err = R_SCI_Close( g_console );
        if( SCI_SUCCESS != sci_err )
        {
            /* Send Message failure */
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_APL | USB_DEBUG_HOOK_CODE7);
            ret_code = USB_ERROR;
        }
    }

    if( USB_OK == ret_code )
    {
        data_rate_top = (uint8_t *)&p_com_para->dw_dte_rate;
        /* DATA RATE Set */
        data_rate = (uint32_t)data_rate_top[0];
        data_rate |= (uint32_t)data_rate_top[1] << 8;
        data_rate |= (uint32_t)data_rate_top[2] << 16;
        data_rate |= (uint32_t)data_rate_top[3] << 24;

        /* DATA RATE Set */
        config.baud_rate = data_rate;
        /* use internal clock for baud generation */
        config.clk_src = SCI_CLK_INT;

        /* CHARACTER LENGTH Setting */
        if (USB_SCI_DATA_BIT_8 == p_com_para->b_data_bits)
        {
            config.data_size = SCI_DATA_8BIT;
        }
        else
        {
            config.data_size = SCI_DATA_7BIT;
        }

        /* PARITY BIT Setting */
        if (USB_SCI_PARITY_BIT_ODD == p_com_para->b_parity_type)
        {
            config.parity_en = SCI_PARITY_ON;
            config.parity_type = SCI_ODD_PARITY;
        }
        else if (USB_SCI_PARITY_BIT_EVEN == p_com_para->b_parity_type)
        {
            config.parity_en = SCI_PARITY_ON;
            config.parity_type = SCI_EVEN_PARITY;
        }
        else
        {
            config.parity_en = SCI_PARITY_OFF;
        }

        /* STOP BIT Setting */
        if (USB_SCI_STOP_BIT_1 == p_com_para->b_char_format)
        {
            config.stop_bits = SCI_STOPBITS_1;
        }
        else
        {
            config.stop_bits = SCI_STOPBITS_2;
        }

        config.int_priority = 2;            /* 1=lowest, 15=highest */
        /* Open channel for Async Mode operation */
        sci_err = R_SCI_Open( USB_PSMPL_USE_SCI, SCI_MODE_ASYNC, (void *)&config, cdc_uart_callback, &g_console );
        if( SCI_SUCCESS != sci_err )
        {
            /* Send Message failure */
            USB_DEBUG_HOOK( USB_DEBUG_HOOK_APL | USB_DEBUG_HOOK_CODE8 );
            ret_code = USB_ERROR;
        }
        else
        {
            sci_err = R_SCI_Control( g_console, SCI_CMD_EN_TEI, NULL );
            if( SCI_SUCCESS != sci_err )
            {
                /* Send Message failure */
                USB_DEBUG_HOOK( USB_DEBUG_HOOK_APL | USB_DEBUG_HOOK_CODE9 );
                ret_code = USB_ERROR;
            }
            else
            {
                R_SCI_Control (g_console, SCI_CMD_EN_NOISE_CANCEL, NULL);
                R_SCI_PinSet_SCI();
            }
        }
    }
    return ret_code;
}   /* eof usb_cpu_Sci_Set1() */

/*****************************************************************************
* Function Name: cdc_uart_callback
* Description  : SCI Async Mode callback function.
* Arguments    : pArgs -
*                    ptr to sci_cb_args_t structure casted to a void. Structure
*                    contains event and associated data.
* Return Value : none
******************************************************************************/
static void cdc_uart_callback(void *pArgs)
{
    sci_cb_args_t    *args;
    uint16_t        status_old,status_new;

    args = (sci_cb_args_t *)pArgs;
    status_old = *(uint16_t *)&cdc_serial_state;
    if( SCI_EVT_RX_CHAR == args->event )
    {
        /* from RXI interrupt; received character   */
        /* char is in args->byte    */
        cdc_serial_state.BIT.bover_run = USB_PSMPL_ERROR_CLEAR;      /* Over Run error clear */
        cdc_serial_state.BIT.bframing = USB_PSMPL_ERROR_CLEAR;      /* Framing error clear */
        cdc_serial_state.BIT.bparity = USB_PSMPL_ERROR_CLEAR;       /* Framing error clear */
    }
#if SCI_CFG_TEI_INCLUDED
    else if( SCI_EVT_TEI == args->event )
    {   
        /* from TEI interrupt; xmt is idle  */
        /* possibly disable external xcvr here  */
        nop();
    }
#endif
    else if( SCI_EVT_RXBUF_OVFL == args->event )
    {
        /* from RXI interrupt; rx queue is full */
        /* "lost" char is in args->byte */
        /* will need to increase buffer size or reduce baud rate    */
        cdc_serial_state.BIT.bover_run = USB_PSMPL_ERROR_SET;      /* Over Run error */
    }
    else if( SCI_EVT_OVFL_ERR == args->event )
    {
        /* from GROUP12 interrupt; receiver overflow error  */
        /* error char is in args->byte  */
        /* error condition is cleared in calling interrupt routine  */
        cdc_serial_state.BIT.bover_run = USB_PSMPL_ERROR_SET;      /* Over Run error */
    }
    else if( SCI_EVT_FRAMING_ERR == args->event )
    {
        /* from GROUP12 interrupt; receiver framing error   */
        /* error char is in args->byte  */
        /* error condition is cleared in calling interrupt routine  */
        cdc_serial_state.BIT.bframing = USB_PSMPL_ERROR_SET;      /* Framing error */
    }
    else if( SCI_EVT_PARITY_ERR == args->event )
    {
        /* from GROUP12 interrupt; receiver parity error    */
        /* error char is in args->byte  */
        /* error condition is cleared in calling interrupt routine  */
        cdc_serial_state.BIT.bparity = USB_PSMPL_ERROR_SET;       /* Framing error */
    }
    else
    {
    }

    status_new = *(uint16_t *)&cdc_serial_state;
    if( status_new != status_old )
    {
        g_serial_state[0] = (uint8_t)(status_new & 0xff);
        g_serial_state[1] = (uint8_t)(status_new >> 8);
        g_is_serial_state = 1;         /* Set Event(USB receive complete)  */
    }
}   /* eof cdc_uart_callback() */

/***********************************************************************************************************************
* Function Name: R_SCI_PinClear_SCI
* Description  : This function initializes pins for r_sci_rx module
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
static void R_SCI_PinClear_SCI(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Clear RXD pin */
    RXD_PDR = 0U;
    RXD_PMR = 0U;
    RXD_PFS = 0x00U;

    /* Clear TXD pin */
    TXD_PODR = 1U;
    TXD_PDR  = 1U;
    TXD_PMR  = 0U;
    TXD_PFS  = 0x00U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

#endif /* OPERATION_MODE == USB_UART */
/******************************************************************************
End  Of File
******************************************************************************/
