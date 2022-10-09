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
/***********************************************************************************************************************
* File Name    : r_usb_rsk_sci.h
* Description  : Sci define
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 01.03.2020 1.30 RX72N/RX66N is added and uITRON is supported.
***********************************************************************************************************************/
#ifndef R_USB_RSK_SCI_H
#define R_USB_RSK_SCI_H

/******************************************************************************
Constant macro definitions
******************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    RSK USE IO SELECT
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/******************************************************************************
Macro definitions
******************************************************************************/
/* Use SCI configuration */
/* Include target mcu interface header file. */
#if defined(BSP_MCU_RX113)
    #define PCDC_USE_SCI                SCI6
    #define USB_PSMPL_USE_SCI           (SCI_CH6)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI6
    #define RXD_PDR                     PORT0.PDR.BIT.B2
    #define RXD_PMR                     PORT0.PMR.BIT.B2
    #define RXD_PFS                     MPC.P02PFS.BYTE
    #define TXD_PODR                    PORT0.PODR.BIT.B7
    #define TXD_PDR                     PORT0.PDR.BIT.B7
    #define TXD_PMR                     PORT0.PMR.BIT.B7
    #define TXD_PFS                     MPC.P07PFS.BYTE
#elif defined(BSP_MCU_RX111)
    #define PCDC_USE_SCI                SCI12
    #define USB_PSMPL_USE_SCI           (SCI_CH12)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI12
    #define RXD_PDR                     PORTE.PDR.BIT.B1
    #define RXD_PMR                     PORTE.PMR.BIT.B1
    #define RXD_PFS                     MPC.PE1PFS.BYTE
    #define TXD_PODR                    PORTE.PODR.BIT.B2
    #define TXD_PDR                     PORTE.PDR.BIT.B2
    #define TXD_PMR                     PORTE.PMR.BIT.B2
    #define TXD_PFS                     MPC.PE2PFS.BYTE
#elif defined(BSP_MCU_RX64M)
    #define PCDC_USE_SCI                SCI7
    #define USB_PSMPL_USE_SCI           (SCI_CH7)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI7
    #define RXD_PDR                     PORT9.PDR.BIT.B2
    #define RXD_PMR                     PORT9.PMR.BIT.B2
    #define RXD_PFS                     MPC.P92PFS.BYTE
    #define TXD_PODR                    PORT9.PODR.BIT.B0
    #define TXD_PDR                     PORT9.PDR.BIT.B0
    #define TXD_PMR                     PORT9.PMR.BIT.B0
    #define TXD_PFS                     MPC.P90PFS.BYTE
#elif defined(BSP_MCU_RX71M)
    #define PCDC_USE_SCI                SCI7
    #define USB_PSMPL_USE_SCI           (SCI_CH7)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI7
    #define RXD_PDR                     PORT9.PDR.BIT.B2
    #define RXD_PMR                     PORT9.PMR.BIT.B2
    #define RXD_PFS                     MPC.P92PFS.BYTE
    #define TXD_PODR                    PORT9.PODR.BIT.B0
    #define TXD_PDR                     PORT9.PDR.BIT.B0
    #define TXD_PMR                     PORT9.PMR.BIT.B0
    #define TXD_PFS                     MPC.P90PFS.BYTE
#elif defined(BSP_MCU_RX72T)
    #define PCDC_USE_SCI                SCI5
    #define USB_PSMPL_USE_SCI           (SCI_CH5)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI5
    #define RXD_PDR                     PORTB.PDR.BIT.B6
    #define RXD_PMR                     PORTB.PMR.BIT.B6
    #define RXD_PFS                     MPC.PB6PFS.BYTE
    #define TXD_PODR                    PORTB.PODR.BIT.B5
    #define TXD_PDR                     PORTB.PDR.BIT.B5
    #define TXD_PMR                     PORTB.PMR.BIT.B5
    #define TXD_PFS                     MPC.PB5PFS.BYTE
