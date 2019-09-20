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
#include <string.h>

#include "r_typedefs.h"
#include "iodefine.h"
#include "r_cache_lld_rza2m.h"

#include "r_bcd_ae.h"
#include "r_bcd_camera.h"
#include "r_bcd_lcd.h"
#include "r_bcd_ai.h"
#include "draw.h"
#include "perform.h"
#include "r_mmu_lld_rza2m.h"

#include "r_dk2_if.h"
#include "r_drp_simple_isp.h"
#include "r_drp_resize_bilinear_fixed.h"

#include "r_drp_gaussian_blur.h"
#include "r_drp_canny_calculate.h"
#include "r_drp_canny_hysterisis.h"
#include "r_drp_dilate.h"
#include "r_drp_find_contours.h"
#include "r_drp_bayer2rgb_color_correction.h"
#include "r_drp_cropping.h"
#include "r_drp_histogram_normalization/r_drp_histogram_normalization.h"
#include "r_drp_histogram_normalization_rgb/r_drp_histogram_normalization_rgb.h"
#include "r_drp_cropping_rgb/r_drp_cropping_rgb.h"
#include "r_drp_resize_bilinear_fixed_rgb/r_drp_resize_bilinear_fixed_rgb.h"
#include "r_drp_bayer2rgb/r_drp_bayer2rgb.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
#define TILE_0          (0)
#define TILE_1          (1)
#define TILE_2          (2)
#define TILE_3          (3)
#define TILE_4          (4)
#define TILE_5          (5)

#define DRP_NOT_FINISH  (0)
#define DRP_FINISH      (1)

#define DRP_DRV_ASSERT(x) if ((x) != 0) while(1);
#define WCHAR(a,s,x) sprintf((char *)&a[0],"%s %d.%d[ms]", s, (int)(x / 1000), (int)((x - ((x / 1000) * 1000)) / 100))

#define WEAK_LEVEL      (0x28)
#define STRONG_LEVEL    (0x58)

#define HYSTERISIS_COUNT  (2)
#define FIND_CONTOURS_NUM (15)

/* DRP Parameter of Resize bilinear fixed */
#define DRP_RESIZE_HALF         (0x20)

#define R_BCD_BAYER_RGB

/* Offset lines for Layer0 display */
#define LAYER0_DISP_OFFSET      (240)

/* Display result string length */
#define RESULT_DISP_BUF_SIZE    (80)

/* ISP parameter */
#define ISP_GAIN_R              (0x1500)
#define ISP_GAIN_G              (0x1000)
#define ISP_GAIN_B              (0x18E3)
#define ISP_GAMMA               (0x01)
#define ISP_COMPONENT           (0x01)
#define ISP_BIAS_R              (0x0000)
#define ISP_BIAS_G              (0x0000)
#define ISP_BIAS_B              (0x0000)
#define ISP_MEDIAN_A            (0x0100)
#define ISP_STRENGTH            (28)
#define ISP_CORING              (60)
#define GAMMA_VALUE             ((double)1.2)
#define AE_MARGIN               (BRIGHTNESS_TH * 3)

/* Status of key_status */
#define KEY_STAUS_INIT  (0xFFFFFFFFU)
#define KEY_CHECK_BIT   (0x00000003U)
#define KEY_JUST_ON     (0x00000002U)

#define R_BCD_AI_INPUT_64
#if defined(R_BCD_AI_INPUT_128)
#define R_BCD_AI_INPUT_X 	128
#define R_BCD_AI_INPUT_Y 	128
#elif defined(R_BCD_AI_INPUT_64)
#define R_BCD_AI_INPUT_X 	64
#define R_BCD_AI_INPUT_Y 	64
#endif

#define R_BCD_RESIZE_LENGTH_X 	R_BCD_AI_INPUT_X
#define R_BCD_RESIZE_LENGTH_Y 	R_BCD_AI_INPUT_Y

#define R_BCD_DRP_CROP_NUM 4
#define R_BCD_DRP_CROP_SQUARE

#define R_BCD_PRESTIC_LENDGTH_MIN 1
#define R_BCD_DRP_CROP_LENGTH_MIN 32
#define R_BCD_DRP_CROP_LENGTH_MAX 128
#define R_BCD_DRP_CROP_LENGTH_DIFF 10
#define R_BCD_DRP_IMAGE_RESIZE 2

/* Status of isp_ae_mode */
#define ISP_AE_OFF  (0)
#define ISP_AE_ON   (1)
/*******************************************************************************
Imported global variables and functions (from other files)
*******************************************************************************/
extern uint8_t g_drp_lib_isp_bayer2grayscale_6[];
extern uint8_t g_drp_lib_resize_bilinear_fixed[];

extern uint8_t g_drp_lib_gaussian_blur[];
extern uint8_t g_drp_lib_canny_calculate[];
extern uint8_t g_drp_lib_canny_hysterisis[];

extern uint8_t g_drp_lib_dilate[];
extern uint8_t g_drp_lib_find_contours[];
extern uint8_t g_drp_lib_bayer2rgb_color_correction[];
extern uint8_t g_drp_lib_cropping[];
extern uint8_t g_drp_lib_histogram_normalization[];

extern uint8_t g_drp_histogram_normalization_rgb[];
extern uint8_t g_drp_cropping_rgb[];
extern uint8_t g_drp_resize_bilinear_fixed_rgb[];
extern uint8_t g_drp_bayer2rgb[];

typedef struct
{
	uint16_t	x;
	uint16_t	y;
	uint16_t	width;
	uint16_t	height;
	uint32_t	num;
	uint32_t	addr;
} contours_rect_t;

typedef struct
{
	uint16_t	x;
	uint16_t	y;
} contours_region_t;

/*******************************************************************************
Private global variables and functions
*******************************************************************************/
static r_drp_simple_isp_t param_isp __attribute__ ((section("Uncache_IRAM")));
static uint8_t look_up_table[256] __attribute__ ((section("Uncache_IRAM")));
static uint32_t ave_result[9] __attribute__ ((section("Uncache_IRAM")));

static uint8_t drp_lib_id[R_DK2_TILE_NUM] = {0};
static volatile uint8_t drp_lib_status[R_DK2_TILE_NUM] = {DRP_NOT_FINISH};

static r_drp_resize_bilinear_fixed_t param_resize __attribute__ ((section("Uncache_IRAM")));
static r_drp_dilate_t param_dilate[R_DK2_TILE_NUM] __attribute__ ((section("Uncache_IRAM")));
static contours_rect_t   contours_rect_adr[FIND_CONTOURS_NUM + 1]  __attribute__ ((section("Uncache_IRAM")));
static contours_region_t contours_region_adr[FIND_CONTOURS_NUM + 1]  __attribute__ ((section("Uncache_IRAM")));

static r_drp_gaussian_blur_t param[R_DK2_TILE_NUM] __attribute__ ((section("Uncache_IRAM")));
static r_drp_canny_calculate_t param_canny_cal[3] __attribute__ ((section("Uncache_IRAM")));
static r_drp_canny_hysterisis_t param_canny_hyst __attribute__ ((section("Uncache_IRAM")));

static r_drp_find_contours_t param_find_contours __attribute__ ((section("Uncache_IRAM")));

static r_drp_histogram_normalization_t parm_hist[R_DK2_TILE_NUM] __attribute((section("Uncache_IRAM")));
static r_drp_histogram_normalization_output_mode1_t histGRY_output_data[R_DK2_TILE_NUM] __attribute((aligned(32))) = {0x0,0x0};

static r_drp_cropping_t parm_crop_gry __attribute((section("Uncache_IRAM")));
static uint16_t app_data_cropping[4] __attribute__ ((section("Uncache_IRAM_DATA"))) = {0x00, 0x00, 0x00, 0x00};

#ifdef R_BCD_BAYER_RGB
static r_drp_cropping_rgb_t parm_crop_rgb __attribute((section("Uncache_IRAM")));
static r_drp_resize_bilinear_fixed_rgb_t parm_resize_rgb __attribute((section("Uncache_IRAM")));
static r_drp_histogram_normalization_rgb_t parm_histogram_rgb[6] __attribute((section("Uncache_IRAM")));
static r_drp_histogram_normalization_rgb_mode1_dst_t histRGB_output_data[6] __attribute((section("Uncache_IRAM")));
static r_drp_bayer2rgb_t param_b2rgb[3] __attribute((section("Uncache_IRAM")));
#endif

static uint8_t frame_RAM[4][R_BCD_CAMERA_HEIGHT * R_BCD_CAMERA_WIDTH] __attribute__ ((section("ImageWork_RAM")));

static void cb_drp_finish(uint8_t id);
static void write_canny_data(uint8_t *psrc, uint8_t *pdst);

static uint16_t R_BCD_DrpGetCropLength(uint16_t length);
static uint16_t R_BCD_DrpGetCropLength_longer(uint16_t x, uint16_t y);
static uint32_t R_BCD_DrpSetCropParam(uint32_t i, r_drp_cropping_t *crop);
static uint32_t R_BCD_DrpCheckEmptyContours(uint32_t i);
static int16_t R_BCD_DrpGetResizeScale(uint16_t input_length, uint16_t output_length);
static uint8_t R_BCD_DrpCheckCropParam_AIsize(void);
static void R_BCD_DrpDeleteContours(void);
static uint32_t R_BCD_DrpSortContours (void);
static void R_BCD_DecomposeRGB(uint32_t input_address, uint32_t output_address);

static int R_BCD_DRP_do_CroppResizeAI_RGB ( uint32_t input_address );

static uint8_t* eAI_Crop_rgb;
static uint8_t* eAI_Resize_rgb;
static uint8_t* eAI_InBuffer;

static float ai_result = 0.0;

