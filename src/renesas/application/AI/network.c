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
* File Name    : network.c
* Version      : 1.00
* Description  : Definitions of all functions
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.06.2017 1.00     First Release
***********************************************************************************************************************/

#include <math.h>
#include "Typedef.h"
/***********************************************************************************************************************
* Function Name: padding
* Description  : - Padding operation
*				 - Creates a new output matrix with zero's padded on the original input 
* Arguments    : dData		- Pointer to the input data
*				 dPad		- Pointer to the padding output
*				 iShapes	- Dimensions 
*					( N, input channels, input height, input width, output height, output width ,pad top, pad left)
* Return Value : no return value
***********************************************************************************************************************/
void padding(TPrecision *dData, TPrecision *dPad, TsInt *iShapes)
{
	TsInt iD1,iD2,iD3,iD4;
	TsInt iN = iShapes[0];		// Number of examples
	TsInt iC = iShapes[1];		// Number of channels
	TsInt iH = iShapes[2];		// Input image height
	TsInt iW = iShapes[3];		// Input image width
	TsInt iPH = iShapes[4];		// Output height
	TsInt iPW = iShapes[5];		// Output width
	TsInt pad_top = iShapes[6];	// Number of pixels to be added to the height
	TsInt pad_left = iShapes[7];	// Number of pixels to be added to the width

	// Padding operation
	for(iD1=0; iD1 < iN*iC*iPH*iPW; iD1++)
		dPad[iD1]=0;

	for(iD1=0; iD1 < iN; iD1++)
	{
		for(iD2 = 0; iD2 < iC; iD2++)
		{
			for(iD3 = pad_top; iD3 < iH + pad_top; iD3++)
			{				
				for(iD4 = pad_left; iD4 < iW + pad_left; iD4++)
				{
					dPad[(iD1*iC*iPH*iPW)+(iD2*iPH*iPW)+(iD3*iPW)+(iD4)] = dData[(iD1*iC*iH*iW)+(iD2*iH*iW)+((iD3-pad_top)*iW)+(iD4-pad_left)];
				}
			}
		}
	}
}

/***********************************************************************************************************************
* Function Name: convolution_without_pad
* Description  : - Convolution layer without padding operation.
*				 - Performs elementwise multiplication of selected input data and weights and add them up with biases
*				   by taking in the filter size and strides as the convolution parameters
* Arguments    : dData		- Pointer to the input data
*			 	 dWeights	- Pointer to the weights
*				 dBiases	- Pointer to the biases
*				 dOut		- Pointer to the convolution output (to be filled with values during convolution operation)
*				 iShapes	- Dimensions 
*					( N, Channels, input height, input width, No. of filters, channels, filter height, filter width,
*					  output height, output width, stride height, stride width)
* Return Value : no return value
***********************************************************************************************************************/
void convolution_without_pad(TPrecision *dData,const TPrecision *dWeights,const TPrecision *dBiases,TPrecision *dOut,TsInt *iShapes){
	TsInt iD1,iD2,iD3,iD4;   // Dimensions
	TsInt iItr1,iItr2,iItr3;
	TsInt iN = iShapes[0];     // Number of examples
	TsInt iC = iShapes[1];     // Number of channels
	TsInt iH = iShapes[2];     // Input image height
	TsInt iW = iShapes[3];     // Input image width
	TsInt iF = iShapes[4];     // Number of filters
	TsInt iFH = iShapes[6];    // Filter height
	TsInt iFW = iShapes[7];    // Filter width
	TsInt iHp = iShapes[8];    // Output height
	TsInt iWp = iShapes[9];    // Output width

	TsInt iSH = iShapes[10];  // Stride height
	TsInt iSW = iShapes[11];  // Stride width

	TsInt iws, ihs ;
	TPrecision dvalue;
		
	for(iD1=0;iD1<iN*iF*iHp*iWp;iD1++)
		dOut[iD1]=0;
	
	// Filtering operation
	for (iD1=0; iD1<iN; iD1++)
	{
	    for (iD2=0; iD2<iF; iD2++)
	    {
	    	for (iD3=0; iD3<iHp;iD3++)
	    	{
	    		ihs = iD3 * iSH;
	    		//Conv operation for each channel
	    		for (iD4=0; iD4<iWp; iD4++)
	    		{
	    			iws = iD4 * iSW;
				dvalue=0;
				for(iItr1=0; iItr1<iC; iItr1++)
				{
					for(iItr2=ihs; iItr2<(ihs+iFH); iItr2++)
					{
						for(iItr3=iws; iItr3<(iws+iFW); iItr3++)
						{
							dvalue += dData[(iD1*iC*iH*iW)+(iItr1*iH*iW)+(iItr2*iW)+iItr3] * dWeights[(iD2*iC*iFH*iFW)+(iItr1*iFH*iFW)+((iItr2-ihs)*iFW)+(iItr3-iws)];
						}
					}
				}
				dOut[(iD1*iF*iHp*iWp)+(iD2*iHp*iWp)+(iD3*iWp)+iD4] = dvalue + dBiases[iD2];	// out = (data*weights)+biases
	    		}
	    	}
	    }
	}
}


