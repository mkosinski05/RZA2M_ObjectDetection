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

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdio.h>
#include <fcntl.h>

#include "r_typedefs.h"
#include "iodefine.h"

#include "r_bcd_ae.h"
#include "r_bcd_camera.h"
#include "draw.h"
#include "r_mmu_drv_api.h"
#include "r_riic_drv_api.h"
#include "r_devlink_wrapper.h"
#include "r_riic_drv_sc_cfg.h"


/*******************************************************************************
Macro definitions
*******************************************************************************/
#define MIPI_CAMERA_SUBADD  (0x20)
#define MASK_8BIT           (0xFFu)

/*******************************************************************************
Imported global variables and functions (from other files)
*******************************************************************************/
extern  void RIIC_MipiWrite( uint8_t ic_adr, uint8_t reg_adr_upper, uint8_t reg_adr_lower, uint8_t reg_data );

/*******************************************************************************
Private global variables and functions
*******************************************************************************/

/******************************************************************************
* Function Name: R_BCD_AeInit
* Description  : Initialize AE
* Arguments    : pAeSet
*              :   AE setting struct address
* Return Value : AE mode
******************************************************************************/
uint8_t R_BCD_AeInit(r_bcd_ae_setting_t* pAeSet)
{
    uint16_t mipi_set;

    pAeSet->mode           = R_BCD_AE_MODE_INIT;
    pAeSet->brightness     = 0;
    pAeSet->brightness_old = 0;
    pAeSet->shutter_speed  = R_BCD_AE_S_SPEED_DEFAULT;
    pAeSet->grobal_gain    = R_BCD_AE_G_GAIN_DEFAULT;
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
    mipi_set = pAeSet->shutter_speed;
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5A, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5B, (uint8_t)(mipi_set & MASK_8BIT));
    mipi_set = pAeSet->grobal_gain;
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x58, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x59, (uint8_t)(mipi_set & MASK_8BIT));
#endif

    return R_BCD_AE_MODE_INIT;
}

/******************************************************************************
* Function Name: R_BCD_AeStop
* Description  : Stop AE
* Arguments    : pAeSet
*              :   AE setting struct address
* Return Value : AE mode
******************************************************************************/
uint8_t R_BCD_AeStop(r_bcd_ae_setting_t* pAeSet)
{
    uint16_t mipi_set;

    pAeSet->mode           = R_BCD_AE_MODE_STOP;
    pAeSet->shutter_speed  = R_BCD_AE_S_SPEED_DEFAULT;
    pAeSet->grobal_gain    = R_BCD_AE_G_GAIN_DEFAULT;
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
    mipi_set = pAeSet->shutter_speed;
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5A, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5B, (uint8_t)(mipi_set & MASK_8BIT));
    mipi_set = pAeSet->grobal_gain;
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x58, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x59, (uint8_t)(mipi_set & MASK_8BIT));
#endif
    return R_BCD_AE_MODE_STOP;
}

/******************************************************************************
* Function Name: R_BCD_AeStart
* Description  : Start AE
* Arguments    : pAeSet
*              :   AE setting struct address
* Return Value : AE mode
******************************************************************************/
uint8_t R_BCD_AeStart(r_bcd_ae_setting_t* pAeSet)
{
    pAeSet->mode           = R_BCD_AE_MODE_INIT;

    return R_BCD_AE_MODE_INIT;
}

/******************************************************************************
* Function Name: R_BCD_AeGetMode
* Description  : Get present AE mode 
* Arguments    : pAeSet
*              :   AE setting struct address
* Return Value : AE mode
******************************************************************************/
uint8_t R_BCD_AeGetMode(r_bcd_ae_setting_t* pAeSet)
{
    return pAeSet->mode;
}

/******************************************************************************
* Function Name: R_BCD_AeMakeGammaTable
* Description  : Make gamma table
* Arguments    : pAeSet
*              :   AE setting struct address
*              : gamma
*              :   Gamma value
*              : pTable
*              :   Gamma table address
* Return Value : AE mode
******************************************************************************/
void    R_BCD_AeMakeGammaTable(r_bcd_ae_setting_t* pAeSet, double gamma, uint8_t* pTable)
{
    int32_t i;
    
    pAeSet->brightness = 0;
    if (pAeSet->mode != R_BCD_AE_MODE_STOP) {
        pAeSet->mode = R_BCD_AE_MODE_INIT;
    }
    for( i = 0; i < 256; i++ ) {
        pTable[i] = (uint8_t)(pow((double)i / 255.0, 1.0 / gamma) * 255.0);
        if ((pTable[i] >= BRIGHTNESS_SET) && (pAeSet->brightness == 0)) {
            pAeSet->brightness = (uint16_t)i;
        }
    }
    return;
}

