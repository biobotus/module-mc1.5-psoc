/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "MagnetisationController.h"

extern MC1_5Module  mc1_5Module[2];

uint8 updateMagnetisation(uint8 module)
{
    if(mc1_5Module[module].magnetisationState == magnetisationDisabled && mc1_5Module[module].stepperPosition > 0)
    {
        mc1_5Module[module].stepperPosition--;
    }
    else if(mc1_5Module[module].magnetisationState == magnetisationDisabled && mc1_5Module[module].stepperPosition <= 0)
    {
        Stop(module);
    }
    else if(mc1_5Module[module].magnetisationState == magnetisationEnabled && mc1_5Module[module].stepperPosition < STEP_FOR_MAGN_ON)
    {
        mc1_5Module[module].stepperPosition++;
    }
    else if(mc1_5Module[module].magnetisationState == magnetisationEnabled && mc1_5Module[module].stepperPosition >= STEP_FOR_MAGN_ON)
    {
        Stop(module);
    }
    
    if (mc1_5Module[module].stepperPosition%50 == 0)
    {
        saveStepperPositionToEEPROM(module, mc1_5Module[module].stepperPosition);        
    }    
    
    return 1;
}

uint8 changeMagnState(uint8 desiredMagnState ,uint8 module)
{
    if (module == 0 || module ==1)
    {
        if (mc1_5Module[module].magnetisationState == magnetisationEnabled && desiredMagnState == magnetisationDisabled)
        {
            Move(stepperFall,module); 
        }
        else if (mc1_5Module[module].magnetisationState == magnetisationDisabled && desiredMagnState == magnetisationEnabled)
        {
            Move(stepperRise,module);         
        } 
        mc1_5Module[module].magnetisationState = desiredMagnState;  
    }       

    return 1;
}
uint8 initializeMagn()
{
    Set_StepSize();
}

/* [] END OF FILE */