/*
The inner for loop(48 to 58) is doing the convolution operation for each channel and adding it together to get one single value
ihs and iws is for starting index and (ihs + iFH) and (iws+iFW) is the last index for a particular kernel

dData shape is iN *iC * iH * iW
So the indexing is based on that

dWeights shape is iF * iC * iFH * iFW

All iItr's are for Kernel
All iD's are for the final output
*/

/***********************************************************************************************************************
* Function Name: relu
* Description  : - Rectified Linear Unit (ReLU)
*                - element wise operation and replaces all negative values by zero to introduce non-linearity
* Arguments    : dData	- Array of input data
*                iShapes	- Size of the input array
* Return Value : no return value
***********************************************************************************************************************/
void relu(TPrecision *dData, TsInt iShapes )
{
    TsInt iRow;
    for (iRow=0; iRow<iShapes; iRow++)
    {
        if (dData[iRow] < 0)
        {
        	dData[iRow] = 0;
        }
    }
}

/***********************************************************************************************************************
* Function Name: average_pooling_without_pad
* Description  : - AVERAGE pooling layer without padding operation.
*				 - Creates a new output matrix where each element is the average of a region in the original input.
* Arguments    : dData		- Pointer to the input data
*				 dOut		- Pointer to the average pooling output
*				 iShapes	- Dimensions 
*					( N, input channels, input height, input width, output height, output width, pool height, 
*					   pool width, stride height, stride width, pooling type)
* Return Value : no return value
***********************************************************************************************************************/
void average_pooling_without_pad(TPrecision *dData, TPrecision *dOut, TsInt *iShapes)
{

	TsInt sD1,sD2,sD3,sD4;			// Array dimension
	TsInt sInnerItr,sOuterItr,sOffset;
	TsInt iN = iShapes[0];
	TsInt iC = iShapes[1];			//Input Channels
	TsInt iH = iShapes[2];			// Input data height
	TsInt iW = iShapes[3];			// Input data width
	TsInt iHp = iShapes[4];			// Output data height
	TsInt iWp = iShapes[5];			// Output data width

	TsInt PH = iShapes[6];			// Pool height
	TsInt PW = iShapes[7];			// Pool width
	TsInt stride_H = iShapes[8];	// Stride height
	TsInt stride_W = iShapes[9];	// Stride width

	TsInt iWs, iHs;
	TPrecision dAve;

	// Average pooling operation
	for (sD1=0; sD1<iN; sD1++)
	{
	    for (sD2=0; sD2<iC; sD2++)
	    {
	    	for (sD3=0; sD3<iHp; sD3++)
	    	{
	    		iHs = sD3 * stride_H;
	    		for (sD4=0; sD4<iWp; sD4++)
	    		{
	    			iWs = sD4 * stride_W;
	    			for(sInnerItr=iHs; sInnerItr<(iHs+PH); sInnerItr++)
	    			{
	    				for(sOuterItr=iWs; sOuterItr<(iWs+PW); sOuterItr++)
	    				{
	    					sOffset = (sD1*iC*iH*iW)+(sD2*iH*iW)+(sInnerItr*iW)+sOuterItr;
	    					dAve += dData[sOffset];
	    				}
	    			}
	    			dOut[(sD1*iC*iHp*iWp)+(sD2*iHp*iWp)+(sD3*iWp)+sD4] = dAve/(PH*PW);
	    		}
	    	}
	    }
	}		
}