/******************************************************************************
* Function Name: R_BCD_AeRunAutoExpousure
* Description  : Set average luminance for AE 
* Arguments    : pAeSet
*              :   AE setting struct address
*              : ave_lum
*              :   Average luminance
* Return Value : AE mode
******************************************************************************/
uint8_t R_BCD_AeRunAutoExpousure(r_bcd_ae_setting_t* pAeSet, uint16_t ave_lum)
{
    if ((ave_lum <= BRIGHTNESS_MAX) && (pAeSet->mode != R_BCD_AE_MODE_STOP)) {
        if ((ave_lum >= BRIGHTNESS_INIT_TOP) || (ave_lum <= BRIGHTNESS_INIT_BOT)) {
            pAeSet->mode = R_BCD_AE_MODE_INIT;
        }
        if (pAeSet->mode == R_BCD_AE_MODE_INIT) {
            if (ave_lum < pAeSet->brightness) {
                /* under */
                pAeSet->shutter_speed = (uint16_t)(((uint32_t)pAeSet->shutter_speed * pAeSet->brightness) / ave_lum);
                if (pAeSet->shutter_speed >= R_BCD_AE_S_SPEED_MAX) {
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
                    if (pAeSet->grobal_gain == 0x0100) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_6DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else if (pAeSet->grobal_gain == 0x01FF) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_9DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else if (pAeSet->grobal_gain == 0x02D2) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_12DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else if (pAeSet->grobal_gain == 0x03FB) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_18DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else if (pAeSet->grobal_gain == 0x07F1) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_24DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else {
                        pAeSet->shutter_speed = R_BCD_AE_S_SPEED_MAX;
                    }
#else
                    pAeSet->shutter_speed = R_BCD_AE_S_SPEED_MAX;
#endif
                } else {
                    /* DO NOTHING */
                }
                R_BCD_AeSetShutterSpeed(pAeSet, pAeSet->shutter_speed);
            } else if (ave_lum > pAeSet->brightness) {
                /* upper */
                pAeSet->shutter_speed = (uint16_t)(((uint32_t)pAeSet->shutter_speed * pAeSet->brightness) / ave_lum);
                if (pAeSet->shutter_speed <= R_BCD_AE_S_SPEED_MIN) {
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
                    if (pAeSet->grobal_gain == 0x01FF) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_0DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else if (pAeSet->grobal_gain == 0x02D2) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_6DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else if (pAeSet->grobal_gain == 0x03FB) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_9DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else if (pAeSet->grobal_gain == 0x07F1) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_12DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else if (pAeSet->grobal_gain == 0x0FD9) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_18DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else {
                        pAeSet->shutter_speed = R_BCD_AE_S_SPEED_MIN;
                    }
#else
                    pAeSet->shutter_speed = R_BCD_AE_S_SPEED_MIN;
#endif
                } else {
                    /* DO NOTHING */
                }
                R_BCD_AeSetShutterSpeed(pAeSet, pAeSet->shutter_speed);
            }
            pAeSet->mode = R_BCD_AE_MODE_SET_IN;
        } else if (pAeSet->mode == R_BCD_AE_MODE_SET_IN) {
            if (ave_lum < ((pAeSet->brightness * (100 - BRIGHTNESS_TH)) / 100)) {
                /* under */
                if (pAeSet->shutter_speed <= R_BCD_AE_S_SPEED_MAX) {
                    pAeSet->shutter_speed = (uint16_t)(pAeSet->shutter_speed + R_BCD_AE_S_SPEED_GAIN);
                } else {
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
                    if (pAeSet->grobal_gain == 0x0100) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_6DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else if (pAeSet->grobal_gain == 0x01FF) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_9DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else if (pAeSet->grobal_gain == 0x02D2) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_12DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else if (pAeSet->grobal_gain == 0x03FB) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_18DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else if (pAeSet->grobal_gain == 0x07F1) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_24DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MAX / 2);
                    } else {
                        /* DO NOTHING */
                    }
#else
                    /* DO NOTHING */
#endif
                }
                R_BCD_AeSetShutterSpeed(pAeSet, pAeSet->shutter_speed);
            } else if (ave_lum > ((pAeSet->brightness * (100 + BRIGHTNESS_TH)) / 100)) {
                /* upper */
                if (pAeSet->shutter_speed >= R_BCD_AE_S_SPEED_MIN) {
                    pAeSet->shutter_speed = (uint16_t)(pAeSet->shutter_speed - R_BCD_AE_S_SPEED_GAIN);
                } else {
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
                    if (pAeSet->grobal_gain == 0x01FF) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_0DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else if (pAeSet->grobal_gain == 0x02D2) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_6DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else if (pAeSet->grobal_gain == 0x03FB) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_9DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else if (pAeSet->grobal_gain == 0x07F1) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_12DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else if (pAeSet->grobal_gain == 0x0FD9) {
                        R_BCD_AeSetGrobalGain(pAeSet, R_BCD_AE_G_GAIN_18DB);
                        pAeSet->mode = R_BCD_AE_MODE_INIT;
                        pAeSet->shutter_speed = (R_BCD_AE_S_SPEED_MIN * 4);
                    } else {
                        /* DO NOTHING */
                    }
#else
                    /* DO NOTHING */
#endif
                }
                R_BCD_AeSetShutterSpeed(pAeSet, pAeSet->shutter_speed);
            }
        }
        pAeSet->brightness_old = ave_lum;
    }
    return pAeSet->mode;
}