/*******************************************************************************
* Function Name: cb_drp_finish
* Description  : This function is a callback function called from the
*              : DRP driver at the finish of the DRP library processing.
* Arguments    : id
*              :   The ID of the DRP library that finished processing.
* Return Value : -
*******************************************************************************/
static void cb_drp_finish(uint8_t id)
{
    uint32_t tile_no;

    /* Change the operation state of the DRP library notified by the argument to finish */
    for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)
    {
        if (drp_lib_id[tile_no] == id)
        {
            drp_lib_status[tile_no] = DRP_FINISH;
            break;
        }
    }

    return;
}
/*******************************************************************************
* End of function cb_drp_finish
*******************************************************************************/

/******************************************************************************
* Function Name: write_canny_data
* Description  : Write Canny(Hysteresis) output result to the overlapping buffer
* Arguments    : psrc
*              :   Pointer of input image.
*              : pdst
*              :   Pointer of Output buffer.
* Return Value : -
******************************************************************************/
static void write_canny_data(uint8_t *psrc, uint8_t *pdst)
{
    uint32_t *psrc_w = (uint32_t *)psrc;
    uint32_t *pdst_h = (uint32_t *)pdst;
    uint32_t i,j;

    /* Loop for one screen */
    for (i = 0; i < (R_BCD_CAMERA_HEIGHT / 2); i++)
    {
       for (j = 0; j < ((R_BCD_CAMERA_WIDTH / 2) / 4); j++)
    	{
            /* Write patterns corresponding to the data output by the DRP application in the overlapping buffer */
            switch (*psrc_w)
            {
                case 0x000000FF:
                    pdst_h[0]  = 0xFFFFFF55;
                    pdst_h[80] = 0xFFFFFF55;
                    break;
                case 0x0000FF00:
                    pdst_h[0]  = 0xFFFF55FF;
                    pdst_h[80] = 0xFFFF55FF;
                    break;
                case 0x0000FFFF:
                    pdst_h[0]  = 0xFFFF5555;
                    pdst_h[80] = 0xFFFF5555;
                    break;
                case 0x00FF0000:
                    pdst_h[0]  = 0xFF55FFFF;
                    pdst_h[80] = 0xFF55FFFF;
                    break;
                case 0x00FF00FF:
                    pdst_h[0]  = 0xFF55FF55;
                    pdst_h[80] = 0xFF55FF55;
                    break;
                case 0x00FFFF00:
                    pdst_h[0]  = 0xFF5555FF;
                    pdst_h[80] = 0xFF5555FF;
                    break;
                case 0x00FFFFFF:
                    pdst_h[0]  = 0xFF555555;
                    pdst_h[80] = 0xFF555555;
                    break;
                case 0xFF000000:
                    pdst_h[0]  = 0x55FFFFFF;
                    pdst_h[80] = 0x55FFFFFF;
                    break;
                case 0xFF0000FF:
                    pdst_h[0]  = 0x55FFFF55;
                    pdst_h[80] = 0x55FFFF55;
                    break;
                case 0xFF00FF00:
                    pdst_h[0]  = 0x55FF55FF;
                    pdst_h[80] = 0x55FF55FF;
                    break;
                case 0xFF00FFFF:
                    pdst_h[0]  = 0x55FF5555;
                    pdst_h[80] = 0x55FF5555;
                    break;
                case 0xFFFF0000:
                    pdst_h[0]  = 0x5555FFFF;
                    pdst_h[80] = 0x5555FFFF;
                    break;
                case 0xFFFF00FF:
                    pdst_h[0]  = 0x5555FF55;
                    pdst_h[80] = 0x5555FF55;
                    break;
                case 0xFFFFFF00:
                    pdst_h[0]  = 0x555555FF;
                    pdst_h[80] = 0x555555FF;
                    break;
                case 0xFFFFFFFF:
                    pdst_h[0]  = 0x55555555;
                    pdst_h[80] = 0x55555555;
                    break;
                default : break;
            }

            psrc_w++;
            pdst_h++;
        }
        pdst_h = pdst_h + 80;
    }
    return;
}
/*******************************************************************************
* End of function write_canny_data
*******************************************************************************/

/*******************************************************************************
* Function Name: sample_main
* Description  : First function called after initialization is completed
* Arguments    : -
* Return Value : -
*******************************************************************************/
void sample_main(void)
{
    int32_t frame_buf_id;
    uint8_t buf[RESULT_DISP_BUF_SIZE];
    int32_t ret_val;
    uint32_t us;

    static uint8_t *input_bufadr;
    static uint8_t *output_bufadr;

	uint32_t resize_width;
	uint32_t resize_height;

    uint8_t color;
    uint32_t x1 = 60;
    uint32_t y1 = 60;
    uint32_t length = 0;
    uint32_t height = 0;

    uint16_t brightness;
    r_bcd_ae_setting_t ae_setting;

    uint32_t key_status;
    uint32_t isp_ae_mode;

    uint32_t tile_no;
    uint32_t bufsize;
    
    uint32_t loop_count;

    uint8_t* frame_RAM1;
    uint8_t* frame_RAM2;
    uint8_t* frame_RAM3;

    uint32_t contors_no = 0;

    frame_RAM1 = &frame_RAM[0];
    frame_RAM2 = &frame_RAM[1];
    frame_RAM3 = &frame_RAM[2];
    eAI_Crop_rgb = &frame_RAM[3];
    eAI_Resize_rgb = eAI_Crop_rgb + 0x1000;
    eAI_InBuffer = frame_RAM1;


    /**************/
    /* Initialize */
    /**************/

    /* VDC6 L2 Cache ON */
    PRR.AXIBUSCTL6.BIT.VDC601ARCACHE = 0xF;
    PRR.AXIBUSCTL6.BIT.VDC602ARCACHE = 0xF;
    PRR.AXIBUSCTL7.BIT.VDC604ARCACHE = 0xF;

    /* DRP L2 Cache ON */
    PRR.AXIBUSCTL4.BIT.DRPARCACHE = 0xF;
    PRR.AXIBUSCTL4.BIT.DRPAWCACHE = 0xF;

    /* Initialization of VIN and MIPI driver */
    R_BCD_CameraInit();

    /* Initialization of LCD driver */
    R_BCD_LcdInit();

    /* Key status initialize */
    key_status = KEY_STAUS_INIT;

    /* Initialize AE mode (=ON) */
    isp_ae_mode = ISP_AE_ON;

    /* Capture Start */
    R_BCD_CameraClearCaptureStatus();
    R_BCD_CameraCaptureStart();

    /* Initialize of Performance counter */
    PerformInit();

    /* Initialization of DRP driver */
    R_DK2_Initialize();

    /* Initialization of Simple ISP */
    R_BCD_AeInit(&ae_setting);
    R_BCD_AeMakeGammaTable(&ae_setting, GAMMA_VALUE, look_up_table);

    /*************/
    /* Main loop */
    /*************/
    while (1)
    {

        /* Get key */
        key_status = key_status << 1 | PORTJ.PIDR.BIT.PIDR1;
        if ( ( key_status & KEY_CHECK_BIT) == KEY_JUST_ON )
        {
            /* Change AE mode On/Off */
            if ( isp_ae_mode == ISP_AE_OFF )
            {
                R_BCD_AeStart(&ae_setting);
                isp_ae_mode = ISP_AE_ON;
            }
            else
            {
                R_BCD_AeStop(&ae_setting);
                isp_ae_mode = ISP_AE_OFF;
            }
        }

/* Function : Simple ISP(AE, Bayer to grayscale conversion, Noise reduction) */

        /******************************/
        /* Load DRP Library           */
        /*        +-----------------+ */
        /* tile 0 |IspBayer2Gray    | */
        /*        |                 | */
        /* tile 1 |                 | */
        /*        |                 | */
        /* tile 2 |                 | */
        /*        |                 | */
        /* tile 3 |                 | */
        /*        |                 | */
        /* tile 4 |                 | */
        /*        |                 | */
        /* tile 5 |                 | */
        /*        +-----------------+ */
        /******************************/
        ret_val = R_DK2_Load(&g_drp_lib_isp_bayer2grayscale_6[0],
                             R_DK2_TILE_0,
                             R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Wait until camera capture is complete */
        while ((frame_buf_id = R_BCD_CameraGetCaptureStatus()) == R_BCD_CAMERA_NOT_CAPTURED)
        {
            /* DO NOTHING */
        }

        /* Set start time of process*/
        PerformSetStartTime(0);

        /* Start isp */
        /* Set ISP parameters */
        input_bufadr  = R_BCD_CameraGetFrameAddress(frame_buf_id);
        output_bufadr = R_BCD_LcdGetVramAddress();
        //output_bufadr = (uint32_t)frame_RAM3;

        R_MMU_VAtoPA((uint32_t)input_bufadr, &(param_isp.src));
        R_MMU_VAtoPA((uint32_t)output_bufadr, &(param_isp.dst));

        param_isp.width             = R_BCD_CAMERA_WIDTH;
        param_isp.height            = R_BCD_CAMERA_HEIGHT;
        R_MMU_VAtoPA((uint32_t)look_up_table, &(param_isp.table));
        param_isp.gain_r            = ISP_GAIN_R;
        param_isp.gain_g            = ISP_GAIN_G;
        param_isp.gain_b            = ISP_GAIN_B;
        param_isp.gamma             = ISP_GAMMA;
        param_isp.component         = ISP_COMPONENT;
        param_isp.bias_r            = ISP_BIAS_R;
        param_isp.bias_g            = ISP_BIAS_G;
        param_isp.bias_b            = ISP_BIAS_B;
        param_isp.blend             = ISP_MEDIAN_A;
        param_isp.strength          = ISP_STRENGTH;
        param_isp.coring            = ISP_CORING;

        param_isp.area1_offset_x    = 0;
        param_isp.area1_offset_y    = 0;
        param_isp.area1_width       = R_BCD_CAMERA_WIDTH;
        param_isp.area1_height      = R_BCD_CAMERA_HEIGHT;
        param_isp.area2_offset_x    = 0;
        param_isp.area2_offset_y    = 0;
        param_isp.area2_width       = 0;
        param_isp.area2_height      = 0;
        param_isp.area3_offset_x    = 0;
        param_isp.area3_offset_y    = 0;
        param_isp.area3_width       = 0;
        param_isp.area3_height      = 0;
        R_MMU_VAtoPA((uint32_t)ave_result, &(param_isp.accumulate));

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_isp, sizeof(r_drp_simple_isp_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            /* Wait drp finish */
        }

        /* Set AE */
        {
            brightness = (uint16_t)((ave_result[1]) / (R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT / 2));
        }
        R_BCD_AeRunAutoExpousure(&ae_setting, brightness);

        /**********************/
        /* Unload DRP Library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /* Clear the current capture state and enable the detection of the next capture completion */
        R_BCD_CameraClearCaptureStatus();


/* Function : Resize Bilinear Fixed */
        /**************************/
        /* Load DRP Library       */
        /*        +-------------+ */
        /* tile 0 | Resize      | */
        /*        | Bilinear    | */
        /* tile 1 | Fixed       | */
        /*        |             | */
        /* tile 2 |             | */
        /*        |             | */
        /* tile 3 |             | */
        /*        +-------------+ */
        /* tile 4 | (not use)   | */
        /*        +-------------+ */
        /* tile 5 | (not use)   | */
        /*        +-------------+ */
        /**************************/

        ret_val = R_DK2_Load(&g_drp_lib_resize_bilinear_fixed[0],
                             R_DK2_TILE_0 ,
                             R_DK2_TILE_PATTERN_4_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);
         /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Start resize */


        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/

        /* Set the address of buffer to be read/write by DRP */
        param_resize.src = (uint32_t)output_bufadr;
        param_resize.dst = (uint32_t)frame_RAM1;

        R_MMU_VAtoPA((uint32_t)param_resize.src, &(param_resize.src ));
        R_MMU_VAtoPA((uint32_t)param_resize.dst, &(param_resize.dst ));

        /* Set Image size */
        param_resize.src_width = R_BCD_CAMERA_WIDTH;
        param_resize.src_height = R_BCD_CAMERA_HEIGHT;

        /* Set magnification */
        param_resize.fx = DRP_RESIZE_HALF;
        param_resize.fy = DRP_RESIZE_HALF;

        /* Set Image size */
        resize_width = R_BCD_CAMERA_WIDTH / 2;
        resize_height = R_BCD_CAMERA_HEIGHT / 2;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_resize, sizeof(r_drp_resize_bilinear_fixed_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            /* Wait drp finish */
        }

        /**********************/
        /* Unload DRP library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);


#if(1)
        /*****************************/
        /* Load DRP Library          */
        /*        +----------------+ */
        /* tile 0 | Gaussian Blur  | */
        /*        +----------------+ */
        /* tile 1 | Gaussian Blur  | */
        /*        +----------------+ */
        /* tile 2 | Gaussian Blur  | */
        /*        +----------------+ */
        /* tile 3 | Gaussian Blur  | */
        /*        +----------------+ */
        /* tile 4 | Gaussian Blur  | */
        /*        +----------------+ */
        /* tile 5 | Gaussian Blur  | */
        /*        +----------------+ */
        /*****************************/

        ret_val = R_DK2_Load(&g_drp_lib_gaussian_blur[0],
        					R_DK2_TILE_0 | R_DK2_TILE_1 | R_DK2_TILE_2 | R_DK2_TILE_3 | R_DK2_TILE_4 | R_DK2_TILE_5,
							R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3] | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(2);
        /**********************************************************/
         /* Set R_DK2_Start function parameters　for　Gaussian Blur */
         /*********************************************************/
        /* Set the address of buffer to be read/write by DRP */
        for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)	//tile[0]～tile[5]
        {
            /* Set Image size */
            param[tile_no].width  = resize_width;
            param[tile_no].height = resize_height / R_DK2_TILE_NUM;

        	/* Set the address of buffer to be read/write by DRP */
            param[tile_no].src = (uint32_t)&frame_RAM1[(param[tile_no].width * param[tile_no].height * tile_no)];
            param[tile_no].dst = (uint32_t)&frame_RAM2[(param[tile_no].width * param[tile_no].height * tile_no)];
            R_MMU_VAtoPA((uint32_t)param[tile_no].src, &(param[tile_no].src ));
            R_MMU_VAtoPA((uint32_t)param[tile_no].dst, &(param[tile_no].dst ));

            /* Set whether to perform top or bottom edge border processing. */
            param[tile_no].top = (tile_no == TILE_0) ? 1 : 0;
            param[tile_no].bottom = (tile_no == TILE_5) ? 1 : 0;

            /* Initialize variables to be used in termination judgment of the DRP application */
            drp_lib_status[tile_no] = DRP_NOT_FINISH;

            /*********************/
            /* Start DRP Library */
            /*********************/
            ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *)&param[tile_no], sizeof(r_drp_gaussian_blur_t));
            DRP_DRV_ASSERT(ret_val);
        }
        /***************************************/
         /* Wait until DRP processing is finish */
         /***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_5; tile_no += 1)
        {
            /***************************************/
            /* Wait until DRP processing is finish */
            /***************************************/
            while (drp_lib_status[tile_no] == DRP_NOT_FINISH);
        }

         /* Set end time of DRP processing */
         PerformSetEndTime(2);

         /**********************/
         /* Unload DRP library */
         /**********************/
         ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] |drp_lib_id[TILE_2] |drp_lib_id[TILE_3] | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], &drp_lib_id[TILE_0]);
         DRP_DRV_ASSERT(ret_val);

