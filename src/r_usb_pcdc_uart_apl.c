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
 * File Name    : r_usb_pcdc_uart_apl.c
 * Description  : USB Peripheral Communications Devices Class Sample Code
 *******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 30.09.2017 1.22 "g_is_usb_bulk_writing","g_is_usb_interrupt_writing" is set "USB_NO" for USB Configured.
 *         : 31.03.2018 1.23 Using Pin setting API.
 *                           Num String descriptor is added.
 *         : 01.03.2020 1.30 RX72N/RX66N is added and uITRON is supported.
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_pcdc_apl.h"
#include "r_usb_pcdc_apl_config.h"

#if USB_SUPPORT_LPW == USB_APL_ENABLE
#include "r_usb_rsk_lowpower.h"
#endif /* USB_SUPPORT_LPW == USB_APL_ENABLE */

#if USB_SUPPORT_RTOS == USB_APL_ENABLE
#include "r_usb_rtos_apl.h"
#include "r_rtos_abstract.h"
#if (BSP_CFG_RTOS_USED == 4)        /* Renesas RI600V4 & RI600PX */
#include "kernel_id.h"
#endif /* (BSP_CFG_RTOS_USED == 4) */
#endif /* USB_SUPPORT_RTOS == USB_APL_ENABLE */

#if OPERATION_MODE == USB_UART
#include "r_sci_rx_if.h"
#include "r_usb_rsk_sci.h"

/******************************************************************************
 Macro definitions
 ***************************************************************************#***/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static  void        apl_init(void);

static  void        usb_pin_setting (void);
static  uint8_t     g_usb_data[DATA_LEN];
static  uint8_t     g_sci_data[DATA_LEN];

static  uint32_t    g_read_size;
static  uint8_t     g_is_usb_bulk_writing;
static  uint8_t     g_is_usb_interrupt_writing;
static  usb_pcdc_linecoding_t g_line_coding;

#if (BSP_CFG_RTOS_USED != 0)        /* Use RTOS */
static  rtos_mbx_id_t   g_usb_apl_mbx_id;
#endif /* (BSP_CFG_RTOS_USED != 0) */

const static usb_descriptor_t usb_descriptor =
{   
    g_apl_device,                   /* Pointer to the device descriptor */
    g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
    USB_NULL,                       /* Pointer to the configuration descriptor for Hi-speed */
    USB_NULL,                       /* Pointer to the qualifier descriptor */
    g_apl_string_table,             /* Pointer to the string descriptor table */
    NUM_STRING_DESCRIPTOR
};

uint8_t     g_is_serial_state;
uint8_t     g_serial_state[2];
uint8_t     g_sci_counter;
sci_hdl_t   g_console;

/******************************************************************************
Exported global functions (to be accessed by other files)
 ******************************************************************************/
extern void    R_USB_PinSet_USB0_PERI(void);
extern void    R_USB_PinSet_USBA_PERI(void);

/******************************************************************************
 Renesas Peripheral Communications Devices Class Sample Code functions
 ******************************************************************************/

#if USB_SUPPORT_RTOS == USB_APL_ENABLE
/******************************************************************************
 Function Name   : usb_apl_callback
 Description     : Callback function for Application program
 Arguments       : usb_ctrl_t *p_ctrl   : Control structure for USB API.
                   rtos_task_id_t  cur_task  : Task Handle
                   uint8_t    usb_state : USB_ON(USB_STS_REQUEST) / USB_OFF
 Return value    : none
 ******************************************************************************/
void usb_apl_callback (usb_ctrl_t *p_ctrl, rtos_task_id_t cur_task, uint8_t usb_state)
{
    rtos_send_mailbox(&g_usb_apl_mbx_id, (void *)p_ctrl);
} /* End of function usb_apl_callback */
#endif /* USB_SUPPORT_RTOS == USB_APL_ENABLE */

/******************************************************************************
 Function Name   : usb_main
 Description     : Peripheral CDC application main process
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_main (void)
{
    usb_cfg_t   cfg;
    uint16_t    size;
    usb_err_t   err;
    sci_err_t   sci_err;
    usb_ctrl_t  ctrl;
#if (BSP_CFG_RTOS_USED != 0)                                /* Use RTOS */
    usb_ctrl_t  *p_mess;
    rtos_err_t  ret;
#if (BSP_CFG_RTOS_USED == 1)                                /* FreeRTOS */
    rtos_mbx_info_t     mbx_info;
