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
/******************************************************************************
* System Name : [RZ/A2M] DRP dynamic loading sample program 3
* File Name   : readme-e.txt
******************************************************************************/
/**********************************************************************************
*
* History     : May. 31,2019 Rev.1.00.00    First edition issued
***********************************************************************************/

1. Before Use

  This sample program has been run and confirmed by the CPU board (RTK7921053C00000BE) 
  and the SUB board (RTK79210XXB00000BE) with the RZ/A2M group R7S921053VCBG. 
  Use this Application note and the sample program as a reference 
  for your software development.


  ****************************** CAUTION ******************************
   This sample programs are all reference, and no one to guarantee the 
   operation. Please use this sample program for the technical 
   reference when customers develop software.
  ****************************** CAUTION ******************************


2. Directory Configuration
  Refer to "RZ/A2M Group 2D Barcode Package Release Note(R01AN4487)" 
  for directory configuration of sample program.


3. System Verification
  This sample program was developed and tested with the following components:

    CPU                                 : RZ/A2M
    Board                               : RZ/A2M CPU board (RTK7921053C00000BE)
                                          RZ/A2M SUB board (RTK79210XXB00000BE)
                                          Display Output Board (RTK79210XXB00010BE)
    Compiler                            : GNU Arm Embedded Toolchain 6-2017-q2-update
    Integrated development environment  : e2 studio Version 7.4.0.
    Emulator                            : SEGGER J-Link Base
                                          (Customers are required to prepare J-Link emulator compatible with RZ/A2M.)
    Camera                              : Raspberry Pi Camera V2
    Monitor                             : Monitor compatible with VGA(640*480) resolution


4. About Sample Programs

  This sample programs operate the following processing.

    DRP Basic Operation Sample Program
      Converts the input image from MIPI camera to grayscale image
      using DRP and outputs to display.

    DRP Parallel Operation Sample Program
      Converts the input image from MIPI camera to grayscale image at high speed
      using parallel operation function of DRP and outputs to display.

    DRP Dynamic Loading Sample Program 1
      Detects the edges of the input image from MIPI camera
      by Canny method using DRP and outputs to display.

    DRP Dynamic Loading Sample Program 2
      Detects the corners of the input image from MIPI camera
      by the Harris corner detector using DRP and outputs to display.

*    DRP Dynamic Loading Sample Program 3
*    After detecting the edges of the input image from MIPI camera
*    by Canny method using DRP, performs contour detection using findcontours application.
*    The results output to display.


5. Operation Confirmation Conditions

  (1) Boot mode
    - Boot mode 3
      (Boot from serial flash memory 3.3V)
      * The program can not be operated if the boot mode except the above is specified.

  (2) Operating frequency
      The RZ/A2M clock pulse oscillator is set to see that the RZ/A2M clocks on the CPU board
      board have the following frequencies. 
      (The frequencies indicate the values in the state that the clock with 24MHz
      is input to the EXTAL pin in RZ/A2M clock mode 1.)
      - CPU clock (I clock)                 : 528MHz
      - Image processing clock (G clock)    : 264MHz
      - Internal bus clock (B clock)        : 132MHz
      - Peripheral clock1 (P1 clock)        :  66MHz
      - Peripheral clock0 (P0 clock)        :  33MHz
      - QSPI0_SPCLK                         :  66MHz
      - CKIO                                : 132MHz

  (3) Serial flash memory used
    - Manufacturer  : Macronix Inc.
    - Product No.   : MX25L51245G

  (4) Setting for cache
      Initial setting for the L1 and L2 caches is executed by the MMU. 
      Refer to the "RZ/A2M group Example of Initialization" application note(R01AN4321)
      about "Setting for MMU" for the valid/invalid area of L1 and L2 caches.