#elif defined(BSP_MCU_RX231)
    #define PCDC_USE_SCI                SCI5
    #define USB_PSMPL_USE_SCI           (SCI_CH5)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI5
    #define RXD_PDR                     PORTA.PDR.BIT.B3
    #define RXD_PMR                     PORTA.PMR.BIT.B3
    #define RXD_PFS                     MPC.PA3PFS.BYTE
    #define TXD_PODR                    PORTA.PODR.BIT.B4
    #define TXD_PDR                     PORTA.PDR.BIT.B4
    #define TXD_PMR                     PORTA.PMR.BIT.B4
    #define TXD_PFS                     MPC.PA4PFS.BYTE
#elif defined(BSP_MCU_RX65N_2MB)
    #define PCDC_USE_SCI                SCI8
    #define USB_PSMPL_USE_SCI           (SCI_CH8)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI8
    #define RXD_PDR                     PORTJ.PDR.BIT.B1
    #define RXD_PMR                     PORTJ.PMR.BIT.B1
    #define RXD_PFS                     MPC.PJ1PFS.BYTE
    #define TXD_PODR                    PORTJ.PODR.BIT.B2
    #define TXD_PDR                     PORTJ.PDR.BIT.B2
    #define TXD_PMR                     PORTJ.PMR.BIT.B2
    #define TXD_PFS                     MPC.PJ2PFS.BYTE
#elif defined(BSP_MCU_RX65N)
    #define PCDC_USE_SCI                SCI2
    #define USB_PSMPL_USE_SCI           (SCI_CH2)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI2
    #define RXD_PDR                     PORT5.PDR.BIT.B2
    #define RXD_PMR                     PORT5.PMR.BIT.B2
    #define RXD_PFS                     MPC.P52PFS.BYTE
    #define TXD_PODR                    PORT5.PODR.BIT.B0
    #define TXD_PDR                     PORT5.PDR.BIT.B0
    #define TXD_PMR                     PORT5.PMR.BIT.B0
    #define TXD_PFS                     MPC.P50PFS.BYTE
#elif defined(BSP_MCU_RX23W)
    #define PCDC_USE_SCI                SCI8
    #define USB_PSMPL_USE_SCI           (SCI_CH8)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI8
    #define RXD_PDR                     PORTC.PDR.BIT.B6
    #define RXD_PMR                     PORTC.PMR.BIT.B6
    #define RXD_PFS                     MPC.PC6PFS.BYTE
    #define TXD_PODR                    PORTC.PODR.BIT.B7
    #define TXD_PDR                     PORTC.PDR.BIT.B7
    #define TXD_PMR                     PORTC.PMR.BIT.B7
    #define TXD_PFS                     MPC.PC7PFS.BYTE
#elif defined(BSP_MCU_RX72M)
    #define PCDC_USE_SCI                SCI6
    #define USB_PSMPL_USE_SCI           (SCI_CH6)
    #define R_SCI_PinSet_SCI            R_SCI_PinSet_SCI6
    #define RXD_PDR                     PORT0.PDR.BIT.B1
    #define RXD_PMR                     PORT0.PMR.BIT.B1
    #define RXD_PFS                     MPC.P01PFS.BYTE
    #define TXD_PODR                    PORT0.PODR.BIT.B0
    #define TXD_PDR                     PORT0.PDR.BIT.B0
    #define TXD_PMR                     PORT0.PMR.BIT.B0
    #define TXD_PFS                     MPC.P00PFS.BYTE

#else
    #error "This MCU is not supported."
#endif

/****** COMMON ***************************************************************/
#define USB_PSMPL_ERROR_SET         1
#define USB_PSMPL_ERROR_CLEAR       0

/* MPC write protect */
#define MPC_WRITE_PROTECT_ON        MPC.PWPR.BIT.PFSWE = 0u;\
                                    MPC.PWPR.BIT.B0WI  = 1u;

#define MPC_WRITE_PROTECT_OFF       MPC.PWPR.BIT.B0WI  = 0u;\
                                    MPC.PWPR.BIT.PFSWE = 1u;

/* Port init */
#define SCI_TXD_PIN_SET             TXD_PMR   = 0u;    /* Port mode register : peripheral io module use */\
                                    TXD_PFS    = 0x0a;  /* Port Pin function select register : TXD */\
                                    TXD_PMR   = 1u;

