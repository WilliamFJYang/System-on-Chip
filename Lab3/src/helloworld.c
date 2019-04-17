/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "myip1.h"
#include "sleep.h"
#include "xil_printf.h"

XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of the controller */

int XPAR_FABRIC_MYIP_0_INTR_INTR = 61;//######interrupt number##########

void INTERRUPT_Handler0(void *baseaddr_p){
	int i;
	print("Push Button was pressed\r\n");
	//xil_printf("Push Button was pressed\r\n");
	for(i=0;i<2;i++){
		MYIP1_mWriteReg(0x43C00000, MYIP1_S00_AXI_SLV_REG0_OFFSET, 0xf0);
		usleep(500000);
		MYIP1_mWriteReg(0x43C00000, MYIP1_S00_AXI_SLV_REG0_OFFSET, 0x0f);
		usleep(500000);
	}
}

int ScuGicInterrupt_Init(void)
	{
	int Status;
	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 * */
	Xil_ExceptionInit();

	GicConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
	GicConfig->CpuBaseAddress);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the Interrupt System
	 * */

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
	(Xil_ExceptionHandler) XScuGic_InterruptHandler,
	(void *) &InterruptController);

	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler performs
	 * the specific interrupt processing for the device
	 * since the xparameters.h file doesnt detect the external interrupts, we have to manually
	 * use the IRQ_F2P port numbers ; 91, 90, ect..
	 */

	Status = XScuGic_Connect(&InterruptController,XPAR_FABRIC_MYIP_0_INTR_INTR,
	(Xil_ExceptionHandler)INTERRUPT_Handler0,
	(void *)&InterruptController);

	XScuGic_Enable(&InterruptController, XPAR_FABRIC_MYIP_0_INTR_INTR);
	/*
	 * Enable interrupts in the ARM
	 */
	Xil_ExceptionEnable();
	/*
	 * Set interrupts PriorityTriggerType
	 */
	XScuGic_SetPriorityTriggerType(&InterruptController, XPAR_FABRIC_MYIP_0_INTR_INTR,
	0xa0, 3);

	if (Status != XST_SUCCESS)
		return XST_FAILURE;
}

int main()
{

	int xstatus;

	// Interrupt Test

	xstatus = ScuGicInterrupt_Init();

	if (xstatus != XST_SUCCESS)
		return XST_FAILURE;

	// Wait For interrupt;
	print("Hello World\n\r");
	print("Wait for the interrupt to trigger \r\n");
	print("########################################\r\n");
	print(" \r\n");
	int i=0;
	while(1){
		for(i=0;i<=255;i++){
			MYIP1_mWriteReg(0x43C00000, MYIP1_S00_AXI_SLV_REG0_OFFSET, i);
			printf("i = %d \r\n",i);
			sleep(1);
		}
	}
	cleanup_platform();
	return 0;
}
