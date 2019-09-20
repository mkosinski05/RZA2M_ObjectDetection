/*
 * r_bcd_ai.h
 *
 *  Created on: 2018/05/18
 *      Author: 16194PP29
 */

#ifndef INC_R_BCD_AI_H_
#define INC_R_BCD_AI_H_

#define R_BCD_BAYER_RGB
//#define R_BCD_AI_INPUT_128
#define R_BCD_AI_INPUT_64

//wang_test
#define R_BCD_BAYER_CAMERA
#define R_BCD_AI_INPUT_RGB
//#define R_BCD_AI_INPUT_GRY

//#define R_BCD_DRP_AI_DISABLE

#include"r_bcd_camera.h"


#if defined(R_BCD_AI_INPUT_128)
#define R_BCD_AI_INPUT_X 	128
#define R_BCD_AI_INPUT_Y 	128
#elif defined(R_BCD_AI_INPUT_64)
#define R_BCD_AI_INPUT_X 	64
#define R_BCD_AI_INPUT_Y 	64
#endif

#ifdef R_BCD_BAYER_CAMERA
#if defined(R_BCD_AI_INPUT_RGB)
#define R_BCD_BAYER_RGB
#else
#define R_BCD_BAYER_GRY
#endif
#endif

float R_BCD_AI(unsigned char *inputdata);



#endif /* INC_R_BCD_AI_H_ */