#define SCI_RXD_PIN_SET             RXD_PMR   = 0u;    /* Port mode register : peripheral io module use */\
                                    RXD_PFS    = 0x0a;  /* Port Pin function select register : RXD */\
                                    RXD_PMR   = 1u;

#define SCI_TXD_PIN_CLEAR           TXD_PMR   = 0u;    /* Port mode register : peripheral io module use */\
                                    PCDC_USE_SCI.SCR.BIT.TIE   = 0u;    \
                                    PCDC_USE_SCI.SCR.BIT.TE    = 0u;    \
                                    PCDC_USE_SCI.SCR.BIT.TEIE  = 0u;

#define SCI_RXD_PIN_CLEAR           RXD_PMR   = 0u;    /* Port mode register : peripheral io module use */\
                                    PCDC_USE_SCI.SCR.BIT.RE    = 0;

#define SCI_TXD_PIN_RESUME          TXD_PMR   = 1u;    /* Port mode register : peripheral io module use */\
                                    PCDC_USE_SCI.SCR.BIT.TIE   = 1u;    \
                                    PCDC_USE_SCI.SCR.BIT.TE    = 1u;    \
                                    PCDC_USE_SCI.SCR.BIT.TEIE  = 1u;

#define SCI_RXD_PIN_RESUME          RXD_PMR   = 1u;      /* Port mode register : peripheral io module use */\
                                    PCDC_USE_SCI.SCR.BIT.RE    = 1;


#define USB_SCI_CLEAR               0u
#define USB_SCI_ERR_DETECT          1u

/*  */
#if OPERATION_MODE == USB_UART
#define USB_RSK_SCI_INIT()              usb_cpu_Sci_Init()
#else
#define USB_RSK_SCI_INIT()              /* Not use */
#endif  /* OPERATION_MODE == USB_UART */

/*******************************************************************************
Typedef definitions
*******************************************************************************/
typedef enum
{
    USB_SCI_DATA_BIT_7  = 7u,   /* Line Coding 7Data bits */
    USB_SCI_DATA_BIT_8  = 8u    /* Line Coding 8Data bits */
} USB_SCI_DATA_BIT_t;

typedef enum
{
    USB_SCI_STOP_BIT_1  = 0u,   /* Line Coding 1Stop bits */
    USB_SCI_STOP_BIT_2  = 2u    /* Line Coding 2Stop bits */
}
USB_SCI_STOT_BIT_t;

typedef enum
{
    USB_SCI_PARITY_BIT_NONE = 0u,   /* Line Coding None Parity */
    USB_SCI_PARITY_BIT_ODD  = 1u,   /* Line Coding Odd Parity */
    USB_SCI_PARITY_BIT_EVEN = 2u    /* Line Coding Even Parity */
}
USB_SCI_PARITY_BIT_t;

typedef enum
{
    USB_SCI_SPEED_1200      = 1200u,    /* Line Coding 1200bps */
    USB_SCI_SPEED_2400      = 2400u,    /* Line Coding 2400bps */
    USB_SCI_SPEED_4800      = 4800u,    /* Line Coding 4800bps */
    USB_SCI_SPEED_9600      = 9600u,    /* Line Coding 9600bps */
    USB_SCI_SPEED_14400     = 14400u,   /* Line Coding 14400bps */
    USB_SCI_SPEED_19200     = 19200u,   /* Line Coding 19200bps */
    USB_SCI_SPEED_38400     = 38400u,   /* Line Coding 38400bps */
    USB_SCI_SPEED_57600     = 57600u,   /* Line Coding 57600bps */
    USB_SCI_SPEED_115200    = 115200u,  /* Line Coding 115200bps */
}
USB_SCI_LINE_SPEED_t;

/******************************************************************************
External variables and functions
******************************************************************************/
extern void         usb_cpu_Sci_Init(void);
extern uint16_t     usb_cpu_Sci_Set1(usb_pcdc_linecoding_t *p_com_para);

extern uint8_t      g_is_serial_state;
extern uint8_t      g_serial_state[];

#endif  /* R_USB_RSK_SCI_H */
/******************************************************************************
End Of File
******************************************************************************/