#if(0)
		for(loop_count = 0; loop_count < (resize_width * resize_height); loop_count++)
		{
			output_bufadr[loop_count] = frame_RAM2[loop_count];
		}
#endif
#endif
#if(1)
        /******************************/
        /* Load DRP Library           */
        /*        +-----------------+ */
        /* tile 0 |                 | */
        /*        + Canny Calculate + */
        /* tile 1 |                 | */
        /*        +-----------------+ */
        /* tile 2 |                 | */
        /*        + Canny Calculate + */
        /* tile 3 |                 | */
        /*        +-----------------+ */
        /* tile 4 |                 | */
        /*        + Canny Calculate + */
        /* tile 5 |                 | */
        /*        +-----------------+ */
        /******************************/
        ret_val = R_DK2_Load(&g_drp_lib_canny_calculate[0],
                             R_DK2_TILE_0 | R_DK2_TILE_2 | R_DK2_TILE_4,
                             R_DK2_TILE_PATTERN_2_2_2, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /********************/
        /* Activate DRP Lib */
        /********************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0] | drp_lib_id[TILE_2] | drp_lib_id[TILE_4], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(2);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_4; tile_no += 2)
        {
            /* Set Image size */
            param_canny_cal[tile_no / 2].width = resize_width;
            param_canny_cal[tile_no / 2].height = (resize_height / 3);

            /* Set Buffer size */
            bufsize = (uint32_t)(param_canny_cal[tile_no / 2].width * param_canny_cal[tile_no / 2].height);

            /* Set the address of buffer to be read/write by DRP */
            param_canny_cal[tile_no / 2].src = (uint32_t)&frame_RAM2[((tile_no / 2) * bufsize)];
            param_canny_cal[tile_no / 2].dst = (uint32_t)&frame_RAM3[((tile_no / 2) * bufsize)];
            R_MMU_VAtoPA((uint32_t)param_canny_cal[tile_no/2].src, &(param_canny_cal[tile_no/2].src ));
            R_MMU_VAtoPA((uint32_t)param_canny_cal[tile_no/2].dst, &(param_canny_cal[tile_no/2].dst ));

            /* Set additional information (currently no additional information) */
            param_canny_cal[tile_no / 2].work = (uint32_t)&frame_RAM1[((tile_no / 2) * (bufsize + (resize_width*2) ))*2];
            R_MMU_VAtoPA((uint32_t)param_canny_cal[tile_no/2].work, &(param_canny_cal[tile_no/2].work ));

            param_canny_cal[tile_no / 2].threshold_high = STRONG_LEVEL;
            param_canny_cal[tile_no / 2].threshold_low = WEAK_LEVEL;

            /* Set whether to perform top or bottom edge border processing. */
            param_canny_cal[tile_no / 2].top = (tile_no == TILE_0) ? 1 : 0;
            param_canny_cal[tile_no / 2].bottom = (tile_no == TILE_4) ? 1 : 0;

            /* Initialize variables to be used in termination judgment of the DRP library */
            drp_lib_status[tile_no] = DRP_NOT_FINISH;

            /*********************/
            /* Start DRP Library */
            /*********************/
            ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *)&param_canny_cal[tile_no / 2], sizeof(r_drp_canny_calculate_t));
            DRP_DRV_ASSERT(ret_val);
        }

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_4; tile_no += 2)
        {
            /***************************************/
            /* Wait until DRP processing is finish */
            /***************************************/
            while (drp_lib_status[tile_no] == DRP_NOT_FINISH);
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(2);

        /**********************/
        /* Unload DRP library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_2] | drp_lib_id[TILE_4], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /*******************************/
        /* Load DRP Library            */
        /*        +------------------+ */
        /* tile 0 |                  | */
        /*        +                  + */
        /* tile 1 |                  | */
        /*        +                  + */
        /* tile 2 |                  | */
        /*        + Canny Hysterisis + */
        /* tile 3 |                  | */
        /*        +                  + */
        /* tile 4 |                  | */
        /*        +                  + */
        /* tile 5 |                  | */
        /*        +------------------+ */
        /*******************************/
        ret_val = R_DK2_Load(&g_drp_lib_canny_hysterisis[0],
                             R_DK2_TILE_0,
                             R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of Canny Hyst process */
        PerformSetStartTime(3);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        /* Set Image size */
        param_canny_hyst.width = resize_width;
        param_canny_hyst.height = resize_height;

        /* Set the address of buffer to be read/write by DRP */
        param_canny_hyst.src = (uint32_t)frame_RAM3;
        param_canny_hyst.dst = (uint32_t)frame_RAM2;
        R_MMU_VAtoPA((uint32_t)param_canny_hyst.src, &(param_canny_hyst.src ));
        R_MMU_VAtoPA((uint32_t)param_canny_hyst.dst, &(param_canny_hyst.dst ));

        /* Set additional information (currently no additional information) */
        param_canny_hyst.work = (uint32_t)&frame_RAM1[0];
        R_MMU_VAtoPA((uint32_t)param_canny_hyst.work, &(param_canny_hyst.work ));
        param_canny_hyst.iterations = HYSTERISIS_COUNT;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_canny_hyst, sizeof(r_drp_canny_hysterisis_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH);

        /* Set end time of DRP processing */
        PerformSetEndTime(3);

        /**********************/
        /* Unload DRP Library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

#endif

#if(1)
        /*****************************/
        /* Load DRP Library          */
        /*        +----------------+ */
        /* tile 0 | Dilate         | */
        /*        +----------------+ */
        /* tile 1 | Dilate         | */
        /*        +----------------+ */
        /* tile 2 | Dilate         | */
        /*        +----------------+ */
        /* tile 3 | Dilate         | */
        /*        +----------------+ */
        /* tile 4 | Dilate         | */
        /*        +----------------+ */
        /* tile 5 | Dilate         | */
        /*        +----------------+ */
        /*****************************/

        ret_val = R_DK2_Load(&g_drp_lib_dilate[0],
        					R_DK2_TILE_0 | R_DK2_TILE_1 | R_DK2_TILE_2 | R_DK2_TILE_3 | R_DK2_TILE_4 | R_DK2_TILE_5,
							R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);	//エラーチェック

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3] | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(2);
        /**********************************************************/
         /* Set R_DK2_Start function parameters　for　Gaussian Blur */
         /*********************************************************/
        /* Set the address of buffer to be read/write by DRP */
        for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)	//tile[0]～tile[5]
        {
            /* Set Image size */
        	param_dilate[tile_no].width = resize_width;
        	param_dilate[tile_no].height = resize_height / R_DK2_TILE_NUM;

         	param_dilate[tile_no].src = (uint32_t)&frame_RAM2[(param_dilate[tile_no].width * param_dilate[tile_no].height * tile_no)];
        	param_dilate[tile_no].dst = (uint32_t)&frame_RAM3[(param_dilate[tile_no].width * param_dilate[tile_no].height * tile_no)];
            R_MMU_VAtoPA((uint32_t)param_dilate[tile_no].src, &(param_dilate[tile_no].src ));
            R_MMU_VAtoPA((uint32_t)param_dilate[tile_no].dst, &(param_dilate[tile_no].dst ));

            param_dilate[tile_no].width = resize_width;
            param_dilate[tile_no].height = resize_height / R_DK2_TILE_NUM;

            /* Set whether to perform top or bottom edge border processing. */
            param_dilate[tile_no].top = (tile_no == TILE_0) ? 1 : 0;
            param_dilate[tile_no].bottom = (tile_no == TILE_5) ? 1 : 0;

            /* Initialize variables to be used in termination judgment of the DRP application */
            drp_lib_status[tile_no] = DRP_NOT_FINISH;

            /*********************/
            /* Start DRP Library */
            /*********************/
            ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *)&param_dilate[tile_no], sizeof(r_drp_dilate_t));
            DRP_DRV_ASSERT(ret_val);
        }
        /***************************************/
         /* Wait until DRP processing is finish */
         /***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_5; tile_no += 1)
        {
            /***************************************/
            /* Wait until DRP processing is finish */
            /***************************************/
            while (drp_lib_status[tile_no] == DRP_NOT_FINISH);
        }

         /* Set end time of DRP processing */
         PerformSetEndTime(2);

         /**********************/
         /* Unload DRP library */
         /**********************/
         ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] |drp_lib_id[TILE_2] |drp_lib_id[TILE_3] | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], &drp_lib_id[TILE_0]);
         DRP_DRV_ASSERT(ret_val);

