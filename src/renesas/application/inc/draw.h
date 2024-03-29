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
* Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         draw.h
* @version      0.01
* @brief        graphics draw sample header
******************************************************************************/

#ifndef GRA_DRAW_H
#define GRA_DRAW_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include <stdlib.h> 

#include    "r_typedefs.h"
#include    "r_vdc.h"

/******************************************************************************
Macro definitions
******************************************************************************/
typedef struct
{
    uint8_t  * buff_addr1;
    uint8_t  * buff_addr2;
    uint32_t   buff_select;
    uint32_t   buff_stride;
    uint32_t   buff_bit_per_byte;
    uint32_t   window_width;
    uint32_t   window_height;
} draw_buffer_info;

typedef enum
{
    DRAW_LAYER0 = 0,
    DRAW_LAYER1,
    DRAW_LAYER2,
    DRAW_LAYER3,
    DRAW_LAYER_NUM,
} draw_layer_t;

/******************************************************************************
Typedef definitions
******************************************************************************/

#define CLUT_TABLE_MAX 16u
#define BLACK_DATA       (0xFF000000u)
#define BLUE_DATA        (0xFF0000FFu)
#define GREEN_DATA       (0xFF00FF00u)
#define RED_DATA         (0xFFFF0000u)
#define WHITE_DATA       (0xFFFFFFFFu)
#define CYAN_DATA        (0xFF00FFFFu)
#define YELLOW_DATA      (0xFFFFFF00u)
#define MAGENTA_DATA     (0xFFFF00FFu)
#define NAVY_DATA        (0xFF000080u)
#define DARKGREEN_DATA   (0xFF006400u)
#define DEEPSKYBLUE_DATA (0xFF00BFFFu)
#define PURPLE_DATA      (0xFF800080u)
#define GRAY_DATA        (0xFF808080u)
#define BROWN_DATA       (0xFFA52A2Au)
#define GOLD_DATA        (0xFFFFD780u)
#define TRANSPARENT_DATA (0x00FFFFFFu)

#define BLACK       (0u)
#define BLUE        (1u)
#define GREEN       (2u)
#define RED         (3u)
#define WHITE       (4u)
#define CYAN        (5u)
#define YELLOW      (6u)
#define MAGENTA     (7u)
#define NAVY        (8u)
#define DARKGREEN   (9u)
#define DEEPSKYBLUE (10u)
#define PURPLE      (11u)
#define GRAY        (12u)
#define BROWN       (13u)
#define GOLD        (14u)
#define TRANSPARENT (15u)

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
void DrawInit( vdc_layer_id_t layer_id, draw_buffer_info * info );
void DrawSwapBuffer(vdc_layer_id_t layer_id);
void * DrawGetBackBuffer (vdc_layer_id_t layer_id);
void DrawPoint( vdc_layer_id_t layer_id, int32_t x, int32_t y , uint8_t color );
void DrawLine( vdc_layer_id_t layer_id, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color );
void DrawRect( vdc_layer_id_t layer_id, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color);
void DrawRectFill( vdc_layer_id_t layer_id, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color);
void DrawFont1bpp( vdc_layer_id_t layer_id,
		           int32_t x, int32_t y,
		           int32_t fwidth, int32_t fheight,
		           uint8_t fcolor, uint8_t bcolor,
		           uint8_t *font_data );
void DrawString( vdc_layer_id_t layer_id,
                        uint32_t x,      uint32_t y,
                        uint8_t  fcolor, uint8_t  bcolor,
                        int8_t * buff,   uint32_t len);
#endif  /* GRA_DRAW_H */
