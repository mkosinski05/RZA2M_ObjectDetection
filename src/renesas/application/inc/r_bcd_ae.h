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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
#ifndef R_BCD_AE_H
#define R_BCD_AE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_typedefs.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
#define CAM_IMX219_B    (0)   /* Raspberri Pi Camera V2, WVGA */

#define ISP_CAM_MIPI        CAM_IMX219_B

#if   (ISP_CAM_MIPI == CAM_IMX219_B)
#else
    #error  Undefined mipi camera!
#endif

#define R_BCD_AE_MODE_STOP          (0xFFu)
#define R_BCD_AE_MODE_INIT          (0xFEu)
#define R_BCD_AE_MODE_NONE          (0x00u)
#define R_BCD_AE_MODE_SET_IN        (0x01u)

#if   (ISP_CAM_MIPI == CAM_IMX219_B)
#define R_BCD_AE_S_SPEED_DEFAULT    (0x03E8u)
#define R_BCD_AE_S_SPEED_MAX        (0x0DACu)
#define R_BCD_AE_S_SPEED_MIN        (0x0080u)
#define R_BCD_AE_S_SPEED_GAIN       (0x0020u)
#define R_BCD_AE_G_GAIN_DEFAULT     (0x0100u)
#else
#define R_BCD_AE_S_SPEED_DEFAULT    (0x03E8u)
#define R_BCD_AE_S_SPEED_MAX        (0x1388u)
#define R_BCD_AE_S_SPEED_MIN        (0x0080u)
#define R_BCD_AE_S_SPEED_GAIN       (0x0020u)
#define R_BCD_AE_G_GAIN_DEFAULT     (0x0100u)
#endif

#define R_BCD_AE_G_GAIN_0DB         (0)
#define R_BCD_AE_G_GAIN_6DB         (1)
#define R_BCD_AE_G_GAIN_9DB         (2)
#define R_BCD_AE_G_GAIN_12DB        (3)
#define R_BCD_AE_G_GAIN_18DB        (4)
#define R_BCD_AE_G_GAIN_24DB        (5)

#define BRIGHTNESS_SET              (100)
#define BRIGHTNESS_TH               (10)

#define BRIGHTNESS_MAX              (255)
#define BRIGHTNESS_INIT_TOP         (200)
#define BRIGHTNESS_INIT_BOT         (40)

typedef struct
{
    uint16_t    brightness;
    uint16_t    brightness_old;
    uint16_t    shutter_speed;
    uint16_t    grobal_gain;
    uint8_t     mode;
}
r_bcd_ae_setting_t;

/*******************************************************************************
Functions Prototypes
*******************************************************************************/
uint8_t R_BCD_AeInit(r_bcd_ae_setting_t* pIspSet);
uint8_t R_BCD_AeStart(r_bcd_ae_setting_t* pIspSet);
uint8_t R_BCD_AeStop(r_bcd_ae_setting_t* pIspSet);
uint8_t R_BCD_AeGetMode(r_bcd_ae_setting_t* pIspSet);
void    R_BCD_AeMakeGammaTable(r_bcd_ae_setting_t* pIspSet, double gamma, uint8_t* pTable);
uint8_t R_BCD_AeRunAutoExpousure(r_bcd_ae_setting_t* pIspSet, uint16_t ave_lum);
uint8_t R_BCD_AeSetShutterSpeed(r_bcd_ae_setting_t* pAeSet, uint16_t speed);
uint8_t R_BCD_AeSetGrobalGain(r_bcd_ae_setting_t* pIspSet, uint8_t db);

#ifdef __cplusplus
}
#endif

#endif  /* R_BCD_AE_H */

/* End of File */