/***********************************************************************************************************************
* Function Name: max_pooling_without_pad
* Description  : - MAX Pooling layer without padding operation.
*				 - taking the most responsive node of the given TsInterest region
*				 - Creates a new output matrix where each element is the max of a region in the original input
* Arguments    : dData		- Pointer to the input data
*				 dOut		- Pointer to the max pooling output
*				 iShapes	- Dimensions 
*					( N, input channels, input height, input width, output height, output width, pool height, 
*					  pool width, stride height, stride width, pooling type)
* Return Value : no return value
***********************************************************************************************************************/
void max_pooling_without_pad(TPrecision *dData, TPrecision *dOut, TsInt *iShapes)
{

	TsInt sD1,sD2,sD3,sD4;			// Array dimension
	TsInt sInnerItr,sOuterItr,sOffset;
	TsInt iN = iShapes[0];
	TsInt iC = iShapes[1];			//Input Channels
	TsInt iH = iShapes[2];			// Input data height
	TsInt iW = iShapes[3];			// Input data width
	TsInt iHp = iShapes[4];			// Output data height
	TsInt iWp = iShapes[5];			// Output data width

	TsInt PH = iShapes[6];			// Pool height
	TsInt PW = iShapes[7];			// Pool width
	TsInt stride_H = iShapes[8];	// Stride height
	TsInt stride_W = iShapes[9];	// Stride width

	TsInt iWs, iHs, flag;
	TPrecision dMax;

	// MAX pooling operation
	for (sD1=0; sD1<iN; sD1++)
	{
	    for (sD2=0; sD2<iC; sD2++)
	    {
	    	for (sD3=0; sD3<iHp; sD3++)
	    	{
	    		iHs = sD3 * stride_H;
	    		for (sD4=0; sD4<iWp; sD4++)
	    		{
	    			iWs = sD4 * stride_W;
	    			flag = 0;
	    			for(sInnerItr=iHs; sInnerItr<(iHs+PH); sInnerItr++)
	    			{
	    				for(sOuterItr=iWs; sOuterItr<(iWs+PW); sOuterItr++)
	    				{
	    					sOffset = (sD1*iC*iH*iW)+(sD2*iH*iW)+(sInnerItr*iW)+sOuterItr;
	    					if(flag == 0)
	    					{
	    						dMax=dData[sOffset];
	    						flag = 1;
	    					}
	    					else
	    					{
	    						if(dMax < dData[sOffset])
	    							dMax = dData[sOffset];
	    					}
	    				}
	    			}
	    			dOut[(sD1*iC*iHp*iWp)+(sD2*iHp*iWp)+(sD3*iWp)+sD4] = dMax;
	    		}
	    	}
	    }
	}		
}