#endif

         /* Clear Graphics buffer*/
        R_BCD_LcdClearGraphicsBuffer();

         /* Clear the cache to read the frame buffer in physical memory */
        R_CACHE_L1DataCleanInvalidLine((void *)&frame_RAM3[0], (uint32_t)(param_canny_hyst.width * param_canny_hyst.height));
#if(0)
       /* Write the canny's result into the graphics buffer */
        write_canny_data((uint8_t *)frame_RAM2, R_BCD_LcdGetOLVramAddress());
#endif


        /******************************/
        /* Load DRP Library           */
        /*        +-----------------+ */
        /* tile 0 |                 | */
        /*        +  FindContours   + */
        /* tile 1 |                 | */
        /*        +-----------------+ */
        /* tile 2 |                 | */
        /*        +-----------------+ */
        /* tile 3 |                 | */
        /*        +-----------------+ */
        /* tile 4 |                 | */
        /*        +-----------------+ */
        /* tile 5 |                 | */
        /*        +-----------------+ */
        /******************************/
        R_BCD_DrpDeleteContours();
        ret_val = R_DK2_Load(&g_drp_lib_find_contours[0],
                              R_DK2_TILE_0,
                              R_DK2_TILE_PATTERN_2_1_1_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /********************/
        /* Activate DRP Lib */
        /********************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(2);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        param_find_contours.src = (uint32_t)frame_RAM3;
        R_MMU_VAtoPA((uint32_t)param_find_contours.src, &(param_find_contours.src ));
        param_find_contours.dst_region = (uint32_t)contours_region_adr;
        R_MMU_VAtoPA((uint32_t)param_find_contours.dst_region, &(param_find_contours.dst_region ));
        /* Set Image size */
        param_find_contours.width = resize_width;
        param_find_contours.height = resize_height;

        /* Set whether to perform top or bottom edge border processing. */
        param_find_contours.work = (uint32_t)frame_RAM1;
        R_MMU_VAtoPA((uint32_t)param_find_contours.work, &(param_find_contours.work ));
        param_find_contours.dst_rect = (uint32_t)contours_rect_adr;
        R_MMU_VAtoPA((uint32_t)param_find_contours.dst_rect, &(param_find_contours.dst_rect ));

        param_find_contours.dst_region_size = 0;
        param_find_contours.dst_rect_size = FIND_CONTOURS_NUM;
        param_find_contours.threshold_width  = 20;
        param_find_contours.threshold_height = 20;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[0] = DRP_NOT_FINISH;
        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_find_contours, sizeof(r_drp_find_contours_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[0] == DRP_NOT_FINISH);

        /* Set end time of DRP processing */
        PerformSetEndTime(2);

        /**********************/
        /* Unload DRP library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /* Set end time of process*/
        PerformSetEndTime(0);


        R_BCD_DrpResizeContours();
        R_BCD_DrpCompressContours(R_BCD_PRESTIC_LENDGTH_MIN);
        contors_no = R_BCD_DrpSortContours();
        R_BCD_DrpSticCountours(contors_no);
        R_BCD_DrpCompressContours(R_BCD_DRP_CROP_LENGTH_MIN);
        R_BCD_DrpSortContours();



#ifdef R_BCD_BAYER_RGB
#if 0
		//R_BCD_DRP_do_Application(R_BCD_DRP_BAYER2RGB, 0, drp_work_frame_addr, drp_work_RGB, R_BCD_CAMERA_WIDTH, R_BCD_CAMERA_HEIGHT, 6);
		ret_val = R_DK2_Load(&g_drp_bayer2rgb[0],
				R_DK2_TILE_0 | R_DK2_TILE_2 | R_DK2_TILE_4,
				R_DK2_TILE_PATTERN_2_2_2, NULL, &cb_drp_finish, &drp_lib_id[0]);
		DRP_DRV_ASSERT(ret_val);

		ret_val = R_DK2_Activate(drp_lib_id[TILE_0] | drp_lib_id[TILE_2] | drp_lib_id[TILE_4], 0);
		DRP_DRV_ASSERT(ret_val);
		/***************************************/
		/* Set R_DK2_Start function parameters */
		/***************************************/
		for (tile_no = 0; tile_no <= TILE_4; tile_no+=2)
		{

			/* Set the address of buffer to be read/write by DRP */
			R_MMU_VAtoPA((uint32_t)input_bufadr + (R_BCD_CAMERA_WIDTH * (R_BCD_CAMERA_HEIGHT / 3)) * (tile_no/2), &(param_b2rgb[tile_no/2].src));
			R_MMU_VAtoPA((uint32_t)output_bufadr + (R_BCD_CAMERA_WIDTH * (R_BCD_CAMERA_HEIGHT / 3)) * (tile_no/2), &(param_b2rgb[tile_no/2].dst));

			/* Set Image size */
			param_b2rgb[tile_no/2].width = R_BCD_CAMERA_WIDTH;
			param_b2rgb[tile_no/2].height = R_BCD_CAMERA_HEIGHT / 3;

			/* Set whether to perform top or bottom edge border processing. */
			param_b2rgb[tile_no/2].top = (tile_no == TILE_0) ? 1 : 0;
			param_b2rgb[tile_no/2].bottom = (tile_no == TILE_4) ? 1 : 0;

			/* Initialize variables to be used in termination judgment of the DRP library */
			drp_lib_status[tile_no] = DRP_NOT_FINISH;

			/*********************/
			/* Start DRP Library */
			/*********************/
			ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *)&param_b2rgb[tile_no/2], sizeof(r_drp_bayer2rgb_t));
			DRP_DRV_ASSERT(ret_val);
		}

		/***************************************/
		/* Wait until DRP processing is finish */
		/***************************************/
		for (tile_no = TILE_0; tile_no <= TILE_4; tile_no+=2)
		{
			while (drp_lib_status[tile_no] == DRP_NOT_FINISH);
		}

		ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_2] | drp_lib_id[TILE_4], &drp_lib_id[TILE_0]);
		DRP_DRV_ASSERT(ret_val);
