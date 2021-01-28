/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Empty PSoC6 Application
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define TOGGLE_THREAD_PRIORITY (5)


SemaphoreHandle_t mySem;

void toggleThread(void *arg){
	while(1){
		//Aquire Mutex
		xSemaphoreTake(mySem, portMAX_DELAY);
		while(!cyhal_gpio_read(CYBSP_USER_BTN)){
			cyhal_gpio_toggle(CYBSP_USER_LED);
			vTaskDelay(100);
		}
		//Release Mutex
		xSemaphoreGive(mySem);
		//Yield control when button is not pressed
		vTaskDelay(1);
	}
}

void toggleThread2(void *arg){
	while(1){
		//Aquire Mutex
		xSemaphoreTake(mySem, portMAX_DELAY);
		while(!cyhal_gpio_read(CYBSP_USER_BTN2)){
			cyhal_gpio_toggle(CYBSP_USER_LED);
			vTaskDelay(200);
		}
		//Release Mutex
		xSemaphoreGive(mySem);
		//Yield control when button is not pressed
		vTaskDelay(1);
	}
}

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    /*BSP init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LED */
	result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

    /* Initialize the user button */
	result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initialize the user button2 */
	result = cyhal_gpio_init(CYBSP_USER_BTN2, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	// Enable Interrupts
    __enable_irq();

    //Create Mutex
    mySem = xSemaphoreCreateMutex();

    //Create the toggleThread task
    BaseType_t retval;
    retval = xTaskCreate(toggleThread, "toggleThread", 1024, NULL, TOGGLE_THREAD_PRIORITY, NULL);
    retval = xTaskCreate(toggleThread2, "toggleThread2", 1024, NULL, TOGGLE_THREAD_PRIORITY, NULL);
    //Start the task scheduler if the toggleThread task was created successfully
    if(pdPASS == retval){
    	vTaskStartScheduler();
    }
}

/* [] END OF FILE */