/***********************************************************************************************************************
* Function Name: pooling_without_pad
* Description  : - Pooling layer without padding operation.
*				 - Creates a new output matrix where each element is filled based on the pooling type
*					(either max pooling or average pooling)
* Arguments    : dData		- Pointer to the input data
*				 dOut		- Pointer to the pooling output
*				 iShapes	- Dimensions
*					( N, input channels, input height, input width, output height, output width, pool height,
*					  pool width, stride height, stride width, pooling type)
* Return Value : no return value
***********************************************************************************************************************/
void pooling_without_pad(TPrecision *dData, TPrecision *dOut, TsInt *iShapes)
{
	TsInt ipool_type = iShapes[10];		//Type of Pooling
	if (ipool_type == 0){		// MAX pooling
		max_pooling_without_pad(dData, dOut, iShapes);
	}
	else{						// AVERAGE pooling
		average_pooling_without_pad(dData, dOut, iShapes);
	}
}
        


/***********************************************************************************************************************
* Function Name: average_pooling
* Description  : - AVERAGE pooling layer
*				 - Creates a new output matrix where each element is the average of a region in the original input 
* Arguments    : dData		- Pointer to the input data
*				 dPad		- Pointer to the padding output
*				 dOut		- Pointer to the average pooling output
*				 iShapes	- Dimensions 
*					( N, input channels, input height, input width, output height, output width ,pad top, pad bottom , 
*					  pad left, pad right, pool height, pool width, stride height, stride width, pooling type)
* Return Value : no return value
***********************************************************************************************************************/
void average_pooling(TPrecision *dData, TPrecision *dPad, TPrecision *dOut, TsInt *iShapes)
{

	TsInt sD1,sD2,sD3,sD4;			// Array dimension
	TsInt sInnerItr,sOuterItr,sOffset;
	TsInt iN = iShapes[0];
	TsInt iC = iShapes[1];			//Input Channels
	TsInt iH = iShapes[2];			// Input data height
	TsInt iW = iShapes[3];			// Input data width
	TsInt iHp = iShapes[4];			// Output data height
	TsInt iWp = iShapes[5];			// Output data width

	TsInt pad_top = iShapes[6];		//No. of pixels to pad on top of input
	TsInt pad_bottom = iShapes[7];	//No. of pixels to pad on bottom of input
	TsInt pad_left = iShapes[8];	//No. of pixels to pad on left of input
	TsInt pad_right = iShapes[9];	//No. of pixels to pad on right of input

	TsInt PH = iShapes[10];			// Pool height
	TsInt PW = iShapes[11];			// Pool width
	TsInt stride_H = iShapes[12];	// Stride height
	TsInt stride_W = iShapes[13];	// Stride width

	TsInt iWs, iHs, iPH, iPW;
	TsInt pad_shapes[] = {iN, iC, iH, iW, iH + pad_top + pad_bottom, iW + pad_left + pad_right, pad_top, pad_left};
	TPrecision dAve;
	
	// Padding operation
	iPH = iH + pad_top + pad_bottom;
	iPW =iW + pad_left + pad_right;


	if (pad_top!=0 || pad_bottom!=0 || pad_left!=0 || pad_right!=0)
	{
		padding(dData, dPad, pad_shapes);
	}
	else
	{
		dPad = dData;
	}

	// Average pooling operation
	for (sD1=0; sD1<iN; sD1++)
	{
	    for (sD2=0; sD2<iC; sD2++)
	    {
	    	for (sD3=0; sD3<iHp; sD3++)
	    	{
	    		iHs = sD3 * stride_H;
	    		for (sD4=0; sD4<iWp; sD4++)
	    		{
	    			iWs = sD4 * stride_W;
	    			for(sInnerItr=iHs; sInnerItr<(iHs+PH); sInnerItr++)
	    			{
	    				for(sOuterItr=iWs; sOuterItr<(iWs+PW); sOuterItr++)
	    				{
	    					sOffset = (sD1*iC*iPH*iPW)+(sD2*iPH*iPW)+(sInnerItr*iPW)+sOuterItr;
	    					dAve += dPad[sOffset];
	    				}
	    			}
	    			dOut[(sD1*iC*iHp*iWp)+(sD2*iHp*iWp)+(sD3*iWp)+sD4] = dAve/(PH*PW);
	    		}
	    	}
	    }
	}		
}

