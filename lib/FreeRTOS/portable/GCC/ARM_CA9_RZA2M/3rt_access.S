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
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : access.S
* Device(s)     : RZ/A2M
* Tool-Chain    : GNUARM-RZv13.01-EABI
* H/W Platform  : RZ/A2M EBK CPU Board
* Description   : This is the code provides access to the co-processors
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 18.06.2013 1.00
*               : 21.03.2014 2.00
*               : 08.04.2019 3.00 RZA2M version 
*******************************************************************************/

    .text
    .code 32

        .global get_apsr
        .global set_fpscr

/* ========================================================================== */
/* getAPSR                                                                   */
/* ========================================================================== */
	.func get_apsr
get_apsr:
/* ========================================================================== */
/* Get Application Program Status Register contents                           */
/* ========================================================================== */
	MRS  r0, APSR
	ISB
	BX		lr
	.endfunc
	.align 4

/* ========================================================================== */
/* setFPSCR                                                                   */
/* ========================================================================== */
	.func set_fpscr
set_fpscr:
/* ========================================================================== */
/* Set Floating Point Status and Control Register contents                    */
/* ========================================================================== */
	FMXR 	FPSCR, r0
	ISB
	BX		lr
	.endfunc
	.align 4
	.end

    
    