#endif /* (BSP_CFG_RTOS_USED == 1) */
#endif /* (BSP_CFG_RTOS_USED != 0) */

#if (BSP_CFG_RTOS_USED == 1)                                /* FreeRTOS */
    mbx_info.length = USB_APL_QUEUE_SIZE;
    rtos_create_mailbox(&g_usb_apl_mbx_id, &mbx_info);      /* For APL */
#elif (BSP_CFG_RTOS_USED == 4)                              /* Renesas RI600V4 & RI600PX */
    g_usb_apl_mbx_id = ID_USB_APL_MBX;
#endif /* (BSP_CFG_RTOS_USED == 1) */

    apl_init();                 /* Application program initialization */
    usb_pin_setting();          /* USB MCU pin setting */

    USB_RSK_SCI_INIT();

    ctrl.module     = USE_USBIP;
    ctrl.type       = USB_PCDC;
    cfg.usb_speed   = USB_SUPPORT_SPEED; /* USB_HS/USB_FS */
    cfg.usb_mode    = USB_PERI;
    cfg.p_usb_reg   = (usb_descriptor_t *)&usb_descriptor;
    R_USB_Open(&ctrl, &cfg); /* Initializes the USB module */

#if USB_SUPPORT_RTOS == USB_APL_ENABLE
    R_USB_Callback(usb_apl_callback);
