/*******************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only
 * intended for use with Renesas products. No other uses are authorized. This
 * software is owned by Renesas Electronics Corporation and is protected under
 * all applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 * LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 * TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 * ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 * ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software
 * and to discontinue the availability of this software. By using this software,
 * you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/**************************************************************************//**
* @file         lcd_lvds_clk.h
* @version      0.01
* @brief        Defines for VDC clock.
******************************************************************************/
#ifndef LCD_LVDS_CLK_H
#define LCD_LVDS_CLK_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdlib.h>

#include    "r_typedefs.h"

#include    "r_vdc.h"
#include    "pc_monitor.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* Clock setting
        DCDR  NFD   NRD  NODIV
        1     28    6    -      (44.0)  44.0 MHz, WVGA Signal  800 x 480 (60 Hz)
*/
#define LCD_CH0_PANEL_CLK         (VDC_PANEL_ICKSEL_LVDS_DIV7)
#define LVDS_PLL_INPUT_CLK        (VDC_LVDS_INCLK_SEL_PERI)
#define LCD_CH0_PANEL_CLK_DIV     (VDC_PANEL_CLKDIV_1_1)
#define LVDS_PLL_NFD              (28u-2u)
#define LVDS_PLL_NRD              (6u-1u)
#define LVDS_PLL_NODIV            (VDC_LVDS_NDIV_1)

#define LVDS_PLL_NIDV             (VDC_LVDS_NDIV_1)      /* Not use */
#define LVDS_PLL_NOD              (VDC_LVDS_PLL_NOD_4)   /* Not use */

#endif  /* LCD_LVDS_CLK_H */
