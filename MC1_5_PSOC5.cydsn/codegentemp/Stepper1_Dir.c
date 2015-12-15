/*******************************************************************************
* File Name: Stepper1_Dir.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Stepper1_Dir.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 Stepper1_Dir__PORT == 15 && ((Stepper1_Dir__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: Stepper1_Dir_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None
*  
*******************************************************************************/
void Stepper1_Dir_Write(uint8 value) 
{
    uint8 staticBits = (Stepper1_Dir_DR & (uint8)(~Stepper1_Dir_MASK));
    Stepper1_Dir_DR = staticBits | ((uint8)(value << Stepper1_Dir_SHIFT) & Stepper1_Dir_MASK);
}


/*******************************************************************************
* Function Name: Stepper1_Dir_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  Stepper1_Dir_DM_STRONG     Strong Drive 
*  Stepper1_Dir_DM_OD_HI      Open Drain, Drives High 
*  Stepper1_Dir_DM_OD_LO      Open Drain, Drives Low 
*  Stepper1_Dir_DM_RES_UP     Resistive Pull Up 
*  Stepper1_Dir_DM_RES_DWN    Resistive Pull Down 
*  Stepper1_Dir_DM_RES_UPDWN  Resistive Pull Up/Down 
*  Stepper1_Dir_DM_DIG_HIZ    High Impedance Digital 
*  Stepper1_Dir_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void Stepper1_Dir_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(Stepper1_Dir_0, mode);
}


/*******************************************************************************
* Function Name: Stepper1_Dir_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro Stepper1_Dir_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 Stepper1_Dir_Read(void) 
{
    return (Stepper1_Dir_PS & Stepper1_Dir_MASK) >> Stepper1_Dir_SHIFT;
}


/*******************************************************************************
* Function Name: Stepper1_Dir_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 Stepper1_Dir_ReadDataReg(void) 
{
    return (Stepper1_Dir_DR & Stepper1_Dir_MASK) >> Stepper1_Dir_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(Stepper1_Dir_INTSTAT) 

    /*******************************************************************************
    * Function Name: Stepper1_Dir_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 Stepper1_Dir_ClearInterrupt(void) 
    {
        return (Stepper1_Dir_INTSTAT & Stepper1_Dir_MASK) >> Stepper1_Dir_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
