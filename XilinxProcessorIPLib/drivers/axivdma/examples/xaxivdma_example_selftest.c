/******************************************************************************
*
* (c) Copyright 2015 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xaxivdma.h"
#include "xparameters.h"
#include "xdebug.h"

/******************** Constant Definitions **********************************/

/*
 * Device hardware build related constants.
 */

 #ifndef TESTAPP_GEN
#define DMA_DEVICE_ID		XPAR_AXIVDMA_0_DEVICE_ID
#endif

#define XAXIVDMA_RESET_TIMEOUT   500

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

int XAxiVdma_Selftest(u16 DeviceId);

/************************** Variable Definitions *****************************/
/*
 * Device instance definitions
 */
XAxiVdma AxiVdma;


/*****************************************************************************/
/*
* The entry point for this example. It invokes the example function,
* and reports the execution status.
*
* @param	None.
*
* @return
*		- XST_SUCCESS if example finishes successfully
*		- XST_FAILURE if example fails.
*
* @note		None.
*
******************************************************************************/
#ifndef TESTAPP_GEN
int main()
{
	int Status;

	xil_printf("\r\n--- Entering main() --- \r\n");

	/* Run the poll example for simple transfer */
	Status = XAxiVdma_Selftest(DMA_DEV_ID);

	if (Status != XST_SUCCESS) {

		xil_printf("XAxiVdma_Selftest: Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("XAxiDma_Selftest: Passed\r\n");

	xil_printf("--- Exiting main() --- \r\n");

	return XST_SUCCESS;

}
#endif

/*****************************************************************************/
/*
* This function performance a reset of the VDMA device and checks the device is
* coming out of reset or not.
*
* @param	DeviceId is the DMA device id.
*
* @return	- XST_SUCCESS if channel reset is successful
*		- XST_FAILURE if channel reset fails.
*
* @note		None.
*
******************************************************************************/
int XAxiVdma_Selftest(u16 DeviceId)
{
	XAxiVdma_Config *Config;
	int Status = XST_SUCCESS;
	XAxiVdma_Channel *RdChannel;
	XAxiVdma_Channel *WrChannel;
	int Polls;

	Config = XAxiVdma_LookupConfig(DeviceId);
	if (!Config) {
			return XST_FAILURE;
	}

	/* Initialize DMA engine */
	Status = XAxiVdma_CfgInitialize(&AxiVdma, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	if (Config->HasMm2S) {
			RdChannel = XAxiVdma_GetChannel(&AxiVdma, XAXIVDMA_READ);
			RdChannel->ChanBase = Config->BaseAddress + XAXIVDMA_TX_OFFSET;
			RdChannel->InstanceBase = Config->BaseAddress;
			XAxiVdma_ChannelReset(RdChannel);
			Polls = XAXIVDMA_RESET_TIMEOUT;

		while (Polls && XAxiVdma_ChannelResetNotDone(RdChannel)) {
			Polls -= 1;
		}

		if (!Polls) {
			return XST_FAILURE;
		}
	}

	if (Config->HasS2Mm) {
		WrChannel = XAxiVdma_GetChannel(&AxiVdma, XAXIVDMA_WRITE);
		WrChannel->ChanBase = Config->BaseAddress + XAXIVDMA_RX_OFFSET;
		WrChannel->InstanceBase = Config->BaseAddress;
		XAxiVdma_ChannelReset(WrChannel);

		Polls = XAXIVDMA_RESET_TIMEOUT;

		while (Polls && XAxiVdma_ChannelResetNotDone(WrChannel)) {
			Polls -= 1;
		}

		if (!Polls) {
			return XST_FAILURE;
		}

	}

	return Status;
}