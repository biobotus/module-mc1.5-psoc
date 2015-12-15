/*******************************************************************************
* File Name: pwmStepper_2.c
* Version 3.30
*
* Description:
*  The PWM User Module consist of an 8 or 16-bit counter with two 8 or 16-bit
*  comparitors. Each instance of this user module is capable of generating
*  two PWM outputs with the same period. The pulse width is selectable between
*  1 and 255/65535. The period is selectable between 2 and 255/65536 clocks.
*  The compare value output may be configured to be active when the present
*  counter is less than or less than/equal to the compare value.
*  A terminal count output is also provided. It generates a pulse one clock
*  width wide when the counter is equal to zero.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "pwmStepper_2.h"

/* Error message for removed <resource> through optimization */
#ifdef pwmStepper_2_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* pwmStepper_2_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 pwmStepper_2_initVar = 0u;


/*******************************************************************************
* Function Name: pwmStepper_2_Start
********************************************************************************
*
* Summary:
*  The start function initializes the pwm with the default values, the
*  enables the counter to begin counting.  It does not enable interrupts,
*  the EnableInt command should be called if interrupt generation is required.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmStepper_2_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void pwmStepper_2_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(pwmStepper_2_initVar == 0u)
    {
        pwmStepper_2_Init();
        pwmStepper_2_initVar = 1u;
    }
    pwmStepper_2_Enable();

}


/*******************************************************************************
* Function Name: pwmStepper_2_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  pwmStepper_2_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void pwmStepper_2_Init(void) 
{
    #if (pwmStepper_2_UsingFixedFunction || pwmStepper_2_UseControl)
        uint8 ctrl;
    #endif /* (pwmStepper_2_UsingFixedFunction || pwmStepper_2_UseControl) */

    #if(!pwmStepper_2_UsingFixedFunction)
        #if(pwmStepper_2_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 pwmStepper_2_interruptState;
        #endif /* (pwmStepper_2_UseStatus) */
    #endif /* (!pwmStepper_2_UsingFixedFunction) */

    #if (pwmStepper_2_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        pwmStepper_2_CONTROL |= pwmStepper_2_CFG0_MODE;
        #if (pwmStepper_2_DeadBand2_4)
            pwmStepper_2_CONTROL |= pwmStepper_2_CFG0_DB;
        #endif /* (pwmStepper_2_DeadBand2_4) */

        ctrl = pwmStepper_2_CONTROL3 & ((uint8 )(~pwmStepper_2_CTRL_CMPMODE1_MASK));
        pwmStepper_2_CONTROL3 = ctrl | pwmStepper_2_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        pwmStepper_2_RT1 &= ((uint8)(~pwmStepper_2_RT1_MASK));
        pwmStepper_2_RT1 |= pwmStepper_2_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        pwmStepper_2_RT1 &= ((uint8)(~pwmStepper_2_SYNCDSI_MASK));
        pwmStepper_2_RT1 |= pwmStepper_2_SYNCDSI_EN;

    #elif (pwmStepper_2_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = pwmStepper_2_CONTROL & ((uint8)(~pwmStepper_2_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~pwmStepper_2_CTRL_CMPMODE1_MASK));
        pwmStepper_2_CONTROL = ctrl | pwmStepper_2_DEFAULT_COMPARE2_MODE |
                                   pwmStepper_2_DEFAULT_COMPARE1_MODE;
    #endif /* (pwmStepper_2_UsingFixedFunction) */

    #if (!pwmStepper_2_UsingFixedFunction)
        #if (pwmStepper_2_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            pwmStepper_2_AUX_CONTROLDP0 |= (pwmStepper_2_AUX_CTRL_FIFO0_CLR);
        #else /* (pwmStepper_2_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            pwmStepper_2_AUX_CONTROLDP0 |= (pwmStepper_2_AUX_CTRL_FIFO0_CLR);
            pwmStepper_2_AUX_CONTROLDP1 |= (pwmStepper_2_AUX_CTRL_FIFO0_CLR);
        #endif /* (pwmStepper_2_Resolution == 8) */

        pwmStepper_2_WriteCounter(pwmStepper_2_INIT_PERIOD_VALUE);
    #endif /* (!pwmStepper_2_UsingFixedFunction) */

    pwmStepper_2_WritePeriod(pwmStepper_2_INIT_PERIOD_VALUE);

        #if (pwmStepper_2_UseOneCompareMode)
            pwmStepper_2_WriteCompare(pwmStepper_2_INIT_COMPARE_VALUE1);
        #else
            pwmStepper_2_WriteCompare1(pwmStepper_2_INIT_COMPARE_VALUE1);
            pwmStepper_2_WriteCompare2(pwmStepper_2_INIT_COMPARE_VALUE2);
        #endif /* (pwmStepper_2_UseOneCompareMode) */

        #if (pwmStepper_2_KillModeMinTime)
            pwmStepper_2_WriteKillTime(pwmStepper_2_MinimumKillTime);
        #endif /* (pwmStepper_2_KillModeMinTime) */

        #if (pwmStepper_2_DeadBandUsed)
            pwmStepper_2_WriteDeadTime(pwmStepper_2_INIT_DEAD_TIME);
        #endif /* (pwmStepper_2_DeadBandUsed) */

    #if (pwmStepper_2_UseStatus || pwmStepper_2_UsingFixedFunction)
        pwmStepper_2_SetInterruptMode(pwmStepper_2_INIT_INTERRUPTS_MODE);
    #endif /* (pwmStepper_2_UseStatus || pwmStepper_2_UsingFixedFunction) */

    #if (pwmStepper_2_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        pwmStepper_2_GLOBAL_ENABLE |= pwmStepper_2_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        pwmStepper_2_CONTROL2 |= pwmStepper_2_CTRL2_IRQ_SEL;
    #else
        #if(pwmStepper_2_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            pwmStepper_2_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            pwmStepper_2_STATUS_AUX_CTRL |= pwmStepper_2_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(pwmStepper_2_interruptState);

            /* Clear the FIFO to enable the pwmStepper_2_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            pwmStepper_2_ClearFIFO();
        #endif /* (pwmStepper_2_UseStatus) */
    #endif /* (pwmStepper_2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: pwmStepper_2_Enable
