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
* File Name    : dnn_compute.c
* Version      : 1.00
* Description  : The function calls
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.06.2017 1.00     First Release
***********************************************************************************************************************/

 
#include "layer_shapes.h"
#include "layer_graph.h"
#include "weights.h"
 
TPrecision* dnn_compute(TPrecision* input_img)
{
  convolution_without_pad(input_img,conv1_weights,conv1_biases,conv1_out,layer_shapes.conv1_shape);
  relu(conv1_out,layer_shapes.relu1_shape);
  pooling_without_pad(conv1_out,pool1_out,layer_shapes.pool1_shape);
  convolution_without_pad(pool1_out,conv2_weights,conv2_biases,conv2_out,layer_shapes.conv2_shape);
  relu(conv2_out,layer_shapes.relu2_shape);
  convolution_without_pad(conv2_out,conv3_weights,conv3_biases,conv3_out,layer_shapes.conv3_shape);
  relu(conv3_out,layer_shapes.relu3_shape);
  pooling(conv3_out,pool2_pad,pool2_out,layer_shapes.pool2_shape);
   
  transpose(ip1_weights,ip1_weights_trans,layer_shapes.ip1_shape);
  innerproduct(pool2_out,ip1_weights_trans,ip1_biases,ip1_out,layer_shapes.ip1_shape);
  softmax(ip1_out,layer_shapes.softmax1_shape);
  return(ip1_out);
}
