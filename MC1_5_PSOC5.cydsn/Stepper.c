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

#include "Stepper.h"

extern MC1_5Module  mc1_5Module [2];

void Move(StepperDirType direction, uint8 module)
{
    if(direction ==stepperFall || direction ==stepperRise)
    {
        Set_DIR(direction, module);
        Enable_Allegro(module);
    }
    switch (module)
    {
        case 0:
            pwmStepper_1_Start();
            break;
        case 1:
            pwmStepper_2_Start();
            break;
        default:
            break;
    }
}
void Stop(uint8 module)
{
    switch (module)
    {
        case 0:
            pwmStepper_1_Stop();
            break;
        case 1:
            pwmStepper_2_Stop();
            break;
        default:
            break;
    }
    Disable_Allegro(module);
    
}

void Enable_Allegro(uint8 module)
{
    switch (module)
    {
        case 0:
            Stepper1_Enable_Write(0);
            break;
        case 1:
            Stepper2_Enable_Write(0);
            break;
        default:
            break;
    }
}

void Disable_Allegro(uint8 module)
{
    switch (module)
    {
        case 0:
            Stepper1_Enable_Write(1);
            break;
        case 1:
            Stepper2_Enable_Write(1);
            break; 
        default:
            break;
    }
}


void Set_DIR(StepperDirType direction, uint8 module)
{
    switch (module)
    {
        case 0:
            Stepper1_Dir_Write(direction);
            break;
        case 1:
            Stepper2_Dir_Write(direction);
            break;
        default:
            break;
    }
}

void Set_StepSize()
{
    Stepper1_Step_SizeA1_Write(1);
    Stepper1_Step_SizeB1_Write(1);
    Stepper2_Step_SizeA2_Write(1);
    Stepper2_Step_SizeB2_Write(1);
}



/* [] END OF FILE */