#else
		//R_BCD_DRP_do_Application(R_BCD_DRP_BAYER2RGB, 0, drp_work_frame_addr, drp_work_RGB, R_BCD_CAMERA_WIDTH, R_BCD_CAMERA_HEIGHT, 6);
		ret_val = R_DK2_Load(&g_drp_bayer2rgb[0],
				R_DK2_TILE_0 ,
				R_DK2_TILE_PATTERN_2_1_1_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
		DRP_DRV_ASSERT(ret_val);

		ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
		DRP_DRV_ASSERT(ret_val);
		/***************************************/
		/* Set R_DK2_Start function parameters */
		/***************************************/

		/* Set the address of buffer to be read/write by DRP */
		R_MMU_VAtoPA((uint32_t)input_bufadr, 	&(param_b2rgb[0].src));
		R_MMU_VAtoPA((uint32_t)eAI_InBuffer , 		&(param_b2rgb[0].dst));

		/* Set Image size */
		param_b2rgb[0].width = R_BCD_CAMERA_WIDTH;
		param_b2rgb[0].height = R_BCD_CAMERA_HEIGHT;

		/* Set whether to perform top or bottom edge border processing. */
		param_b2rgb[0].top = 1;
		param_b2rgb[0].bottom = 1;

		/* Initialize variables to be used in termination judgment of the DRP library */
		drp_lib_status[TILE_0] = DRP_NOT_FINISH;

		/*********************/
		/* Start DRP Library */
		/*********************/
		ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_b2rgb[0], sizeof(r_drp_bayer2rgb_t));
		DRP_DRV_ASSERT(ret_val);


		/***************************************/
		/* Wait until DRP processing is finish */
		/***************************************/
		while (drp_lib_status[TILE_0] == DRP_NOT_FINISH);

		ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
		DRP_DRV_ASSERT(ret_val);
#endif
		R_BCD_DRP_do_CroppResizeAI_RGB( (uint32_t)eAI_InBuffer);

#else
		R_BCD_DRP_do_CroppResizeAI( (uint32_t)output_bufadr, (uint32_t)frame_RAM2, (uint32_t)frame_RAM3);
#endif
		/* Write the data(buf) on the cache to physical memory */
		//        R_CACHE_L1DataCleanLine(R_BCD_LcdGetOLVramAddress(), ((R_BCD_LY2_WIDTH * R_BCD_LY2_HEIGHT) / 2));
		//R_CACHE_L1DataCleanLine(R_BCD_LcdGetOLVramAddress(), ((R_BCD_LY2_WIDTH * R_BCD_LY2_HEIGHT)));
		/* Write the data(buf) on the cache to physical memory */
		R_CACHE_L1DataCleanLine(R_BCD_LcdGetVramAddress(), ((R_BCD_LCD_WIDTH * R_BCD_LCD_HEIGHT) ));

		/* Display overlay buffer written processing time */
		//R_BCD_LcdSwapGraphicsBuffer();
		R_BCD_LcdSwapVideoBuffer();

    }

sample_finish:

    return;
}

/*******************************************************************************
* End of function sample_main
*******************************************************************************/

