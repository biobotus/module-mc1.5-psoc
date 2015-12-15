/*******************************************************************************
* File Name: pwmStepper_1_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "pwmStepper_1.h"

static pwmStepper_1_backupStruct pwmStepper_1_backup;


/*******************************************************************************
* Function Name: pwmStepper_1_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmStepper_1_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void pwmStepper_1_SaveConfig(void) 
{

    #if(!pwmStepper_1_UsingFixedFunction)
        #if(!pwmStepper_1_PWMModeIsCenterAligned)
            pwmStepper_1_backup.PWMPeriod = pwmStepper_1_ReadPeriod();
        #endif /* (!pwmStepper_1_PWMModeIsCenterAligned) */
        pwmStepper_1_backup.PWMUdb = pwmStepper_1_ReadCounter();
        #if (pwmStepper_1_UseStatus)
            pwmStepper_1_backup.InterruptMaskValue = pwmStepper_1_STATUS_MASK;
        #endif /* (pwmStepper_1_UseStatus) */

        #if(pwmStepper_1_DeadBandMode == pwmStepper_1__B_PWM__DBM_256_CLOCKS || \
            pwmStepper_1_DeadBandMode == pwmStepper_1__B_PWM__DBM_2_4_CLOCKS)
            pwmStepper_1_backup.PWMdeadBandValue = pwmStepper_1_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(pwmStepper_1_KillModeMinTime)
             pwmStepper_1_backup.PWMKillCounterPeriod = pwmStepper_1_ReadKillTime();
        #endif /* (pwmStepper_1_KillModeMinTime) */

        #if(pwmStepper_1_UseControl)
            pwmStepper_1_backup.PWMControlRegister = pwmStepper_1_ReadControlRegister();
        #endif /* (pwmStepper_1_UseControl) */
    #endif  /* (!pwmStepper_1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: pwmStepper_1_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmStepper_1_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void pwmStepper_1_RestoreConfig(void) 
{
        #if(!pwmStepper_1_UsingFixedFunction)
            #if(!pwmStepper_1_PWMModeIsCenterAligned)
                pwmStepper_1_WritePeriod(pwmStepper_1_backup.PWMPeriod);
            #endif /* (!pwmStepper_1_PWMModeIsCenterAligned) */

            pwmStepper_1_WriteCounter(pwmStepper_1_backup.PWMUdb);

            #if (pwmStepper_1_UseStatus)
                pwmStepper_1_STATUS_MASK = pwmStepper_1_backup.InterruptMaskValue;
            #endif /* (pwmStepper_1_UseStatus) */

            #if(pwmStepper_1_DeadBandMode == pwmStepper_1__B_PWM__DBM_256_CLOCKS || \
                pwmStepper_1_DeadBandMode == pwmStepper_1__B_PWM__DBM_2_4_CLOCKS)
                pwmStepper_1_WriteDeadTime(pwmStepper_1_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(pwmStepper_1_KillModeMinTime)
                pwmStepper_1_WriteKillTime(pwmStepper_1_backup.PWMKillCounterPeriod);
            #endif /* (pwmStepper_1_KillModeMinTime) */

            #if(pwmStepper_1_UseControl)
                pwmStepper_1_WriteControlRegister(pwmStepper_1_backup.PWMControlRegister);
            #endif /* (pwmStepper_1_UseControl) */
        #endif  /* (!pwmStepper_1_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: pwmStepper_1_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmStepper_1_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void pwmStepper_1_Sleep(void) 
{
    #if(pwmStepper_1_UseControl)
        if(pwmStepper_1_CTRL_ENABLE == (pwmStepper_1_CONTROL & pwmStepper_1_CTRL_ENABLE))
        {
            /*Component is enabled */
            pwmStepper_1_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            pwmStepper_1_backup.PWMEnableState = 0u;
        }
    #endif /* (pwmStepper_1_UseControl) */

    /* Stop component */
    pwmStepper_1_Stop();

    /* Save registers configuration */
    pwmStepper_1_SaveConfig();
}


/*******************************************************************************
* Function Name: pwmStepper_1_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmStepper_1_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void pwmStepper_1_Wakeup(void) 
{
     /* Restore registers values */
    pwmStepper_1_RestoreConfig();

    if(pwmStepper_1_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        pwmStepper_1_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