/***********************************************************************************************************************
* Function Name: max_pooling
* Description  : - MAX Pooling layer
*				 - taking the most responsive node of the given TsInterest region
*				 - Creates a new output matrix where each element is the max of a region in the original input 
* Arguments    : dData		- Pointer to the input data
*				 dPad		- Pointer to the padding output
*				 dOut		- Pointer to the max pooling output
*				 iShapes	- Dimensions
*					( N, input channels, input height, input width, output height, output width ,pad top, pad bottom , 
*					  pad left, pad right, pool height, pool width, stride height, stride width, pooling type)
* Return Value : no return value
***********************************************************************************************************************/
void max_pooling(TPrecision *dData, TPrecision *dPad, TPrecision *dOut, TsInt *iShapes)
{

	TsInt sD1,sD2,sD3,sD4;			// Array dimension
	TsInt sInnerItr,sOuterItr,sOffset;
	TsInt iN = iShapes[0];
	TsInt iC = iShapes[1];			//Input Channels
	TsInt iH = iShapes[2];			// Input data height
	TsInt iW = iShapes[3];			// Input data width
	TsInt iHp = iShapes[4];			// Output data height
	TsInt iWp = iShapes[5];			// Output data width

	TsInt pad_top = iShapes[6];		//No. of pixels to pad on top of input
	TsInt pad_bottom = iShapes[7];	//No. of pixels to pad on bottom of input
	TsInt pad_left = iShapes[8];	//No. of pixels to pad on left of input
	TsInt pad_right = iShapes[9];	//No. of pixels to pad on right of input

	TsInt PH = iShapes[10];			// Pool height
	TsInt PW = iShapes[11];			// Pool width
	TsInt stride_H = iShapes[12];	// Stride height
	TsInt stride_W = iShapes[13];	// Stride width

	TsInt iWs, iHs, flag , iPH, iPW;
	TsInt pad_shapes[] = {iN, iC, iH, iW, iH + pad_top + pad_bottom, iW + pad_left + pad_right, pad_top, pad_left};
	TPrecision dMax;
	
	// Padding operation
	iPH = iH + pad_top + pad_bottom;
	iPW =iW + pad_left + pad_right;


	if (pad_top!=0 || pad_bottom!=0 || pad_left!=0 || pad_right!=0)
	{
		padding(dData, dPad, pad_shapes);
	}
	else
	{
		dPad = dData;
	}

	// MAX pooling operation
	for (sD1=0; sD1<iN; sD1++)
	{
	    for (sD2=0; sD2<iC; sD2++)
	    {
	    	for (sD3=0; sD3<iHp; sD3++)
	    	{
	    		iHs = sD3 * stride_H;
	    		for (sD4=0; sD4<iWp; sD4++)
	    		{
	    			iWs = sD4 * stride_W;
	    			flag = 0;
	    			for(sInnerItr=iHs; sInnerItr<(iHs+PH); sInnerItr++)
	    			{
	    				for(sOuterItr=iWs; sOuterItr<(iWs+PW); sOuterItr++)
	    				{
	    					sOffset = (sD1*iC*iPH*iPW)+(sD2*iPH*iPW)+(sInnerItr*iPW)+sOuterItr;
	    					if(flag == 0)
	    					{
	    						dMax=dPad[sOffset];
	    						flag = 1;
	    					}
	    					else
	    					{
	    						if(dMax < dPad[sOffset])
	    							dMax = dPad[sOffset];
	    					}
	    				}
	    			}
	    			dOut[(sD1*iC*iHp*iWp)+(sD2*iHp*iWp)+(sD3*iWp)+sD4] = dMax;
	    		}
	    	}
	    }
	}		
}