static int R_BCD_DRP_do_CroppResizeAI_RGB ( uint32_t input_address ) {
	int16_t ret_val = 0;
	uint32_t i = 0;
	volatile uint8_t checker;
	uint64_t mean = 0ul;
	uint64_t rstd = 0ul;

	if ( R_BCD_DrpCheckEmptyContours(i) ) {
		// Load DRP
		while ( (R_BCD_DrpSetCropParam(i, &parm_crop_rgb) != 0) & (i < R_BCD_DRP_CROP_NUM)) {
			if ( checker == (uint8_t)0 ) {

				/************************************************************************
				* 	RGB Cropping
				*************************************************************************/
				DRP_DRV_ASSERT(ret_val);
				ret_val = R_DK2_Load(g_drp_cropping_rgb,
						   R_DK2_TILE_0,
						   R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL, &cb_drp_finish, drp_lib_id);
				ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);

				R_MMU_VAtoPA((uint32_t)input_address, parm_crop_rgb.src);
				R_MMU_VAtoPA((uint32_t)eAI_Crop_rgb, parm_crop_rgb.dst);

				parm_crop_rgb.src = input_address;       /* Address of input image. */
				parm_crop_rgb.dst = eAI_Crop_rgb;    /* Address of output image. */
				parm_crop_rgb.src_width = R_BCD_CAMERA_WIDTH;   /* Input data width (Pixel) */
				parm_crop_rgb.src_height = R_BCD_CAMERA_HEIGHT; /* Input data height (Pixel) */

				/* Initialize variables to be used in termination judgment of the DRP library */
				drp_lib_status[TILE_0] = DRP_NOT_FINISH;

				ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&parm_crop_rgb, sizeof(r_drp_cropping_rgb_t));

				/***************************************/
				/* Wait until DRP processing is finish */
				/***************************************/
				while (drp_lib_status[TILE_0] == DRP_NOT_FINISH);
				R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);

			  /************************************************************************
			   * 	RGB Resize
			  *************************************************************************/
				ret_val = R_DK2_Load(g_drp_resize_bilinear_fixed_rgb,
						   R_DK2_TILE_0,
						   R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, drp_lib_id);
				DRP_DRV_ASSERT(ret_val);
				ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);

				R_MMU_VAtoPA((uint32_t)eAI_Crop_rgb, (uint32_t*)&parm_resize_rgb.src);
				R_MMU_VAtoPA((uint32_t)eAI_Resize_rgb, (uint32_t*)&parm_resize_rgb.dst);

				parm_resize_rgb.src_width = parm_crop_rgb.dst_width;   /* Input data width (Pixel) */
				parm_resize_rgb.src_height = parm_crop_rgb.dst_height; /* Input data height (Pixel) */
				parm_resize_rgb.fx = R_BCD_DrpGetResizeScale(app_data_cropping[2],R_BCD_RESIZE_LENGTH_X);;          /* The horizontal scale factor */
				parm_resize_rgb.fy = R_BCD_DrpGetResizeScale(app_data_cropping[3],R_BCD_RESIZE_LENGTH_Y);;          /* The vertical scale factor */

				/* Initialize variables to be used in termination judgment of the DRP library */
				drp_lib_status[TILE_0] = DRP_NOT_FINISH;
				R_DK2_Start(drp_lib_id[TILE_0], (void *)&parm_resize_rgb, sizeof(r_drp_resize_bilinear_fixed_rgb_t));
				/***************************************/
				/* Wait until DRP processing is finish */
				/***************************************/
				while (drp_lib_status[TILE_0] == DRP_NOT_FINISH);
				R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);


			} else {

				/************************************************************************
				* 	RGB Cropping only
				*************************************************************************/
				ret_val = R_DK2_Load(g_drp_cropping_rgb,
						   R_DK2_TILE_0,
						   R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL, &cb_drp_finish, drp_lib_id);
				DRP_DRV_ASSERT(ret_val);
				ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);

				R_MMU_VAtoPA((uint32_t)input_address, parm_crop_rgb.src);
				R_MMU_VAtoPA((uint32_t)eAI_Crop_rgb, parm_crop_rgb.dst);

				parm_crop_rgb.src_width = R_BCD_CAMERA_WIDTH;   /* Input data width (Pixel) */
				parm_crop_rgb.src_height = R_BCD_CAMERA_HEIGHT; /* Input data height (Pixel) */

				/* Initialize variables to be used in termination judgment of the DRP library */
				drp_lib_status[TILE_0] = DRP_NOT_FINISH;

				ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&parm_crop_rgb, sizeof(r_drp_cropping_rgb_t));
				//DRP_DRV_ASSERT(ret_val);
				/***************************************/
				/* Wait until DRP processing is finish */
				/***************************************/
				while (drp_lib_status[TILE_0] == DRP_NOT_FINISH);
				R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
			}

			ret_val = R_DK2_Load(g_drp_histogram_normalization_rgb,
					   R_DK2_TILE_0,
					   R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL, &cb_drp_finish, drp_lib_id);
			DRP_DRV_ASSERT(ret_val);
			ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);

			// Histogram Normalization on RGB 1 TIle
			/**************************************************************************
					Histogram Mode 1
			***************************************************************************/
			R_MMU_VAtoPA((uint32_t)eAI_Resize_rgb, parm_histogram_rgb[0].src);
			R_MMU_VAtoPA((uint32_t)&histRGB_output_data[0], parm_histogram_rgb[0].dst);

			parm_histogram_rgb[0].width = R_BCD_AI_INPUT_X;                        /* The horizontal size (pixels) of image */
			parm_histogram_rgb[0].height = R_BCD_AI_INPUT_Y;                             /* The vertical size (pixels) of image */
			parm_histogram_rgb[0].mode = 1;            /* MODE1 executed */
			// Ignore following to Mode 1. Set to zero.
			parm_histogram_rgb[0].src_pixel_red_mean = 0;                                      /* Mean of pixel values in input image */
			parm_histogram_rgb[0].src_pixel_red_rstd = 0;                                      /* Reciprocal standard deviation of pixel values in input image */
			parm_histogram_rgb[0].src_pixel_green_mean = 0;                                 /* Mean of pixel values in input image */
			parm_histogram_rgb[0].src_pixel_green_rstd = 0;                                 /* Reciprocal standard deviation of pixel values in input image */
			parm_histogram_rgb[0].src_pixel_blue_mean = 0;                                 /* Mean of pixel values in input image */
			parm_histogram_rgb[0].src_pixel_blue_rstd = 0;                                 /* Reciprocal standard deviation of pixel values in input image */
			parm_histogram_rgb[0].dst_pixel_mean = 0;                                 /* Mean of pixel values in output image */
			parm_histogram_rgb[0].dst_pixel_std = 0;                                  /* Standard deviation of pixel values in output image */

			/* Initialize variables to be used in termination judgment of the DRP library */
			drp_lib_status[TILE_0] = DRP_NOT_FINISH;
			R_DK2_Start(drp_lib_id[TILE_0], (void *)&parm_histogram_rgb[0], sizeof(r_drp_histogram_normalization_rgb_t));

			/***************************************/
			/* Wait until DRP processing is finish */
			/***************************************/
			while (drp_lib_status[TILE_5] == DRP_NOT_FINISH);

			/**************************************************************************
					Histogram Mode 2
			***************************************************************************/

			// Run Histogram Mode 2
			R_MMU_VAtoPA((uint32_t)eAI_Resize_rgb, parm_histogram_rgb[0].src);
			R_MMU_VAtoPA((uint32_t)eAI_Resize_rgb, parm_histogram_rgb[0].dst);

			parm_histogram_rgb[0].width = R_BCD_AI_INPUT_X;                                /* The horizontal size (pixels) of image */
			parm_histogram_rgb[0].height = R_BCD_AI_INPUT_Y;                                     /* The vertical size (pixels) of image */
			parm_histogram_rgb[0].mode = 2;            /* MODE2 executed */

			mean = (histRGB_output_data[0].sum_red/(R_BCD_AI_INPUT_Y*R_BCD_AI_INPUT_X));
			rstd = 4096 / sqrt((histRGB_output_data[0].square_sum_red/(R_BCD_AI_INPUT_Y*R_BCD_AI_INPUT_X))-(mean*mean));
			parm_histogram_rgb[0].src_pixel_red_mean = (uint32_t)(mean*4096);                                      /* Mean of pixel values in input image */
			parm_histogram_rgb[0].src_pixel_red_rstd = (uint32_t)rstd;                                      /* Reciprocal standard deviation of pixel values in input image */

			mean = (histRGB_output_data[0].sum_green/(R_BCD_AI_INPUT_Y*R_BCD_AI_INPUT_X));
			rstd = 4096 / sqrt((histRGB_output_data[0].square_sum_green/(R_BCD_AI_INPUT_Y*R_BCD_AI_INPUT_X))-(mean*mean));
			parm_histogram_rgb[0].src_pixel_green_mean = (uint32_t)(mean*4096);                                 /* Mean of pixel values in input image */
			parm_histogram_rgb[0].src_pixel_green_rstd = (uint32_t)rstd;                                 /* Reciprocal standard deviation of pixel values in input image */

			mean = (histRGB_output_data[0].sum_blue/(R_BCD_AI_INPUT_Y*R_BCD_AI_INPUT_X));
			rstd = 4096 / sqrt((histRGB_output_data[0].square_sum_blue/(R_BCD_AI_INPUT_Y*R_BCD_AI_INPUT_X))-(mean*mean));
			parm_histogram_rgb[0].src_pixel_blue_mean = (uint32_t)(mean*4096);                                 /* Mean of pixel values in input image */
			parm_histogram_rgb[0].src_pixel_blue_rstd = (uint32_t)rstd;                                 /* Reciprocal standard deviation of pixel values in input image */

			parm_histogram_rgb[0].dst_pixel_mean = 112;  /* Mean of pixel values in output image */
			parm_histogram_rgb[0].dst_pixel_std  = 48;   /* Standard deviation of pixel values in output image */

			/* Initialize variables to be used in termination judgment of the DRP library */
			drp_lib_status[TILE_0] = DRP_NOT_FINISH;
			R_DK2_Start(drp_lib_id[TILE_0], (void *)&parm_histogram_rgb[0], sizeof(r_drp_histogram_normalization_rgb_t));

			/***************************************/
			/* Wait until DRP processing is finish */
			/***************************************/
			while (drp_lib_status[TILE_0] == DRP_NOT_FINISH);
			R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);

			/***************************************/
			/* Preform AI */
			/***************************************/
			R_BCD_DecomposeRGB(eAI_Resize_rgb, eAI_Crop_rgb);
			ai_result result = R_BCD_AI((uint8_t*)eAI_Crop_rgb);

			i++;
		}
	}
	return ret_val;
}
int R_BCD_DRP_do_CroppResizeAI ( uint32_t input_address, uint32_t crop_output_address , uint32_t resize_output_address ) {

	int16_t ret_val = 0;
	volatile float ai_result = 0.0;
	uint32_t i;
		volatile uint64_t mean = 0ul;
		volatile uint64_t rstd = 0ul;

	if ( R_BCD_DrpCheckEmptyContours(i) ) {

		ret_val = R_DK2_Load(g_drp_lib_resize_bilinear_fixed,
				   R_DK2_TILE_0,
				   R_DK2_TILE_PATTERN_4_1_1, NULL, &cb_drp_finish, drp_lib_id);
		DRP_DRV_ASSERT(ret_val);
		ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
		DRP_DRV_ASSERT(ret_val);
		ret_val = R_DK2_Load(g_drp_lib_cropping,
				   R_DK2_TILE_4,
				   R_DK2_TILE_PATTERN_4_1_1, NULL, &cb_drp_finish, drp_lib_id);
		ret_val = R_DK2_Activate(drp_lib_id[TILE_4], 0);
		DRP_DRV_ASSERT(ret_val);
		ret_val = R_DK2_Load(g_drp_lib_histogram_normalization,
				   R_DK2_TILE_5,
				   R_DK2_TILE_PATTERN_4_1_1, NULL, &cb_drp_finish, drp_lib_id);
		DRP_DRV_ASSERT(ret_val);
		ret_val = R_DK2_Activate(drp_lib_id[TILE_5], 0);
		DRP_DRV_ASSERT(ret_val);

		while ( (R_BCD_DrpSetCropParam(i, &parm_crop_gry) != 0) & (i < R_BCD_DRP_CROP_NUM)) {

			volatile uint8_t checker;// = R_BCD_DrpCheckCropParam_AIsize();
			checker = R_BCD_DrpCheckCropParam_AIsize();

			if ( checker == (uint8_t)0 ) {
				/************************************************************************
				* 	Gray Cropping
				*************************************************************************/
				parm_crop_gry.src = input_address;       /* Address of input image. */
				parm_crop_gry.dst = crop_output_address;    /* Address of output image. */
				parm_crop_gry.src_width = R_BCD_CAMERA_WIDTH;   /* Input data width (Pixel) */
				parm_crop_gry.src_height = R_BCD_CAMERA_HEIGHT; /* Input data height (Pixel) */

				/* Initialize variables to be used in termination judgment of the DRP library */
				drp_lib_status[TILE_4] = DRP_NOT_FINISH;

				ret_val = R_DK2_Start(drp_lib_id[TILE_4], (void *)&parm_crop_gry, sizeof(r_drp_cropping_t));
				//DRP_DRV_ASSERT(ret_val);
				/***************************************/
				/* Wait until DRP processing is finish */
				/***************************************/
				while (drp_lib_status[TILE_4] == DRP_NOT_FINISH);
#if 1
			  /************************************************************************
			   * 	Gray Resize
			  *************************************************************************/
				param_resize.src = crop_output_address;       /* Address of input image. */
				param_resize.dst = resize_output_address;    /* Address of output image. */
				param_resize.src_width = parm_crop_gry.dst_width;   /* Input data width (Pixel) */
				param_resize.src_height = parm_crop_gry.dst_height; /* Input data height (Pixel) */
				param_resize.fx = R_BCD_DrpGetResizeScale(app_data_cropping[2],R_BCD_RESIZE_LENGTH_X);;          /* The horizontal scale factor */
				param_resize.fy = R_BCD_DrpGetResizeScale(app_data_cropping[3],R_BCD_RESIZE_LENGTH_Y);;          /* The vertical scale factor */

				/* Initialize variables to be used in termination judgment of the DRP library */
				drp_lib_status[TILE_0] = DRP_NOT_FINISH;
				R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_resize, sizeof(r_drp_resize_bilinear_fixed_t));
				/***************************************/
				/* Wait until DRP processing is finish */
				/***************************************/
				while (drp_lib_status[TILE_0] == DRP_NOT_FINISH);
#endif
			} else {
				/************************************************************************
				* 	Gray Cropping
				*************************************************************************/
				parm_crop_gry.src = input_address;       /* Address of input image. */
				parm_crop_gry.dst = resize_output_address;    /* Address of output image. */
				parm_crop_gry.src_width = R_BCD_CAMERA_WIDTH;   /* Input data width (Pixel) */
				parm_crop_gry.src_height = R_BCD_CAMERA_HEIGHT; /* Input data height (Pixel) */

				/* Initialize variables to be used in termination judgment of the DRP library */
				drp_lib_status[TILE_4] = DRP_NOT_FINISH;
				R_DK2_Start(drp_lib_id[TILE_4], (void *)&parm_crop_gry, sizeof(r_drp_cropping_t));
				/***************************************/
				/* Wait until DRP processing is finish */
				/***************************************/
				while (drp_lib_status[TILE_4] == DRP_NOT_FINISH);
			}

			/**************************************************************************
					Histogram Mode 1
			***************************************************************************/
#if 1
			parm_hist[0].src = resize_output_address;     /* Address of input image */
			parm_hist[0].dst = (uint32_t)&histGRY_output_data[0];     /* Address of output image */
			parm_hist[0].width = R_BCD_AI_INPUT_X;                        /* The horizontal size (pixels) of image */
			parm_hist[0].height = R_BCD_AI_INPUT_Y;                             /* The vertical size (pixels) of image */
			parm_hist[0].mode = 1;            /* MODE1 executed */
			// Ignore following to Mode 1. Set to zero.
			parm_hist[0].src_pixel_mean = 0;                                 /* Mean of pixel values in input image */
			parm_hist[0].src_pixel_rstd = 0;                                 /* Reciprocal standard deviation of pixel values in input image */
			parm_hist[0].dst_pixel_mean = 0;                                 /* Mean of pixel values in output image */
			parm_hist[0].dst_pixel_std = 0;                                  /* Standard deviation of pixel values in output image */

			/* Initialize variables to be used in termination judgment of the DRP library */
			drp_lib_status[TILE_5] = DRP_NOT_FINISH;
			R_DK2_Start(drp_lib_id[TILE_5], (void *)&parm_hist[0], sizeof(r_drp_histogram_normalization_t));

			/***************************************/
			/* Wait until DRP processing is finish */
			/***************************************/
			while (drp_lib_status[TILE_5] == DRP_NOT_FINISH);

			/**************************************************************************
					Histogram Mode 2
			***************************************************************************/

			// Calculate Gray Histogram Mode 2 Params
			mean = (histGRY_output_data[0].sum/(R_BCD_AI_INPUT_Y*R_BCD_AI_INPUT_X));
			rstd = 4096 / sqrt((histGRY_output_data[0].square_sum/(R_BCD_AI_INPUT_Y*R_BCD_AI_INPUT_X))-(mean*mean));

			// Run Histogram Mode 2
			parm_hist[0].src = resize_output_address;            /* Address of input image */
			parm_hist[0].dst = resize_output_address;            /* Address of output image */
			parm_hist[0].width = R_BCD_AI_INPUT_X;                                /* The horizontal size (pixels) of image */
			parm_hist[0].height = R_BCD_AI_INPUT_Y;                                     /* The vertical size (pixels) of image */
			parm_hist[0].mode = 2;            /* MODE2 executed */
			parm_hist[0].src_pixel_mean = mean*4096;                                      /* Mean of pixel values in input image */
			parm_hist[0].src_pixel_rstd = rstd;                                      /* Reciprocal standard deviation of pixel values in input image */
			parm_hist[0].dst_pixel_mean = 112;  /* Mean of pixel values in output image */
			parm_hist[0].dst_pixel_std  = 48;   /* Standard deviation of pixel values in output image */

			/* Initialize variables to be used in termination judgment of the DRP library */
			drp_lib_status[TILE_5] = DRP_NOT_FINISH;
			R_DK2_Start(drp_lib_id[TILE_5], (void *)&parm_hist[0], sizeof(r_drp_histogram_normalization_t));

			/***************************************/
			/* Wait until DRP processing is finish */
			/***************************************/
			while (drp_lib_status[TILE_5] == DRP_NOT_FINISH);
#endif
			float result = R_BCD_AI((uint8_t*)resize_output_address);
			if ( ai_result < result) {
				ai_result = result;
			}
			i++;
		}

		R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], &drp_lib_id[TILE_0]);
	}
	return(ret_val);
}

