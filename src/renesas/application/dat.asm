@/*******************************************************************************
@* DISCLAIMER
@* This software is supplied by Renesas Electronics Corporation and is only
@* intended for use with Renesas products. No other uses are authorized. This
@* software is owned by Renesas Electronics Corporation and is protected under
@* all applicable laws, including copyright laws.
@* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
@* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
@* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
@* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
@* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
@* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
@* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
@* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
@* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
@* Renesas reserves the right, without notice, to make changes to this software
@* and to discontinue the availability of this software. By using this software,
@* you agree to the additional terms and conditions found by accessing the
@* following link:
@* http://www.renesas.com/disclaimer
@* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
@*******************************************************************************/
.section ".rodata"

.balign 32
.global g_drp_lib_isp_bayer2grayscale_6
g_drp_lib_isp_bayer2grayscale_6:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_simple_isp/r_drp_simple_isp_bayer2grayscale_6.dat"

.balign 32
.global g_drp_lib_resize_bilinear_fixed
g_drp_lib_resize_bilinear_fixed:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_resize_bilinear_fixed/r_drp_resize_bilinear_fixed.dat"

.balign 32
.global g_drp_lib_gaussian_blur
g_drp_lib_gaussian_blur:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_gaussian_blur/r_drp_gaussian_blur.dat"

.balign 32
.global g_drp_lib_canny_calculate
g_drp_lib_canny_calculate:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_canny_calculate/r_drp_canny_calculate.dat"

.balign 32
.global g_drp_lib_canny_hysterisis
g_drp_lib_canny_hysterisis:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_canny_hysterisis/r_drp_canny_hysterisis.dat"

.balign 32
.global g_drp_lib_dilate
g_drp_lib_dilate:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_dilate/r_drp_dilate.dat"

.balign 32
.global g_drp_lib_find_contours
g_drp_lib_find_contours:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_find_contours/r_drp_find_contours.dat"

.balign 32
.global g_drp_lib_bayer2rgb_color_correction
g_drp_lib_bayer2rgb_color_correction:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_bayer2rgb_color_correction/r_drp_bayer2rgb_color_correction.dat"

.balign 32
.global g_drp_lib_cropping
g_drp_lib_cropping:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_cropping/r_drp_cropping.dat"

.balign 32
.global g_drp_lib_histogram_normalization
g_drp_lib_histogram_normalization:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_histogram_normalization/r_drp_histogram_normalization.dat"

.balign 32
.global g_drp_histogram_normalization_rgb
g_drp_histogram_normalization_rgb:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_histogram_normalization_rgb/r_drp_histogram_normalization_rgb.dat"

.balign 32
.global g_drp_cropping_rgb
g_drp_cropping_rgb:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_cropping_rgb/r_drp_cropping_rgb.dat"

.balign 32
.global g_drp_resize_bilinear_fixed_rgb
g_drp_resize_bilinear_fixed_rgb:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_resize_bilinear_fixed_rgb/r_drp_resize_bilinear_fixed_rgb.dat"

.balign 32
.global g_drp_bayer2rgb
g_drp_bayer2rgb:
.incbin "../../../generate/sc_drivers/r_drp/drp_lib/r_drp_bayer2rgb/r_drp_bayer2rgb.dat"

.end