/******************************************************************************
* Function Name: R_BCD_AeSetShutterSpeed
* Description  : Set Camera shutter speed
* Arguments    : pAeSet
*              :   AE setting struct address
*              : speed
*              :   Shutter speed
* Return Value : AE mode
******************************************************************************/
uint8_t R_BCD_AeSetShutterSpeed(r_bcd_ae_setting_t* pAeSet, uint16_t speed)
{
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5A, (uint8_t)(speed >> 8) & MASK_8BIT);
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5B, (uint8_t)(speed & MASK_8BIT));
#endif

    return pAeSet->mode;
}

/******************************************************************************
* Function Name: R_BCD_AeSetGrobalGain
* Description  : Set Camera grobal gain
* Arguments    : pAeSet
*              :   AE setting struct address
*              : db
*              :   Grobal gain(dB)
* Return Value : AE mode
******************************************************************************/
uint8_t R_BCD_AeSetGrobalGain(r_bcd_ae_setting_t* pAeSet, uint8_t db)
{
    uint16_t mipi_set;

    switch( db ) {
        case R_BCD_AE_G_GAIN_0DB:
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
        pAeSet->grobal_gain = 0x0100;
        mipi_set = pAeSet->grobal_gain;
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x58, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x59, (uint8_t)(mipi_set & MASK_8BIT));
#endif
        break;
        case R_BCD_AE_G_GAIN_6DB:
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
        pAeSet->grobal_gain = 0x01FF;
        mipi_set = pAeSet->grobal_gain;
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x58, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x59, (uint8_t)(mipi_set & MASK_8BIT));
#endif
        break;
        case R_BCD_AE_G_GAIN_9DB:
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
        pAeSet->grobal_gain = 0x02D2;
        mipi_set = pAeSet->grobal_gain;
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x58, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x59, (uint8_t)(mipi_set & MASK_8BIT));
#endif
        break;
        case R_BCD_AE_G_GAIN_12DB:
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
        pAeSet->grobal_gain = 0x03FB;
        mipi_set = pAeSet->grobal_gain;
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x58, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x59, (uint8_t)(mipi_set & MASK_8BIT));
#endif
        break;
        case R_BCD_AE_G_GAIN_18DB:
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
        pAeSet->grobal_gain = 0x07F1;
        mipi_set = pAeSet->grobal_gain;
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x58, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x59, (uint8_t)(mipi_set & MASK_8BIT));
#endif
        break;
        case R_BCD_AE_G_GAIN_24DB:
#if   (ISP_CAM_MIPI == CAM_IMX219_B)
        pAeSet->grobal_gain = 0x0FD9;
        mipi_set = pAeSet->grobal_gain;
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x58, (uint8_t)(mipi_set >> 8) & MASK_8BIT);
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x59, (uint8_t)(mipi_set & MASK_8BIT));
#endif
        break;
    }
    return pAeSet->mode;
}

/* End of File */
