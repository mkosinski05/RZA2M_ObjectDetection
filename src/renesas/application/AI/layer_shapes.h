/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED 'AS IS' AND RENESAS MAKES NO WARRANTIES REGARDING
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
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : layer_shapes.h
* Version      : 1.00
* Description  : Initializations
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.06.2017 1.00     First Release
***********************************************************************************************************************/

#include "Typedef.h"
#ifndef LAYER_SHAPES_H_
#define LAYER_SHAPES_H_
 
TPrecision* dnn_compute(TPrecision*);
 
TPrecision conv1_out[30752];
TPrecision pool1_out[7688];
TPrecision conv2_out[13456];
TPrecision conv3_out[11664];
TPrecision pool2_pad[12544];
TPrecision pool2_out[3136];
TPrecision ip1_out[3];
TPrecision ip1_weights_trans[9408];
 
struct shapes{
    TsInt conv1_shape[12];
    TsInt relu1_shape;
    TsInt pool1_shape[11];
    TsInt conv2_shape[12];
    TsInt relu2_shape;
    TsInt conv3_shape[12];
    TsInt relu3_shape;
    TsInt pool2_shape[15];
    TsInt ip1_shape[4];
    TsInt softmax1_shape;
};
 
struct shapes layer_shapes ={
    {1,3,64,64,8,3,3,3,62,62,1,1},
    30752,
    {1,8,62,62,31,31,2,2,2,2,0},
    {1,8,31,31,16,8,3,3,29,29,1,1},
    13456,
    {1,16,29,29,16,16,3,3,27,27,1,1},
    11664,
    {1,16,27,27,14,14,0,1,0,1,2,2,2,2,0},
    {1,3136,3,3136},
    3
};
 
#endif
