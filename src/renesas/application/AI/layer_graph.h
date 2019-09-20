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
* File Name    : layer_graph.h
* Version      : 1.00
* Description  : Declarations of all functions
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.06.2017 1.00     First Release
***********************************************************************************************************************/

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
   
void padding(TPrecision *, TPrecision *, TsInt *);
void convolution_without_pad(TPrecision *,const TPrecision *,const TPrecision *,TPrecision *,TsInt *);
void max_pooling(TPrecision *, TPrecision *, TPrecision *, TsInt *);
void average_pooling(TPrecision *, TPrecision *, TPrecision *, TsInt *);
void pooling(TPrecision *, TPrecision *, TPrecision *, TsInt *);
void max_pooling_without_pad(TPrecision *, TPrecision *, TsInt *);
void average_pooling_without_pad(TPrecision *, TPrecision *, TsInt *);
void pooling_without_pad(TPrecision *, TPrecision *, TsInt *);
void innerproduct(TPrecision *,TPrecision *,const TPrecision *,TPrecision *,TsInt *);
void relu(TPrecision *, TsInt );
void softmax( TPrecision *, TsInt  );
void transpose(const TPrecision *,TPrecision *,TsInt*);

#endif
