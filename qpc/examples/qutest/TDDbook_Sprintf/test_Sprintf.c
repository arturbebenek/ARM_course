/*****************************************************************************
* Purpose: example 'sprintf' QUTEST fixture
* Last Updated for Version: 6.3.1
* Date of the Last Update:  2018-05-21
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2005-2018 Quantum Leaps, LLC. All rights reserved.
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
* https://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpc.h"    /* QUTest interface */

#include <stdio.h>  /* CUT interface */

Q_DEFINE_THIS_FILE

/*--------------------------------------------------------------------------*/
static char format[20];
static char output[100];
static char string[20];

enum {
   SPRINTF_CALL = QS_USER,
};

/*--------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {

    QF_init();  /* initialize the framework */

    /* initialize the QS software tracing */
    Q_ALLEGE(QS_INIT(argc > 1 ? argv[1] : (void *)0));

    /* dictionaries... */
    QS_OBJ_DICTIONARY(format);
    QS_OBJ_DICTIONARY(output);
    QS_OBJ_DICTIONARY(string);
    QS_USR_DICTIONARY(SPRINTF_CALL);

    return QF_run(); /* run the tests */
}

/*--------------------------------------------------------------------------*/
void QS_onTestSetup(void) {
}
/*..........................................................................*/
void QS_onTestTeardown(void) {
}

/*..........................................................................*/
void QS_onCommand(uint8_t cmdId,
                  uint32_t param1, uint32_t param2, uint32_t param3)
{
    switch (cmdId) {
        case 0: {   /* call the CUT and report results */
            int ret = sprintf(&output[8],
                             format, param1, string, param2, param3);
            QS_BEGIN(SPRINTF_CALL, (void *)0) /* user-specific record */
                QS_U8(0, (uint8_t)ret);  /* return value */
                QS_STR(&output[8]);      /* produced string */
            QS_END()
            break;
        }
        default:
            break;
    }
}
/*..........................................................................*/
/*! callback function to "massage" the injected QP events (not used here) */
void QS_onTestEvt(QEvt *e) {
    (void)e;
}