/******************************************************************************
* Function Name: R_BCD_DecomposeRGB
* Description  : Decompose RGB to R and G and B
* Arguments    : input_address
*              :   Address of input image(AI input size).
*              : output_address
*              :   Address of Output buffer(AI input size).
* Return Value : -
******************************************************************************/
static void R_BCD_DecomposeRGB(uint32_t input_address, uint32_t output_address)
{
//	volatile int i;
	uint8_t *out = (uint8_t *)output_address;
	uint8_t *in = (uint8_t *)input_address;
	uint32_t data_size =  R_BCD_AI_INPUT_X * R_BCD_AI_INPUT_Y;
#if 0
	for(i = 0; i < data_size; i++)
	{
		out[(data_size * 0) + i] = in[0 + i * 3];
		out[(data_size * 1) + i] = in[1 + i * 3];
		out[(data_size * 2) + i] = in[2 + i * 3];
	}
#else
	for(uint32_t j = 0; j < 3; j++)
	{
		for(uint32_t i = 0; i < data_size; i++)
		{
			out[(data_size * j) + i] = in[j + i * 3];
		}
	}

#endif
}
/******************************************************************************
* Function Name: R_BCD_DrpDeleteContours
* Description  : Clear output contours buffer
* Arguments    : -
* Return Value : -
******************************************************************************/
static void R_BCD_DrpDeleteContours(void)
{
	for(int i = 0; (i < FIND_CONTOURS_NUM); i++)
	{
		memset(&contours_rect_adr[i], 0, sizeof(contours_rect_t));
	}
}

/******************************************************************************
* Function Name: R_BCD_DrpPointhiger
* Description  : return grater number x or y
* Arguments    : x
* 			   :  number
* 			   : y
* 			   :  number
* Return Value : grater number
******************************************************************************/
uint16_t R_BCD_DrpPointhiger(uint16_t x, uint16_t y)
{
	if(x > y)
	{
		return x;
	}
	else
	{
		return y;
	}

}

/******************************************************************************
* Function Name: R_BCD_DrpPointlower
* Description  : return lower number x or y
* Arguments    : x
* 			   :  number
* 			   : y
* 			   :  number
* Return Value : lower number
******************************************************************************/
uint16_t R_BCD_DrpPointlower(uint16_t x, uint16_t y)
{
	if(x < y)
	{
		return x;
	}
	else
	{
		return y;
	}
}

/******************************************************************************
* Function Name: R_BCD_DrpCheckOccupiedAreaContours
* Description  : Check the priority of contour
* Arguments    : i
* 			   :  number of output contour
* Return Value : the point of priority
******************************************************************************/
uint16_t R_BCD_DrpCheckOccupiedAreaContours(uint32_t i)
{
	uint16_t occupied_area;
	occupied_area = contours_rect_adr[i].width * contours_rect_adr[i].height / contours_rect_adr[i].num;

	return occupied_area;

}

/******************************************************************************
* Function Name: R_BCD_DrpGetResizeScale
* Description  : Return parameter Resize scale
* Arguments    : input_length
*              :  input data length
* 			   : output_length
*              :  output data length
* Return Value : Resize scale
******************************************************************************/
static int16_t R_BCD_DrpGetResizeScale(uint16_t input_length, uint16_t output_length)
{
	if(input_length > output_length)
	{
		switch(input_length / output_length)
		{
		case 2:
			return 0x20;
			break;
		case 4:
			return 0x40;
			break;
		case 8:
			return 0x80;
			break;
		default:
			return 0;
			break;
		}
	}
	else
	{
		switch(output_length / input_length)
		{
		case 1:
			return 0x10;
			break;
		case 2:
			return 0x08;
			break;
		case 4:
			return 0x04;
			break;
		case 8:
			return 0x02;
			break;
		case 16:
			return 0x01;
			break;
		default:
			return 0;
			break;
		}
	}
	return 0;
}
/******************************************************************************
* Function Name: R_BCD_DrpCheckCropParam_AIsize
* Description  : Check if Resize Application is necessary
* Arguments    : -
* Return Value : 1 : Skip Resize
*              : 0 : Need Resize
******************************************************************************/
static uint8_t R_BCD_DrpCheckCropParam_AIsize(void)
{
	if((app_data_cropping[2] == R_BCD_AI_INPUT_X) && (app_data_cropping[3] == R_BCD_AI_INPUT_Y))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	return 2;

}

/******************************************************************************
* Function Name: R_BCD_DrpGetCropLength
* Description  : Return crop length to fit AI model
* Arguments    : length
* 			   :  contour length
* Return Value : length fitting AI model
******************************************************************************/
static uint16_t R_BCD_DrpGetCropLength(uint16_t length)
{
#if defined(R_BCD_DRP_CROP_OVER_128)
	if(length <= 128)
	{
		return 128;
	}
	else if(length <= 256)
	{
		return 256;
	}
	else{
		return 0;
	}
#else
	if(length <= 32)
	{
		return 32;
	}
	else if(length <= 64)
	{
		return 64;
	}
	else if(length <= 128)
	{
		return 128;
	}
	else if(length <= 256)
	{
		return 256;
	}
	else{
		return 0;
	}
#endif
}

/******************************************************************************
* Function Name: R_BCD_DrpGetCropLength_longer
* Description  : Return longer crop length between x and y
* Arguments    : x
* 			   :  contour width
* 		       : y
* 			   :  contour height
* Return Value : -
******************************************************************************/
static uint16_t R_BCD_DrpGetCropLength_longer(uint16_t x, uint16_t y)
{

	if(x > y)
	{
		return R_BCD_DrpGetCropLength(x);
	}else
	{
		return R_BCD_DrpGetCropLength(y);
	}

}

/******************************************************************************
* Function Name: R_BCD_DrpSetCropParam
* Description  : Set parameter for Crop process
* Arguments    : i
* 			   :  number of output contour
* Return Value : -
******************************************************************************/
static uint32_t R_BCD_DrpSetCropParam(uint32_t i, r_drp_cropping_t *crop)
{
	if((contours_rect_adr[i].width == 0) || (contours_rect_adr[i].height == 0))
	{
		return 0;
	}

#if defined(R_BCD_DRP_CROP_SQUARE)
	app_data_cropping[2] = R_BCD_DrpGetCropLength_longer(contours_rect_adr[i].width, contours_rect_adr[i].height);
	app_data_cropping[3] = R_BCD_DrpGetCropLength_longer(contours_rect_adr[i].width, contours_rect_adr[i].height);
	crop->dst_width = app_data_cropping[2];
	crop->dst_height = app_data_cropping[3];
#else
	crop->dst_width = R_BCD_DrpGetCropLength(contours_rect_adr[i].width);
	crop->dst_height = R_BCD_DrpGetCropLength(contours_rect_adr[i].height);
#endif
	int32_t x = contours_rect_adr[i].x - (app_data_cropping[2] - contours_rect_adr[i].width) / 2;
	int32_t y = contours_rect_adr[i].y - (app_data_cropping[3] - contours_rect_adr[i].height) / 2;

    if(x <= 0)
    {
    	crop->offset_x = 0;
    }
    else if(x + app_data_cropping[2] >= R_BCD_CAMERA_WIDTH)
    {
    	crop->offset_x = (R_BCD_CAMERA_WIDTH - 1) - app_data_cropping[2];
    }
    else
    {
    	crop->offset_x = x;
    }

    if((y <= 0))
    {
    	crop->offset_y = 0;
    }
    else if((y + app_data_cropping[3] >= R_BCD_CAMERA_HEIGHT))
    {
    	crop->offset_y = (R_BCD_CAMERA_HEIGHT - 1) - app_data_cropping[3];
    }
    else
    {
    	crop->offset_y = y;
    }

#if defined(R_BCD_DRP_COUNTOURS_TO_LCD)
    contours_rect_adr[i].x  = app_data_cropping[0];
    contours_rect_adr[i].y  = app_data_cropping[1];
    contours_rect_adr[i].width = app_data_cropping[2];
    contours_rect_adr[i].height = app_data_cropping[3];
#endif

    return 1;
}
/******************************************************************************
* Function Name: R_BCD_DrpCheckEmptyContours
* Description  : Check Find Contours stack buffer is empty
* Arguments    : -
* Return Value : -
******************************************************************************/
static uint32_t R_BCD_DrpCheckEmptyContours(uint32_t i)
{
	if(contours_rect_adr[i].x)
	{
		return 1;
	}
	if(contours_rect_adr[i].width)
	{
		return 1;
	}
	if(contours_rect_adr[i].y)
	{
		return 1;
	}
	if(contours_rect_adr[i].height)
	{
		return 1;
	}
	if(contours_rect_adr[i].num)
	{
		return 1;
	}
	if(contours_rect_adr[i].addr)
	{
		return 1;
	}
	return 0;

}

