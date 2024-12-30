
#include "..//tm4c123gh6pm_11.h"
#include "Nokia5110.h"
#include "TExaS.h"
#include "timer.h"
unsigned long TimerCount;
unsigned long Semaphore;
int flag = 1, j = -1, cn = 1, i = 0;


void Timer2_Init(unsigned long period) { 
    unsigned long volatile delay;
    SYSCTL_RCGCTIMER_R |= 0x04;   // Activate timer2
    delay = SYSCTL_RCGCTIMER_R;
    TimerCount = 0;
    Semaphore = 0;
    TIMER2_CTL_R = 0x00000000;    // Disable timer2A during setup
    TIMER2_CFG_R = 0x00000000;    // Configure for 32-bit mode
    TIMER2_TAMR_R = 0x00000002;   // Configure for periodic mode
    TIMER2_TAILR_R = period - 1;  // Reload value
    TIMER2_TAPR_R = 0;            // Bus clock resolution
    TIMER2_ICR_R = 0x00000001;    // Clear timer2A timeout flag
    TIMER2_IMR_R = 0x00000001;    // Arm timeout interrupt
    NVIC_PRI5_R = (NVIC_PRI5_R & 0x00FFFFFF) | 0x80000000; // Set priority
    NVIC_EN0_R = 1 << 23;         // Enable IRQ 23 in NVIC
    TIMER2_CTL_R = 0x00000001;    // Enable timer2A
}
// Timer2A interrupt handler

// Timer2A interrupt handler
void Timer2A_Handler(void) { 
    TIMER2_ICR_R = 0x00000001;   // Acknowledge timer2A timeout
    Nokia5110_ClearBuffer();

    if (flag) {
        j++;
        if (j == 3) {
            flag = 0;
        }
    } else {
        j--;
        if (j == 0) {
            flag = 1;
            cn++;
        }
    }

    // Update the display with enemy positions
    Nokia5110_PrintBMP(0, ENEMY10H - 1 + j * 15, SmallEnemy10PointA, 0);
    Nokia5110_PrintBMP(16, ENEMY10H - 1 + j * 15, SmallEnemy20PointA, 0);
    Nokia5110_PrintBMP(32, ENEMY10H - 1 + j * 15, SmallEnemy20PointA, 0);
    Nokia5110_PrintBMP(48, ENEMY10H - 1 + j * 15, SmallEnemy30PointA, 0);
    Nokia5110_PrintBMP(64, ENEMY10H - 1 + j * 15, SmallEnemy30PointA, 0);

    Nokia5110_DisplayBuffer();  // Update the screen

    // Check for game over
    if (cn == 6) { 
        Nokia5110_Clear();
        Nokia5110_SetCursor(1, 1);
        Nokia5110_OutString("GAME OVER");
        Nokia5110_SetCursor(1, 2);
        Nokia5110_OutString("Nice try,");
        Nokia5110_SetCursor(1, 3);
        Nokia5110_OutString("Earthling!");
        Nokia5110_SetCursor(2, 4);
        Nokia5110_OutUDec(1234);  // Display score (1234 as a placeholder)

        TIMER2_CTL_R = 0x00000000;  // Disable timer2
    }
}