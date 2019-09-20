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
#ifndef R_BCD_CAMERA_H
#define R_BCD_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "mipi_camera.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define R_BCD_CAMERA_WIDTH          (CAM_VIN_OUTPUT_IS)
#define R_BCD_CAMERA_HEIGHT         (CAM_VIN_PRECLIP_WIDTH_Y)
#define R_BCD_CAMERA_FRAME_BUF_NUM  (1)
#define R_BCD_CAMERA_NOT_CAPTURED   (-1)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
int32_t R_BCD_CameraInit(void);
int32_t R_BCD_CameraCaptureStart(void);
uint8_t *R_BCD_CameraGetFrameAddress(int32_t index);
int32_t R_BCD_CameraGetCaptureStatus(void);
void R_BCD_CameraClearCaptureStatus(void);
void R_BCD_CameraSetDefaultShtterSpeed (void);

#ifdef __cplusplus
}
#endif

#endif  /* R_BCD_CAMERA_H */

/* End of File */
