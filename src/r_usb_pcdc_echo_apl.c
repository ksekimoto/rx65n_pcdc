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
 * File Name    : r_usb_pcdc_echo_apl.c
 * Description  : USB Peripheral Communications Devices Class Sample Code
 *******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 31.03.2018 1.23 Using Pin setting API.
 *                           Support CDC Class Request.
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

#if OPERATION_MODE == USB_ECHO
/******************************************************************************
 Macro definitions
 ***************************************************************************#***/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static void     usb_pin_setting (void);
static uint8_t  g_buf[DATA_LEN];
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
    usb_ctrl_t  ctrl;
    usb_cfg_t   cfg;
#if (BSP_CFG_RTOS_USED != 0)                                /* Use RTOS */
    usb_ctrl_t  *p_mess;
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

    usb_pin_setting(); /* USB MCU pin setting */

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

        rtos_receive_mailbox(&g_usb_apl_mbx_id, (void **)&p_mess, RTOS_FOREVER);

        ctrl = *p_mess;
        /* Analyzing the received message */
        switch (ctrl.event)
#else   /* USB_SUPPORT_RTOS == USB_APL_ENABLE */
        switch (R_USB_GetEvent(&ctrl))
#endif  /* USB_SUPPORT_RTOS == USB_APL_ENABLE */
        {
            case USB_STS_CONFIGURED :
            case USB_STS_WRITE_COMPLETE :
                ctrl.type = USB_PCDC;
                R_USB_Read(&ctrl, g_buf, DATA_LEN);
            break;

            case USB_STS_READ_COMPLETE :
                R_USB_Write(&ctrl, g_buf, ctrl.size);
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

            case USB_STS_SUSPEND :
            case USB_STS_DETACH :
#if USB_SUPPORT_LPW == USB_APL_ENABLE
                low_power_mcu();
#endif /* USB_SUPPORT_LPW == USB_APL_ENABLE */
            break;

            default :
            break;
        }
    }
} /* End of function usb_main() */

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

#endif  /* OPERATION_MODE == USB_ECHO */

/******************************************************************************
 End  Of File
 ******************************************************************************/