/******************************************************************************
* Function Name: R_BCD_DrpResizeContours
* Description  : Resize output contours fit Camera size
* Arguments    : -
* Return Value : -
******************************************************************************/
void R_BCD_DrpResizeContours(void)
{
	uint32_t i;

	 i = 0;

	/* compare size */
	while(R_BCD_DrpCheckEmptyContours(i))
	{
		contours_rect_adr[i].width = (contours_rect_adr[i].width * R_BCD_DRP_IMAGE_RESIZE) + (R_BCD_DRP_IMAGE_RESIZE - 1);
		contours_rect_adr[i].x  = (contours_rect_adr[i].x  * R_BCD_DRP_IMAGE_RESIZE);
		contours_rect_adr[i].height = (contours_rect_adr[i].height * R_BCD_DRP_IMAGE_RESIZE) + (R_BCD_DRP_IMAGE_RESIZE - 1);
		contours_rect_adr[i].y  = (contours_rect_adr[i].y  * R_BCD_DRP_IMAGE_RESIZE);
		contours_rect_adr[i].num = (contours_rect_adr[i].num * R_BCD_DRP_IMAGE_RESIZE * R_BCD_DRP_IMAGE_RESIZE);
		i++;
	}
}

/******************************************************************************
* Function Name: R_BCD_DrpCheckLengthContours
* Description  : Check if the contour is necessary for AI model
* Arguments    : width
*              :  contour width
* 			   : height
*              :  contour height
* 			   : length_min
*              :  the minimum length of contour
* Return Value : 0 : Not need
*              : 1 : Need
******************************************************************************/
int8_t R_BCD_DrpCheckLengthContours(uint16_t width, uint16_t height, uint32_t length_min)
{
	if((width > R_BCD_DRP_CROP_LENGTH_MAX) || (height > R_BCD_DRP_CROP_LENGTH_MAX))
	{
		return 0;
	}


#if !defined(R_BCD_DRP_CROP_ONE_SIDE_OVER)
	if((width < length_min) || (height < length_min))
	{
		return 0;
	}
#else
	if((width < length_min) && (height < length_min))
	{
		return 0;
	}
#endif

#if defined(R_BCD_DRP_CROP_SIZE_RAITO)
	if((width >= length_min) && (height >= length_min))
	{
		return 1;
	}

	uint16_t checker;
	if(width > height)
	{
		checker = width / height;
	}
	else
	{
		checker = height / width;
	}

	if(checker > R_BCD_DRP_CROP_SIZE_RAITO)
	{
		return 0;
	}
#endif
	return 1;

}

/******************************************************************************
* Function Name: R_BCD_DrpCompressContours
* Description  : Compress output contours for AI model
* Arguments    : length_min
*              :   the minimum size for AI model
* Return Value : -
******************************************************************************/
void R_BCD_DrpCompressContours(uint32_t length_min)
{

	uint32_t i;
	uint32_t num;

	/* Check length */
	{
		 i = 0;
		 num = 0;

		while(1)
		{

			if(R_BCD_DrpCheckLengthContours(contours_rect_adr[i].width, contours_rect_adr[i].height, length_min))
			{
				if((num != i) && (contours_rect_adr[i].x != 0))
				{
					memcpy(&contours_rect_adr[num] ,&contours_rect_adr[i] ,sizeof(contours_rect_t));
					memset(&contours_rect_adr[i] ,0 ,sizeof(contours_rect_t));
				}
				num++;
			}
			else if(R_BCD_DrpCheckEmptyContours(i))
			{
				memset(&contours_rect_adr[i] ,0 ,sizeof(contours_rect_t));
			}
			else
			{
				memset(&contours_rect_adr[i] ,0 ,sizeof(contours_rect_t));
				break;
			}
			i++;
		}
	}

	uint32_t write_point;
	uint32_t j;

	/* check dual */
	{
		write_point = 1;
		j = 0;
		i = 1;

		while(i < num)
		{
			while(i > j)
			{
				int32_t x1_diff = contours_rect_adr[i].x - contours_rect_adr[j].x;
				int32_t y1_diff = contours_rect_adr[i].y - contours_rect_adr[j].y;
				int32_t x2_diff = (contours_rect_adr[i].x + contours_rect_adr[i].width) - (contours_rect_adr[j].x +  contours_rect_adr[j].width);
				int32_t y2_diff = (contours_rect_adr[i].y + contours_rect_adr[i].height) - (contours_rect_adr[j].y +  contours_rect_adr[j].height);
				int32_t max_diff = R_BCD_DRP_CROP_LENGTH_DIFF;

				if(((- max_diff < x1_diff) && (x1_diff < max_diff )) && ((- max_diff < y1_diff) && (y1_diff < max_diff )) &&
						((- max_diff < x2_diff) && (x2_diff < max_diff )) && ((- max_diff < y2_diff) && (y2_diff < max_diff )))
				{
					break;
				}
				else
				{
					j++;
					continue;
				}

			}
			if(i == j)
			{
				if(write_point != i)
				{
					memcpy(&contours_rect_adr[write_point], &contours_rect_adr[i], sizeof(contours_rect_t));
				}
				write_point++;
			}
			i++;
			j = 0;
		}

		while(write_point < num)
		{
			memset(&contours_rect_adr[write_point], 0, sizeof(contours_rect_t));
			write_point++;
		}
	}

	return;
}

/******************************************************************************
* Function Name: R_BCD_DrpSortContours
* Description  : sort output contours for AI model
* Arguments    : -
* Return Value : Number of contours
******************************************************************************/
static  uint32_t R_BCD_DrpSortContours (void) {

	uint32_t i;
#if 0
	uint32_t j;
	contours_rect_t buff;

	/* data sort */

	i = 0;

	while(R_BCD_DrpCheckEmptyContours(i))
	{
		j = i + 1;
		while(R_BCD_DrpCheckEmptyContours(j))
		{
			if(i == j)
			{
				j++;
				continue;
			}
#if defined(R_BCD_DRP_CONTOUR_COMPARE_OCCUPIED_AREA)
			if(R_BCD_DrpCheckOccupiedAreaContours(i) < R_BCD_DrpCheckOccupiedAreaContours(j))
#elif defined(R_BCD_DRP_CONTOUR_COMPARE_AREA_SIZE)
			if(R_BCD_DrpCheckAreaSizeContours(i) < R_BCD_DrpCheckAreaSizeContours(j))
#else
				break;
#endif
			{
				buff = contours_rect_adr[i];
				contours_rect_adr[i] = contours_rect_adr[j];
				contours_rect_adr[j] = buff;
			}
			j++;
		}
		i++;
	}
#endif
	return i;
}

/******************************************************************************
* Function Name: R_BCD_DrpSticCountours
* Description  : Union output contours for AI model
* Arguments    : contours_num
* 			   :  number of contours in output contours buffer
* Return Value : -
******************************************************************************/
void R_BCD_DrpSticCountours(uint32_t contours_num)
{

	uint32_t i;
	uint32_t j;
	uint32_t flag = 1;
	contours_rect_t buff;

	/* data sort */

	i = 0;

	while(flag)
	{
		flag = 0;
		while(R_BCD_DrpCheckEmptyContours(i))
		{
			j = i + 1;
			while(R_BCD_DrpCheckEmptyContours(j))
			{
				if(contours_num > 0x400)
				{
					return;
				}
				if(j <= i)
				{
					j = i + 1;
					continue;
				}

				buff.x = R_BCD_DrpPointlower(contours_rect_adr[i].x, contours_rect_adr[j].x);
				buff.y = R_BCD_DrpPointlower(contours_rect_adr[i].y, contours_rect_adr[j].y);
				buff.width = R_BCD_DrpPointhiger
						(contours_rect_adr[i].x + contours_rect_adr[i].width,
							contours_rect_adr[j].x + contours_rect_adr[j].width)
								- buff.x;
				buff.height = R_BCD_DrpPointhiger
						(contours_rect_adr[i].y + contours_rect_adr[i].height,
							contours_rect_adr[j].y + contours_rect_adr[j].height)
								- buff.y;
				buff.num = contours_rect_adr[i].num + contours_rect_adr[j].num;

				if(R_BCD_DrpCheckLengthContours(buff.width,buff.height, R_BCD_DRP_CROP_LENGTH_MIN))
				{
					contours_rect_adr[i].width = buff.width;
					contours_rect_adr[i].x  = buff.x;
					contours_rect_adr[i].height = buff.height;
					contours_rect_adr[i].y  = buff.y;
					contours_rect_adr[i].num = buff.num;
					contours_rect_adr[i].addr = 0xdeadbeaf;

					contours_num--;
					contours_rect_adr[j].width = contours_rect_adr[contours_num].width;
					contours_rect_adr[j].x  = contours_rect_adr[contours_num].x;
					contours_rect_adr[j].height = contours_rect_adr[contours_num].height;
					contours_rect_adr[j].y  = contours_rect_adr[contours_num].y;
					contours_rect_adr[j].num = contours_rect_adr[contours_num].num;
					contours_rect_adr[j].addr = contours_rect_adr[contours_num].addr;

					contours_rect_adr[contours_num].width = 0;
					contours_rect_adr[contours_num].x  = 0;
					contours_rect_adr[contours_num].height = 0;
					contours_rect_adr[contours_num].y  = 0;
					contours_rect_adr[contours_num].num = 0;
					contours_rect_adr[contours_num].addr = 0;
					flag = 1;
					continue;
				}

				j++;
			}
			i++;
		}
	}
	return;
}
/* End of File */
