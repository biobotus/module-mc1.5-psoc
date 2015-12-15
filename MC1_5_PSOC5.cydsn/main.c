/* ========================================
 *
 * Copyright Biobotus, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <stdio.h>
#include <math.h>
#include "Defines.h"
#include "CAN_Communication.h"
#include "ComputerCommunication.h"
#include "Termistor.h"
#include "memory.h"
#include "MagnetisationController.h"
#include "FanControler.h"
#include "PeltierControler.h"
#include "PIDControler.h"

// FIRST DEFINITION OF GLOBAL VARIABLES DEFINED IN Defines.h.
int messageToBeTreatedCount = 0;
int msgToBeTreatedIndex = 0;
int msgToBeStoredIndex = 0;
CAN_MSG receivedMsgs[MAX_MSG_BUFFER_SIZE];

MC1_5Module mc1_5Module[2];

// FIRST DEFINITION SECTION END

volatile uint8 peltier_update_flag = 0; 



/*===========================================================================*/
// prototypes
/*===========================================================================*/

/*
 * Used to initiate everything that is related to the TAC module.
 */
void initmc1_5Module();
uint8 treatMessage(uint8 *packet);


CY_ISR(pidInterruptHandler)
{
    peltier_update_flag = 1;
    TimerPID_STATUS;
}
CY_ISR(Stepper1InterruptHandler)
{
    updateMagnetisation(0);
}
CY_ISR(Stepper2InterruptHandler)
{
    updateMagnetisation(1);
}

void initTimerPID()
{
    isr_pid_StartEx(pidInterruptHandler);
    TimerPID_Start();
}

void initCAN()
{
    // Initializing required components for CAN communication
    CAN_1_Start();
    CAN_1_GlobalIntEnable();
    CAN_Standby_Write(0);
}

/*===========================================================================*/
// Main loop
/*===========================================================================*/
int main()
{

    /* Main principal sequence :
    1- Initialize components (CAN/Uart/thermistors/ADC)
    2- Identify the module to the master
    3- Wait for commands from the master
    4- Main loop :
        4.1 Verify if an instruction was received by looping through all the flags that can be set by the interrupt 
            driven by the CAN bus.        
    */
    
    // Initialize everything for the module, from communication to peripherals.
      
    initCAN();
    init_eeprom();
    initmc1_5Module();
    initTimerPID();
    
    CyGlobalIntEnable;
    
    
    for(;;)
    {
        
        //Verify if a message needs to be treated in the message received buffer.       
        if(messageToBeTreatedCount>0)
        {
            // Treat the last message received and echo it back on the CAN bus as an acknowledgement.
            CAN_MSG msgToTreat = getReceivedMessage(); 
            treatMessage(msgToTreat.msgBytes.byte);
        }
        
        // Update the peltier outputs of the module0
        if(peltier_update_flag)
        {
            updatePeltierOutput(0, (double)0.5);
            peltier_update_flag=0;
        }
    }   
    
}
/*===========================================================================*/
// definition
/*===========================================================================*/
void initmc1_5Module()
{
   /*Start all the hardware components required*/
	ADC_Start();    // Convertisseur analogique-numérique
    
    /* Start ADC, AMux, OpAmp components */
    Peltier1_PWM_Ctrl_Start();
    Peltier2_PWM_Ctrl_Start();
    
    // Initializing fan pwm

    pwmFan_1_Start();
    pwmFan_2_Start();
    
    
    initializeMagn();
    mc1_5Module[0].stepperPosition=loadStepperPositionFromEEPROM(0);
    if(mc1_5Module[0].stepperPosition<=STEP_FOR_MAGN_ON/2)
    {
        mc1_5Module[0].magnetisationState=magnetisationDisabled;
    }
    else
    {
        mc1_5Module[0].magnetisationState=magnetisationEnabled;
    }
    mc1_5Module[1].stepperPosition=loadStepperPositionFromEEPROM(1);
    if(mc1_5Module[1].stepperPosition<=STEP_FOR_MAGN_ON/2)
    {
        mc1_5Module[1].magnetisationState=magnetisationDisabled;
    }
    else
    {
        mc1_5Module[1].magnetisationState=magnetisationEnabled;
    }
    
    isr_Stepper1_StartEx(Stepper1InterruptHandler);
    isr_Stepper2_StartEx(Stepper2InterruptHandler);
    //Bloc of code to populate the EEPROM for the first time (DEFAULT VALUES)
    /*savePIDToEEPROM(0, 0.2, 0.01, 0);
    savePIDToEEPROM(1, 0.2, 0.01, 0);
    saveTargetTemperatureToEEPROM(0, 250);
    saveTargetTemperatureToEEPROM(1, 250);
    
    saveTemperatureLimitHighToEEPROM(0, 700);
    saveTemperatureLimitHighToEEPROM(1, 700);
    saveTemperatureLimitLowToEEPROM(0, 0);
    saveTemperatureLimitLowToEEPROM(1, 0);//*/
    
    loadPIDFromEEPROM(0, &mc1_5Module[0].kp, &mc1_5Module[0].ki, &mc1_5Module[0].kd);
    loadPIDFromEEPROM(1, &mc1_5Module[1].kp, &mc1_5Module[1].ki, &mc1_5Module[1].kd);
    mc1_5Module[0].temperatureLimitHigh = loadTemperatureLimitHighFromEEPROM(0);
    mc1_5Module[1].temperatureLimitHigh = loadTemperatureLimitHighFromEEPROM(1);
    
    mc1_5Module[0].targetTemperature = loadTargetTemperatureFromEEPROM(0);
    mc1_5Module[1].targetTemperature = loadTargetTemperatureFromEEPROM(1);
    
    
    // Make sure both Peltier "Enable" signal are set to 0.
    changePeltierModuleState(peltierEnabled,0);
    changePeltierModuleState(peltierEnabled,1);
    
    changeFanState(fanEnabled, 0);
    changeFanState(fanEnabled, 1);
}