********************************************************************************
*
* Summary:
*  Enables the PWM block operation
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This works only if software enable mode is chosen
*
*******************************************************************************/
void pwmStepper_2_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (pwmStepper_2_UsingFixedFunction)
        pwmStepper_2_GLOBAL_ENABLE |= pwmStepper_2_BLOCK_EN_MASK;
        pwmStepper_2_GLOBAL_STBY_ENABLE |= pwmStepper_2_BLOCK_STBY_EN_MASK;
    #endif /* (pwmStepper_2_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (pwmStepper_2_UseControl || pwmStepper_2_UsingFixedFunction)
        pwmStepper_2_CONTROL |= pwmStepper_2_CTRL_ENABLE;
    #endif /* (pwmStepper_2_UseControl || pwmStepper_2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: pwmStepper_2_Stop
********************************************************************************
*
* Summary:
*  The stop function halts the PWM, but does not change any modes or disable
*  interrupts.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  If the Enable mode is set to Hardware only then this function
*  has no effect on the operation of the PWM
*
*******************************************************************************/
void pwmStepper_2_Stop(void) 
{
    #if (pwmStepper_2_UseControl || pwmStepper_2_UsingFixedFunction)
        pwmStepper_2_CONTROL &= ((uint8)(~pwmStepper_2_CTRL_ENABLE));
    #endif /* (pwmStepper_2_UseControl || pwmStepper_2_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (pwmStepper_2_UsingFixedFunction)
        pwmStepper_2_GLOBAL_ENABLE &= ((uint8)(~pwmStepper_2_BLOCK_EN_MASK));
        pwmStepper_2_GLOBAL_STBY_ENABLE &= ((uint8)(~pwmStepper_2_BLOCK_STBY_EN_MASK));
    #endif /* (pwmStepper_2_UsingFixedFunction) */
}

