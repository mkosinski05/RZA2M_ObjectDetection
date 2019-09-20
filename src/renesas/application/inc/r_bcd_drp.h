/*
 * r_bcd_drp.h
 *
 *  Created on: Aug 27, 2019
 *      Author: REA
 */

#ifndef RENESAS_APPLICATION_INC_R_BCD_DRP_H_
#define RENESAS_APPLICATION_INC_R_BCD_DRP_H_

#include "r_typedefs.h"

int R_BCD_drp_Isp ( uint8_t *psrc, uint8_t *pdst );
int R_BCD_drp_Resize ( uint8_t *psrc, uint8_t *pdst );
int R_BCD_drp_Gausian_blur ( uint8_t *psrc, uint8_t *pdst );
int R_BCD_drp_Canny ( uint8_t *psrc, uint8_t *pdst, uint8_t *pwork );
int R_BCD_drp_Dilate ( uint8_t *psrc, uint8_t *pdst );
int R_BCD_drp_FindContourse ( uint8_t *psrc, uint8_t *pwork );
int R_BCD_drp_FindContour_Overlay ( void );
int R_BCD_drp_rgb16torgb32 ( uint8_t *output_bufadr );



#endif /* RENESAS_APPLICATION_INC_R_BCD_DRP_H_ */
