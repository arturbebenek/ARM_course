/*****************************************************************************
* Product: "Blinky" example, Win32 API
* Last updated for version 5.6.5
* Last updated on  2016-05-08
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* https://state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpc.h"
#include "bsp.h"

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

Q_DEFINE_THIS_FILE

#ifdef Q_SPY
    #error Simple Blinky Application does not provide Spy build configuration
#endif

/*..........................................................................*/
void BSP_init() {
    printf("Simple Blinky example\nQP version: %s\n"
           "Press ESC to quit...\n",
           QP_VERSION_STR);
}
/*..........................................................................*/
void BSP_ledOff(void) {
    printf("LED OFF\n");
}
/*..........................................................................*/
void BSP_ledOn(void) {
    printf("LED ON\n");
}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    QF_setTickRate(BSP_TICKS_PER_SEC); /* set the desired tick rate */
}
/*..........................................................................*/
void QF_onCleanup(void) {
    printf("\nBye! Bye!\n");
}
/*..........................................................................*/
void QF_onClockTick(void) {
    QF_TICK_X(0U, (void *)0);  /* perform the QF clock tick processing */
    if (_kbhit()) {            /* any key pressed? */
        switch (_getch()) {
            case '\033': {     /* ESC pressed? */
                QF_stop();
                break;
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
void Q_onAssert(char const * const file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    exit(-1);
}