#if (pwmStepper_2_UseOneCompareMode)
    #if (pwmStepper_2_CompareMode1SW)


        /*******************************************************************************
        * Function Name: pwmStepper_2_SetCompareMode
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm output when in Dither mode,
        *  Center Align Mode or One Output Mode.
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void pwmStepper_2_SetCompareMode(uint8 comparemode) 
        {
            #if(pwmStepper_2_UsingFixedFunction)

                #if(0 != pwmStepper_2_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << pwmStepper_2_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != pwmStepper_2_CTRL_CMPMODE1_SHIFT) */

                pwmStepper_2_CONTROL3 &= ((uint8)(~pwmStepper_2_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                pwmStepper_2_CONTROL3 |= comparemodemasked;

            #elif (pwmStepper_2_UseControl)

                #if(0 != pwmStepper_2_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << pwmStepper_2_CTRL_CMPMODE1_SHIFT)) &
                                                pwmStepper_2_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & pwmStepper_2_CTRL_CMPMODE1_MASK;
                #endif /* (0 != pwmStepper_2_CTRL_CMPMODE1_SHIFT) */

                #if(0 != pwmStepper_2_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << pwmStepper_2_CTRL_CMPMODE2_SHIFT)) &
                                               pwmStepper_2_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & pwmStepper_2_CTRL_CMPMODE2_MASK;
                #endif /* (0 != pwmStepper_2_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                pwmStepper_2_CONTROL &= ((uint8)(~(pwmStepper_2_CTRL_CMPMODE1_MASK |
                                            pwmStepper_2_CTRL_CMPMODE2_MASK)));
                pwmStepper_2_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (pwmStepper_2_UsingFixedFunction) */
        }
    #endif /* pwmStepper_2_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (pwmStepper_2_CompareMode1SW)


        /*******************************************************************************
        * Function Name: pwmStepper_2_SetCompareMode1
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm or pwm1 output
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void pwmStepper_2_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != pwmStepper_2_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << pwmStepper_2_CTRL_CMPMODE1_SHIFT)) &
                                           pwmStepper_2_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & pwmStepper_2_CTRL_CMPMODE1_MASK;
            #endif /* (0 != pwmStepper_2_CTRL_CMPMODE1_SHIFT) */

            #if (pwmStepper_2_UseControl)
                pwmStepper_2_CONTROL &= ((uint8)(~pwmStepper_2_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                pwmStepper_2_CONTROL |= comparemodemasked;
            #endif /* (pwmStepper_2_UseControl) */
        }
    #endif /* pwmStepper_2_CompareMode1SW */

#if (pwmStepper_2_CompareMode2SW)


    /*******************************************************************************
    * Function Name: pwmStepper_2_SetCompareMode2
    ********************************************************************************
    *
    * Summary:
    *  This function writes the Compare Mode for the pwm or pwm2 output
    *
    * Parameters:
    *  comparemode:  The new compare mode for the PWM output. Use the compare types
    *                defined in the H file as input arguments.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmStepper_2_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != pwmStepper_2_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << pwmStepper_2_CTRL_CMPMODE2_SHIFT)) &
                                                 pwmStepper_2_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & pwmStepper_2_CTRL_CMPMODE2_MASK;
        #endif /* (0 != pwmStepper_2_CTRL_CMPMODE2_SHIFT) */

        #if (pwmStepper_2_UseControl)
            pwmStepper_2_CONTROL &= ((uint8)(~pwmStepper_2_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            pwmStepper_2_CONTROL |= comparemodemasked;
        #endif /* (pwmStepper_2_UseControl) */
    }
    #endif /*pwmStepper_2_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!pwmStepper_2_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: pwmStepper_2_WriteCounter
    ********************************************************************************
    *
    * Summary:
    *  Writes a new counter value directly to the counter register. This will be
    *  implemented for that currently running period and only that period. This API
    *  is valid only for UDB implementation and not available for fixed function
    *  PWM implementation.
    *
    * Parameters:
    *  counter:  The period new period counter value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  The PWM Period will be reloaded when a counter value will be a zero
    *
    *******************************************************************************/
    void pwmStepper_2_WriteCounter(uint16 counter) \
                                       
    {
        CY_SET_REG16(pwmStepper_2_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadCounter
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current value of the counter.  It doesn't matter
    *  if the counter is enabled or running.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  The current value of the counter.
    *
    *******************************************************************************/
    uint16 pwmStepper_2_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(pwmStepper_2_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG16(pwmStepper_2_CAPTURE_LSB_PTR));
    }

    #if (pwmStepper_2_UseStatus)


        /*******************************************************************************
        * Function Name: pwmStepper_2_ClearFIFO
        ********************************************************************************
        *
        * Summary:
        *  This function clears all capture data from the capture FIFO
        *
        * Parameters:
        *  None
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void pwmStepper_2_ClearFIFO(void) 
        {
            while(0u != (pwmStepper_2_ReadStatusRegister() & pwmStepper_2_STATUS_FIFONEMPTY))
            {
                (void)pwmStepper_2_ReadCapture();
            }
        }

    #endif /* pwmStepper_2_UseStatus */

#endif /* !pwmStepper_2_UsingFixedFunction */


/*******************************************************************************
* Function Name: pwmStepper_2_WritePeriod
********************************************************************************
*
* Summary:
*  This function is used to change the period of the counter.  The new period
*  will be loaded the next time terminal count is detected.
*
* Parameters:
*  period:  Period value. May be between 1 and (2^Resolution)-1.  A value of 0
*           will result in the counter remaining at zero.
*
* Return:
*  None
*
*******************************************************************************/
void pwmStepper_2_WritePeriod(uint16 period) 
{
    #if(pwmStepper_2_UsingFixedFunction)
        CY_SET_REG16(pwmStepper_2_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG16(pwmStepper_2_PERIOD_LSB_PTR, period);
    #endif /* (pwmStepper_2_UsingFixedFunction) */
}

#if (pwmStepper_2_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: pwmStepper_2_WriteCompare
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value when the PWM is in Dither
    *  mode. The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  compared to the compare value based on the compare mode defined in
    *  Dither Mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  This function is only available if the PWM mode parameter is set to
    *  Dither Mode, Center Aligned Mode or One Output Mode
    *
    *******************************************************************************/
    void pwmStepper_2_WriteCompare(uint16 compare) \
                                       
    {
        #if(pwmStepper_2_UsingFixedFunction)
            CY_SET_REG16(pwmStepper_2_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG16(pwmStepper_2_COMPARE1_LSB_PTR, compare);
        #endif /* (pwmStepper_2_UsingFixedFunction) */

        #if (pwmStepper_2_PWMMode == pwmStepper_2__B_PWM__DITHER)
            #if(pwmStepper_2_UsingFixedFunction)
                CY_SET_REG16(pwmStepper_2_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG16(pwmStepper_2_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (pwmStepper_2_UsingFixedFunction) */
        #endif /* (pwmStepper_2_PWMMode == pwmStepper_2__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: pwmStepper_2_WriteCompare1
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value.  The compare output will
    *  reflect the new value on the next UDB clock.  The compare output will be
    *  driven high when the present counter value is less than or less than or
    *  equal to the compare register, depending on the mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmStepper_2_WriteCompare1(uint16 compare) \
                                        
    {
        #if(pwmStepper_2_UsingFixedFunction)
            CY_SET_REG16(pwmStepper_2_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG16(pwmStepper_2_COMPARE1_LSB_PTR, compare);
        #endif /* (pwmStepper_2_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: pwmStepper_2_WriteCompare2
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare value, for compare1 output.
    *  The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  less than or less than or equal to the compare register, depending on the
    *  mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmStepper_2_WriteCompare2(uint16 compare) \
                                        
    {
        #if(pwmStepper_2_UsingFixedFunction)
            CY_SET_REG16(pwmStepper_2_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG16(pwmStepper_2_COMPARE2_LSB_PTR, compare);
        #endif /* (pwmStepper_2_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (pwmStepper_2_DeadBandUsed)


    /*******************************************************************************
    * Function Name: pwmStepper_2_WriteDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function writes the dead-band counts to the corresponding register
    *
    * Parameters:
    *  deadtime:  Number of counts for dead time
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmStepper_2_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!pwmStepper_2_DeadBand2_4)
            CY_SET_REG8(pwmStepper_2_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            pwmStepper_2_DEADBAND_COUNT &= ((uint8)(~pwmStepper_2_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(pwmStepper_2_DEADBAND_COUNT_SHIFT)
                pwmStepper_2_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << pwmStepper_2_DEADBAND_COUNT_SHIFT)) &
                                                    pwmStepper_2_DEADBAND_COUNT_MASK;
            #else
                pwmStepper_2_DEADBAND_COUNT |= deadtime & pwmStepper_2_DEADBAND_COUNT_MASK;
            #endif /* (pwmStepper_2_DEADBAND_COUNT_SHIFT) */

        #endif /* (!pwmStepper_2_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function reads the dead-band counts from the corresponding register
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Dead Band Counts
    *
    *******************************************************************************/
    uint8 pwmStepper_2_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!pwmStepper_2_DeadBand2_4)
            return (CY_GET_REG8(pwmStepper_2_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(pwmStepper_2_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(pwmStepper_2_DEADBAND_COUNT & pwmStepper_2_DEADBAND_COUNT_MASK)) >>
                                                                           pwmStepper_2_DEADBAND_COUNT_SHIFT));
            #else
                return (pwmStepper_2_DEADBAND_COUNT & pwmStepper_2_DEADBAND_COUNT_MASK);
            #endif /* (pwmStepper_2_DEADBAND_COUNT_SHIFT) */
        #endif /* (!pwmStepper_2_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (pwmStepper_2_UseStatus || pwmStepper_2_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: pwmStepper_2_SetInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  This function configures the interrupts mask control of theinterrupt
    *  source status register.
    *
    * Parameters:
    *  uint8 interruptMode: Bit field containing the interrupt sources enabled
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmStepper_2_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(pwmStepper_2_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadStatusRegister
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current state of the status register.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current status register value. The status register bits are:
    *  [7:6] : Unused(0)
    *  [5]   : Kill event output
    *  [4]   : FIFO not empty
    *  [3]   : FIFO full
    *  [2]   : Terminal count
    *  [1]   : Compare output 2
    *  [0]   : Compare output 1
    *
    *******************************************************************************/
    uint8 pwmStepper_2_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(pwmStepper_2_STATUS_PTR));
    }

#endif /* (pwmStepper_2_UseStatus || pwmStepper_2_UsingFixedFunction) */


#if (pwmStepper_2_UseControl)


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the control register. This API is available
    *  only if the control register is not removed.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current control register value
    *
    *******************************************************************************/
    uint8 pwmStepper_2_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(pwmStepper_2_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: pwmStepper_2_WriteControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Sets the bit field of the control register. This API is available only if
    *  the control register is not removed.
    *
    * Parameters:
    *  uint8 control: Control register bit field, The status register bits are:
    *  [7]   : PWM Enable
    *  [6]   : Reset
    *  [5:3] : Compare Mode2
    *  [2:0] : Compare Mode2
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmStepper_2_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(pwmStepper_2_CONTROL_PTR, control);
    }

#endif /* (pwmStepper_2_UseControl) */


#if (!pwmStepper_2_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadCapture
    ********************************************************************************
    *
    * Summary:
    *  Reads the capture value from the capture FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: The current capture value
    *
    *******************************************************************************/
    uint16 pwmStepper_2_ReadCapture(void) 
    {
        return (CY_GET_REG16(pwmStepper_2_CAPTURE_LSB_PTR));
    }

#endif /* (!pwmStepper_2_UsingFixedFunction) */


#if (pwmStepper_2_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadCompare
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare output when the PWM Mode parameter is
    *  set to Dither mode, Center Aligned mode, or One Output mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value
    *
    *******************************************************************************/
    uint16 pwmStepper_2_ReadCompare(void) 
    {
        #if(pwmStepper_2_UsingFixedFunction)
            return ((uint16)CY_GET_REG16(pwmStepper_2_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG16(pwmStepper_2_COMPARE1_LSB_PTR));
        #endif /* (pwmStepper_2_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadCompare1
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare1 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint16 pwmStepper_2_ReadCompare1(void) 
    {
        return (CY_GET_REG16(pwmStepper_2_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadCompare2
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare2 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint16 pwmStepper_2_ReadCompare2(void) 
    {
        return (CY_GET_REG16(pwmStepper_2_COMPARE2_LSB_PTR));
    }

#endif /* (pwmStepper_2_UseOneCompareMode) */


/*******************************************************************************
* Function Name: pwmStepper_2_ReadPeriod
********************************************************************************
*
* Summary:
*  Reads the period value used by the PWM hardware.
*
* Parameters:
*  None
*
* Return:
*  uint8/16: Period value
*
*******************************************************************************/
uint16 pwmStepper_2_ReadPeriod(void) 
{
    #if(pwmStepper_2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(pwmStepper_2_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(pwmStepper_2_PERIOD_LSB_PTR));
    #endif /* (pwmStepper_2_UsingFixedFunction) */
}

#if ( pwmStepper_2_KillModeMinTime)


    /*******************************************************************************
    * Function Name: pwmStepper_2_WriteKillTime
    ********************************************************************************
    *
    * Summary:
    *  Writes the kill time value used by the hardware when the Kill Mode
    *  is set to Minimum Time.
    *
    * Parameters:
    *  uint8: Minimum Time kill counts
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmStepper_2_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(pwmStepper_2_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: pwmStepper_2_ReadKillTime
    ********************************************************************************
    *
    * Summary:
    *  Reads the kill time value used by the hardware when the Kill Mode is set
    *  to Minimum Time.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8: The current Minimum Time kill counts
    *
    *******************************************************************************/
    uint8 pwmStepper_2_ReadKillTime(void) 
    {
        return (CY_GET_REG8(pwmStepper_2_KILLMODEMINTIME_PTR));
    }

#endif /* ( pwmStepper_2_KillModeMinTime) */

/* [] END OF FILE */
