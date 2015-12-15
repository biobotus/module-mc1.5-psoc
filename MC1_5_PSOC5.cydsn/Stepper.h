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

#include <project.h>
#include <CAN_Communication.h>
#include <Defines.h>




void Move(StepperDirType direction, uint8 module);
void Stop(uint8 module);
void Pipetting(uint8);
uint16 Set_Step(int);
void Set_DIR(StepperDirType direction, uint8 module);
void Set_MS_pinout();
void Enable_Allegro(uint8 module);
void Disable_Allegro(uint8 module);
void Set_StepSize();

/* [] END OF FILE */