6. Operational Procedure

  Use the following procedure to execute this sample programs.

  (1) Setting for DIP switches and jumpers  
    Set the DIP switches and jumpers of the CPU board as follows.

     <<Setting for CPU board>>
      - SW1-1  : ON  (Disabled SSCG function)
        SW1-2  : OFF (Setting to clock mode 1(EXTAL input clock frequency : 20 to 24MHz))
        SW1-3  : ON  (MD_BOOT2 = L)
        SW1-4  : OFF (MD_BOOT1 = H)
        SW1-5  : OFF (MD_BOOT0 = H)
        SW1-6  : ON  (BSCANP normal mode (CoreSight debug mode))
        SW1-7  : ON  (CLKTEST OFF)
        SW1-8  : ON  (TESTMD OFF)
      - JP1 :   1-2  (Setting to supply 3.3V power for PVcc_SPI of RZ/A2M and U2)
      - JP2 :   2-3  (Setting to supply 1.8V power for PVcc_HO of RZ/A2M and U3)
      - JP3 :   Open (Use USB ch 0 in the function mode (Not supply VBUS0 power))

    Set the DIP switches and jumpers of the SUB board as follows.

     <<Setting for SUB board>>
      - SW6-1  : OFF (Setting to use P9_[7:0], P8_[7:1], P2_2, P2_0, P1_3, P1_[1:0], P0_[6:0], P6_7, P6_5, P7_[1:0] and P7[5:3] 
                      as DRP, audioÅAUARTÅACAN and USB interface terminals respectively)
        SW6-2  : OFF (Setting to use P8_4, P8_[7:6], P6_4 and P9_[6:3] as audio interface terminals)
        SW6-3  : OFF (Setting to use P9_[1:0], P1_0 and P7_5 as UART and USB interface terminals respectively)
        SW6-4  : OFF (Setting to use P6_[3:1] and PE_[6:0] as CEU terminals)
        SW6-5  : OFF (Setting to use P3_[5:1], PH_5 and PK_[4:0] as FLCTL terminals)
        SW6-6  : ON  (Setting to use PJ_[7:6] as VDC6 terminals)
        SW6-7  : ON  (Setting to use P7_[7:4] as VDC6 terminals)
        SW6-8  : OFF (NC)
        SW6-9  : OFF (P5_3 = "H")
        SW6-10 : OFF (PC_2 = "H")

      - JP1 : 2-JP2  (Setting to use PJ_1 as interrupt terminal for IRQ0 switch (SW3))

    Refer to the CPU board and the SUB board user's manual(R20UT4239,R20UT4240)
    for more details about setting for the DIP switches and jumpers.

  (2) Setting up sample program
    Copy the sample program directory to the e2 studio workspace directory
    in the host PC (e.g.: "C:\e2studio_Workspace_v740").

    Sample program directories:
      DRP Basic Operation Sample Program
        Software\Sample\rza2m_drp_basic_sample_freertos_gcc
      DRP Parallel Operation Sample Program
        Software\Sample\rza2m_drp_parallel_sample_freertos_gcc
      DRP Dynamic Loading Sample Program 1
        Software\Sample\rza2m_drp_dynamic_sample1_freertos_gcc
      DRP Dynamic Loading Sample Program 2
        Software\Sample\rza2m_drp_dynamic_sample2_freertos_gcc
*     DRP Dynamic Loading Sample Program 3
*       Software\Sample\rza2m_drp_dynamic_sample3_freertos_gcc

  (3) Launching integrated development environment
    Launch the integrated development environment e2 studio.

  (4) Building sample program
    After importing project of sample program by the e2 studio menu, build the project 
    and generate an elf file that is an executable file.

    Project names of sample program:
      DRP Basic Operation Sample Program
*        rza2m_drp_basic_sample_freertos_gcc
      DRP Parallel Operation Sample Program
*        rza2m_drp_parallel_sample_freertos_gcc
      DRP Dynamic Loading Sample Program 1
*        rza2m_drp_dynamic_sample1_freertos_gcc
      DRP Dynamic Loading Sample Program 2
*        rza2m_drp_dynamic_sample2_freertos_gcc
*     DRP Dynamic Loading Sample Program 3
*        rza2m_drp_dynamic_sample3_freertos_gcc

  (5) Connecting with emulator
    Connect the J-Link Base and the connector on the CPU board with JTAG cable. 
    When connecting, it is required the conversion adapter "J-Link 19-pin Cortex-M Adapter" made by SEGGER.

  (6) Downloading sample program
    Select "Debug Configuration" by the e2 studio "Run" menu and open the "Debug Configuration" dialog box.
    Select "Renesas GDB Hardware Debugging" from the list and display the detail list.
    Select the debug configuration ["Project name" HardwareDebug] of the loader program, 
    and download the executable file of the loader program generated in (4) to the serial flash memory 
    by pressing the "Debug" button.
    Reset the CPU by pressing the reset button in the e2 studio menu bar.

  (7) Downloading boot loader
    Select the "rza2_qspi_flash_ddr_bootloader.elf" from pull-down menu of "download" button,
    and boot loader is downloaded to the serial flash memory. On the next screen, ensure Load Type is "image only", 
    Address is 0 then press OK to download bootloader to serial flash.

  (8) Executing sample program
    Press reset on the e2 studio menu bar to start the debug session and the "Resume" button to run the code,
    Application code will be run after processing boot loader.