#endif /* USB_SUPPORT_RTOS == USB_APL_ENABLE */

    /* Loop back between PC(TerminalSoft) and USB MCU */
    while (1)
    {
#if USB_SUPPORT_RTOS == USB_APL_ENABLE

        ret = rtos_receive_mailbox(&g_usb_apl_mbx_id, (void **)&p_mess, RTOS_ZERO);
        if (RTOS_SUCCESS == ret)
        {
        ctrl = *p_mess;
        /* Analyzing the received message */
        switch (ctrl.event)
#else   /* USB_SUPPORT_RTOS == USB_APL_ENABLE */
        switch (R_USB_GetEvent(&ctrl))
#endif  /* USB_SUPPORT_RTOS == USB_APL_ENABLE */
        {
            case USB_STS_CONFIGURED :
                g_is_usb_bulk_writing = USB_NO;
                g_is_usb_interrupt_writing = USB_NO;
                ctrl.type = USB_PCDC;
                R_USB_Read(&ctrl, g_usb_data, DATA_LEN);
            break;

            case USB_STS_WRITE_COMPLETE :
                if (USB_PCDC == ctrl.type)
                {
                    /* Bulk IN(CDC data) transfer has been completed */
                    g_is_usb_bulk_writing = USB_NO;
                }
                else
                {
                    /* Interrupt IN(SerialState) transfer has been completed */
                    g_is_usb_interrupt_writing = USB_NO;
                }
            break;

            case USB_STS_READ_COMPLETE :
                g_read_size = ctrl.size;    /* Bulk OUT data size */
                if(g_read_size == 0)
                {
                    ctrl.type = USB_PCDC;
                    R_USB_Read(&ctrl, g_usb_data, DATA_LEN);
                }
            break;

            case USB_STS_REQUEST : /* Receive Class Request */
                if (USB_PCDC_SET_LINE_CODING == (ctrl.setup.type & USB_BREQUEST))
                {
                    ctrl.type = USB_REQUEST;
                    ctrl.module     = USE_USBIP;
                    R_USB_Read(&ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                }
                else if (USB_PCDC_GET_LINE_CODING == (ctrl.setup.type & USB_BREQUEST))
                {
                    ctrl.type = USB_REQUEST;
                    ctrl.module     = USE_USBIP;
                    R_USB_Write(&ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                }
                else
                {
                    ctrl.type = USB_REQUEST;
                    ctrl.status = USB_ACK;
                    R_USB_Write(&ctrl, (uint8_t*)USB_NULL, (uint32_t)USB_NULL);
                }
            break;

            case USB_STS_REQUEST_COMPLETE : /* Complete Class Request */
                if (USB_PCDC_SET_LINE_CODING == (ctrl.setup.type & USB_BREQUEST))
                {
                    /* RS-232C Line set.(Parameter : line_coding) */
                    usb_cpu_Sci_Set1(&g_line_coding);
                }
                else if (USB_PCDC_GET_LINE_CODING == (ctrl.setup.type & USB_BREQUEST))
                {
                    /* none */
                }
                else if (USB_PCDC_SET_CONTROL_LINE_STATE == (ctrl.setup.type & USB_BREQUEST))
                {
                    /* DTR & RTS set value store */

                    /* RS-232 signal RTS & DTR Set */
                    /* If RTS/DTR control function is prepared, calls this function here */
                }
                else
                {
                    /* none */
                }
            break;

            case USB_STS_SUSPEND :
            case USB_STS_DETACH :
#if USB_SUPPORT_LPW == USB_APL_ENABLE
                low_power_mcu();
#endif /* USB_SUPPORT_LPW == USB_APL_ENABLE */
            break;

            default :
            break;
        }
#if USB_SUPPORT_RTOS == USB_APL_ENABLE
        }
#endif /* USB_SUPPORT_RTOS == USB_APL_ENABLE */

        /* SCI Receive -> USB Send */
        /* Check whether SCI data is received or not */
        sci_err = R_SCI_Control(g_console, SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, (void *)&size);
        if ((SCI_SUCCESS == sci_err) && (size > 0UL) && ((g_sci_counter + size) <= DATA_LEN))
        {
            /* Receive the SCI data */
            sci_err = R_SCI_Receive(g_console, ((uint8_t *)&g_sci_data) + g_sci_counter, size);
            if (SCI_SUCCESS == sci_err)
            {
                g_sci_counter += size;
            }
        }
        /* Send the received SCI data to USB Host */
        if ((USB_NO == g_is_usb_bulk_writing) && (g_sci_counter != 0UL))
        {
            ctrl.type = USB_PCDC;
            ctrl.module = USE_USBIP;
            err = R_USB_Write(&ctrl, (uint8_t *)&g_sci_data, g_sci_counter);
            if (USB_SUCCESS == err)
            {
                g_is_usb_bulk_writing = USB_YES;
                g_sci_counter = 0UL;
            }
        }

        /* USB Received -> SCI Send */
        if (g_read_size > 0UL)
        {
            /* Check whether the sending of SCI data is possible or not */
            sci_err = R_SCI_Control(g_console, SCI_CMD_TX_Q_BYTES_FREE, (void *)&size);
            if ((size >= g_read_size) && (SCI_SUCCESS == sci_err))
            {
                /* Sending the received USB data(Bulk OUT) to COM */
                sci_err = R_SCI_Send(g_console, (uint8_t *)&g_usb_data, g_read_size);
                if (SCI_SUCCESS == sci_err)
                {
                    g_read_size = 0UL;

                    ctrl.type = USB_PCDC;
                    ctrl.module     = USE_USBIP;
                    R_USB_Read(&ctrl, g_usb_data, DATA_LEN);
                }
            }
        }

        /* SCI Error -> USB Notification */
        if (USB_NO == g_is_usb_interrupt_writing)
        {
            /* Check whether the SerialState information is or not*/
            if (0 != g_is_serial_state)
            {
                /* Send the SerialState information for SCI to USB Host */
                ctrl.type = USB_PCDCC;
                ctrl.module = USE_USBIP;
                err = R_USB_Write(&ctrl, (uint8_t *)&g_serial_state, 2);
                if (USB_SUCCESS == err)
                {
                    g_is_usb_interrupt_writing = USB_YES;
                    g_is_serial_state = 0;
                }
            }
        }
    }
} /* End of function usb_main() */


/******************************************************************************
 Function Name   : apl_init
 Description     : Application Initialize
 Argument        : none
 Return          : none
 ******************************************************************************/
static void apl_init (void)
{
    memset((void *)&g_line_coding, 0, sizeof(usb_pcdc_linecoding_t));
    memset (g_usb_data, 0, DATA_LEN);
    memset (g_sci_data, 0, DATA_LEN);
    memset (g_serial_state, 0, 2);
    memset (g_console, 0, sizeof(sci_hdl_t));
    g_is_usb_bulk_writing = USB_NO;
    g_is_usb_interrupt_writing = USB_NO;
    g_is_serial_state = 0;
    g_sci_counter = 0UL;
    g_read_size = 0UL;

} /* End of function apl_init */

/******************************************************************************
 Function Name   : usb_pin_setting
 Description     : USB port mode and Switch mode Initialize
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_pin_setting (void)
{
#if USE_USBIP == USB_IP0
    R_USB_PinSet_USB0_PERI();
#else   /* USE_USBIP == USE_USBIP0 */
    R_USB_PinSet_USBA_PERI();
#endif  /* USE_USBIP == USE_USBIP0 */
} /* End of function usb_pin_setting */


#endif  /* OPERATION_MODE == USB_UART */

/******************************************************************************
 End  Of File
 ******************************************************************************/