/***********************************************************************************************************************
* Function Name: pooling
* Description  : - Pooling layer
*				 - Creates a new output matrix where each element is filled based on the pooling type
*				   (either max pooling or average pooling) 
* Arguments    : dData		- Pointer to the input data
*				 dPad		- Pointer to the padding output
*				 dOut		- Pointer to the pooling output
*				 iShapes	- Dimensions
*					( N, input channels, input height, input width, output height, output width ,pad top, pad bottom ,
*					  pad left, pad right, pool height, pool width, stride height, stride width, pooling type)
* Return Value : no return value
***********************************************************************************************************************/
void pooling(TPrecision *dData, TPrecision *dPad, TPrecision *dOut, TsInt *iShapes)
{
	TsInt ipool_type = iShapes[14];		//Type of Pooling
	if (ipool_type == 0){		// MAX pooling
		max_pooling(dData, dPad, dOut, iShapes);
	}
	else{						// AVERAGE pooling
		average_pooling(dData, dPad, dOut, iShapes);
	}
}
        

/***********************************************************************************************************************
* Function Name: transpose
* Description  : Performs matrix transpose by TsInterchanging rows and columns
* Arguments    : dData		- array of input data
*				 dOut		- placeholder for the output
*				 iShapes	- dimensions of input array (i.e., data)
* Return Value : no return value
***********************************************************************************************************************/
void transpose(const TPrecision *dData,TPrecision *dOut,TsInt* iShapes){
	TsInt iRow,iColumn;
	TsInt row = iShapes[2];
	TsInt col = iShapes[3];
	
	for(iRow=0; iRow<row; iRow++)
	{
	  for(iColumn=0; iColumn<col; iColumn++)
	  {
		  dOut[(iColumn*row)+iRow] = dData[(iRow*col)+iColumn];
	  }
	}
}

/***********************************************************************************************************************
* Function Name: innerproduct
* Description  : - Fully connected layer
*                - Performs dot product of data and weights and add them up with biases
*                   (Matrix Multiplication of data and weights and addition of biases)
* Arguments    : data           - Array of input data
*                weight_trans   - Array of weights (transposed)
*                biases 		- Array of biases
*                out            - Placeholder for the output
*                shapes         - Dimensions of data and weights (N, D, F, D)
* Return Value : no return value
***********************************************************************************************************************/
void innerproduct(TPrecision *data,TPrecision *weight_trans,const TPrecision *biases,TPrecision *out,TsInt *shapes){
        TsInt iRow, iColumn;
		TsInt iInneritr;
        TsInt N = shapes[0];
        TsInt D = shapes[1];
        TsInt F = shapes[2];
        TPrecision dSum = 0;
        
        for(iRow=0; iRow<N; iRow++)
        {
          for(iColumn=0; iColumn<F; iColumn++)
          {
        	  dSum = 0;
        	  for(iInneritr=0; iInneritr<D;iInneritr++)
        	  {
            	dSum += data[(iRow*D)+iInneritr] * weight_trans[(iInneritr*F)+iColumn];
        	  }
            out[(iRow*F)+iColumn] = dSum + biases[iColumn];
          }
        }
}





/***********************************************************************************************************************
* Function Name: softmax
* Description  : - Activation function
*                - Squashes an array of arbitrary real values to an array of real values 
*                  in the range(0, 1) that add up to 1	
* Arguments    : dData      - Array of input data
*                iShapes	- Size of the input array
* Return Value : no return value
***********************************************************************************************************************/
void softmax( TPrecision *dData, TsInt iShapes )
{
    TPrecision dMax, dSum = 0;
    TsInt iRow;

    dMax = dData[0];
    for (iRow = 1; iRow < iShapes; iRow++)
    {
        if (dData[iRow] > dMax)
        {
        	dMax = dData[iRow];
        }
    }
    for (iRow = 0; iRow < iShapes; iRow++)
    {
    	dData[iRow] = dData[iRow] - dMax;
        dSum = dSum + exp(dData[iRow]);
    }
    for (iRow = 0; iRow < iShapes; iRow++)
    {
    	dData[iRow] = exp(dData[iRow])/dSum;
    }
}



