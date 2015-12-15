/*******************************************************************************
* File Name: pwmStepper_2_PM.c
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

#include "pwmStepper_2.h"

static pwmStepper_2_backupStruct pwmStepper_2_backup;


/*******************************************************************************
* Function Name: pwmStepper_2_SaveConfig
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
*  pwmStepper_2_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void pwmStepper_2_SaveConfig(void) 
{

    #if(!pwmStepper_2_UsingFixedFunction)
        #if(!pwmStepper_2_PWMModeIsCenterAligned)
            pwmStepper_2_backup.PWMPeriod = pwmStepper_2_ReadPeriod();
        #endif /* (!pwmStepper_2_PWMModeIsCenterAligned) */
        pwmStepper_2_backup.PWMUdb = pwmStepper_2_ReadCounter();
        #if (pwmStepper_2_UseStatus)
            pwmStepper_2_backup.InterruptMaskValue = pwmStepper_2_STATUS_MASK;
        #endif /* (pwmStepper_2_UseStatus) */

        #if(pwmStepper_2_DeadBandMode == pwmStepper_2__B_PWM__DBM_256_CLOCKS || \
            pwmStepper_2_DeadBandMode == pwmStepper_2__B_PWM__DBM_2_4_CLOCKS)
            pwmStepper_2_backup.PWMdeadBandValue = pwmStepper_2_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(pwmStepper_2_KillModeMinTime)
             pwmStepper_2_backup.PWMKillCounterPeriod = pwmStepper_2_ReadKillTime();
        #endif /* (pwmStepper_2_KillModeMinTime) */

        #if(pwmStepper_2_UseControl)
            pwmStepper_2_backup.PWMControlRegister = pwmStepper_2_ReadControlRegister();
        #endif /* (pwmStepper_2_UseControl) */
    #endif  /* (!pwmStepper_2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: pwmStepper_2_RestoreConfig
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
*  pwmStepper_2_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void pwmStepper_2_RestoreConfig(void) 
{
        #if(!pwmStepper_2_UsingFixedFunction)
            #if(!pwmStepper_2_PWMModeIsCenterAligned)
                pwmStepper_2_WritePeriod(pwmStepper_2_backup.PWMPeriod);
            #endif /* (!pwmStepper_2_PWMModeIsCenterAligned) */

            pwmStepper_2_WriteCounter(pwmStepper_2_backup.PWMUdb);

            #if (pwmStepper_2_UseStatus)
                pwmStepper_2_STATUS_MASK = pwmStepper_2_backup.InterruptMaskValue;
            #endif /* (pwmStepper_2_UseStatus) */

            #if(pwmStepper_2_DeadBandMode == pwmStepper_2__B_PWM__DBM_256_CLOCKS || \
                pwmStepper_2_DeadBandMode == pwmStepper_2__B_PWM__DBM_2_4_CLOCKS)
                pwmStepper_2_WriteDeadTime(pwmStepper_2_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(pwmStepper_2_KillModeMinTime)
                pwmStepper_2_WriteKillTime(pwmStepper_2_backup.PWMKillCounterPeriod);
            #endif /* (pwmStepper_2_KillModeMinTime) */

            #if(pwmStepper_2_UseControl)
                pwmStepper_2_WriteControlRegister(pwmStepper_2_backup.PWMControlRegister);
            #endif /* (pwmStepper_2_UseControl) */
        #endif  /* (!pwmStepper_2_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: pwmStepper_2_Sleep
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
*  pwmStepper_2_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void pwmStepper_2_Sleep(void) 
{
    #if(pwmStepper_2_UseControl)
        if(pwmStepper_2_CTRL_ENABLE == (pwmStepper_2_CONTROL & pwmStepper_2_CTRL_ENABLE))
        {
            /*Component is enabled */
            pwmStepper_2_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            pwmStepper_2_backup.PWMEnableState = 0u;
        }
    #endif /* (pwmStepper_2_UseControl) */

    /* Stop component */
    pwmStepper_2_Stop();

    /* Save registers configuration */
    pwmStepper_2_SaveConfig();
}


/*******************************************************************************
* Function Name: pwmStepper_2_Wakeup
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
*  pwmStepper_2_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void pwmStepper_2_Wakeup(void) 
{
     /* Restore registers values */
    pwmStepper_2_RestoreConfig();

    if(pwmStepper_2_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        pwmStepper_2_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