//TODO: -Implement the switch for each instruction -> DONE
//      -Implement function for each intructions
//      -Implement changing of the message
//      -Send back the correct message to the master with the required value.
// This function is used to make the message that is send back to the PC and 
// send it as well as echoing the received message 
uint8 treatMessage(uint8 *packet)
{ 
    int16 tmp;
    uint8 module = packet[INDEX_MODULE_BYTE];
    
    //changeFanSpeed    
    switch (packet[0]) 
    {
        // TODO : Complete this section.        
        case SET_TARGET_TEMPERATURE:
        {   
            tmp = (uint16)(((uint16)packet[3]<<8) | packet[2]);
            mc1_5Module[module].targetTemperature = tmp;
            saveTargetTemperatureToEEPROM(module, tmp);
            resetPIDSumError();
            break;
        }
        case SET_FAN_ENABLE: // To be verified
        {
            changeFanState(packet[2], module);
            break;
        }
        case SET_FAN_SPEED: // To be verified
        {
            changeFanSpeed(packet[2], module);
            break;
        }
        case GET_FAN_STATE: // To be verified
        {
            sendDataToCAN(GET_FAN_STATE, module,mc1_5Module[module].fanState,mc1_5Module[module].fanSpeedPercentage,0,0,0);
            break;
        }
        case GET_CURRENT_TEMPERATURE: // Modify to return the temperature of the desired thermistor.
        {            
            sendDataToCAN(GET_CURRENT_TEMPERATURE, module,mc1_5Module[module].currentTemperature,mc1_5Module[module].currentTemperature>>8,0,0,0);
            break;
        }
        case SET_MAGN_POSITION: // To be verified.
        {
            changeMagnState(packet[2], module);
            break; 
        }
        case GET_MAGN_POSITION: // To be verified.
        {
            sendDataToCAN(GET_MAGN_POSITION, module,mc1_5Module[module].magnetisationState,0, 0,0,0);
            break; 
        }
        case SET_TEMPERATURE_PID_PARAM_P:
        {
            memcpy(&mc1_5Module[module].kp, &packet[4], sizeof(float));
            savePIDToEEPROM(module,mc1_5Module[module].kp, mc1_5Module[module].ki, mc1_5Module[module].kd);
            break;
        }
        case SET_TEMPERATURE_PID_PARAM_I:
        {
            memcpy(&mc1_5Module[module].ki, &packet[4], sizeof(float));
            savePIDToEEPROM(module,mc1_5Module[module].kp, mc1_5Module[module].ki, mc1_5Module[module].kd);
            break;
        }
        case SET_TEMPERATURE_PID_PARAM_D:
        {
            memcpy(&mc1_5Module[module].kd, &packet[4], sizeof(float));
            savePIDToEEPROM(module,mc1_5Module[module].kp, mc1_5Module[module].ki, mc1_5Module[module].kd);
            break;
        }
        case GET_TEMPERATURE_PID_PARAM_P:
        {
            sendFloatToCAN(GET_TEMPERATURE_PID_PARAM_P, module, &mc1_5Module[module].kp);
            break;
        }
        case GET_TEMPERATURE_PID_PARAM_I:
        {
            sendFloatToCAN(GET_TEMPERATURE_PID_PARAM_I, module, &mc1_5Module[module].ki);
            break;
        }
        case GET_TEMPERATURE_PID_PARAM_D:
        {
            sendFloatToCAN(GET_TEMPERATURE_PID_PARAM_D, module, &mc1_5Module[module].kd);
            //sendDataToCAN(module,GET_TEMPERATURE_PID_PARAM_D,k[0],k[1],k[2],k[3]);
            break;
        }
        case GET_TARGET_TEMPERATURE:
        {
            sendDataToCAN(GET_TARGET_TEMPERATURE, module ,mc1_5Module[module].targetTemperature,mc1_5Module[module].targetTemperature>>8, 0,0,0);
            break;
        }
        case SET_PELTIER_ENABLE:
        {
            changePeltierModuleState(packet[2],module);
            break;   
        }
		case SET_PELTIER_SPEED:
		{
			int8 value = packet[2];
			if(value < 0)
				changePeltierModuleCurrentDirection(Cooling, module);
			else
				changePeltierModuleCurrentDirection(Heating, module);
            changePeltierPWMValue(abs(value),module);
            break;   
		}
        case GET_PELTIER_STATE:
        {
            sendDataToCAN(GET_PELTIER_STATE, module, mc1_5Module[module].peltierState, mc1_5Module[module].pwmCompareValue, 0, 0, 0);
            break;
        }
        case SET_TEMPERATURE_LIMIT_HIGH:
        {
            mc1_5Module[module].temperatureLimitHigh = (packet[3]<<8) | packet[2];
            saveTemperatureLimitHighToEEPROM(module, mc1_5Module[module].temperatureLimitHigh);
            break;
        }
        case GET_TEMPERATURE_LIMIT_HIGH:
        {
            sendDataToCAN(GET_TEMPERATURE_LIMIT_HIGH, module, mc1_5Module[module].temperatureLimitHigh, mc1_5Module[module].temperatureLimitHigh>>8, 0, 0, 0);
            break;
        }
        case SET_TEMPERATURE_LIMIT_LOW:
        {
            mc1_5Module[module].temperatureLimitLow = (packet[3]<<8) | packet[2];
            saveTemperatureLimitLowToEEPROM(module, mc1_5Module[module].temperatureLimitLow);
            break;
        }
        case GET_TEMPERATURE_LIMIT_LOW:
        {
            sendDataToCAN(GET_TEMPERATURE_LIMIT_LOW, module, mc1_5Module[module].temperatureLimitLow, mc1_5Module[module].temperatureLimitLow>>8, 0, 0, 0);
            break;
        }
        case GET_MODEL_FIRMWARE:
            sendDataToCAN2(GET_MODEL_FIRMWARE, HARDWARE_CAN_ID, 'T', 'A', 'C', 'v', '1', '0');
        default:          
          break;
    }

    return 1;
}

