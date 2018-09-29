/*****************************************************************************
* Product: QMsmTst Example, Linux
* Last updated for version 5.8.0
* Last updated on  2016-11-29
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
#include "qmsmtst.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

Q_DEFINE_THIS_FILE

/* local objects -----------------------------------------------------------*/
static FILE *l_outFile = (FILE *)0;
static struct termios l_oldt;
static void dispatch(QSignal sig);

/*..........................................................................*/
int main(int argc, char *argv[]) {
    QMsmTst_ctor();   /* instantiate the QMsmTst object */

    if (argc > 1) {   /* file name provided? */
        l_outFile = fopen(argv[1], "w");
    }

    if (l_outFile == (FILE *)0) { /* interactive version? */
        struct termios newt;
        tcgetattr(STDIN_FILENO, &l_oldt); /* save the terminal state */
        newt = l_oldt;
        newt.c_lflag &= ~ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); /* set non-canonical mode */

        l_outFile = stdout; /* use the stdout as the output file */

        printf("QMsmTst example, built on %s at %s,\n"
               "QP: %s.\nPress ESC to quit...\n",
               __DATE__, __TIME__, QP_VERSION_STR);

        QHSM_INIT(the_msm, (QEvt *)0); /* the top-most initial tran. */

        for (;;) { /* event loop */
            QEvt e;
            int c;

            printf("\n>");
            c = getchar(); /* get a character from stdin */
            printf(": ");

            if ('a' <= c && c <= 'i') { /* in range? */
                e.sig = (QSignal)(c - 'a' + A_SIG);
            }
            else if ('A' <= c && c <= 'I') { /* in range? */
                e.sig = (QSignal)(c - 'A' + A_SIG);
            }
            else if (c == '\33') { /* the ESC key? */
                e.sig = TERMINATE_SIG; /* terminate the interactive test */
            }
            else {
                e.sig = IGNORE_SIG;
            }

            QHSM_DISPATCH(the_msm, &e); /* dispatch the event */
        }
    }
    else { /* batch version */
        printf("QMsmTst example, built on %s at %s, QP %s\n"
               "output saved to %s\n",
               __DATE__, __TIME__, QP_VERSION_STR,
               argv[1]);

        fprintf(l_outFile, "QMsmTst example, QEP %s\n",
                QP_VERSION_STR);

        QHSM_INIT(the_msm, (QEvt *)0); /* the top-most initial tran. */

        /* testing of dynamic transitions... */
        dispatch(A_SIG);
        dispatch(B_SIG);
        dispatch(D_SIG);
        dispatch(E_SIG);
        dispatch(I_SIG);
        dispatch(F_SIG);
        dispatch(I_SIG);
        dispatch(I_SIG);
        dispatch(F_SIG);
        dispatch(A_SIG);
        dispatch(B_SIG);
        dispatch(D_SIG);
        dispatch(D_SIG);
        dispatch(E_SIG);
        dispatch(G_SIG);
        dispatch(H_SIG);
        dispatch(H_SIG);
        dispatch(C_SIG);
        dispatch(G_SIG);
        dispatch(C_SIG);
        dispatch(C_SIG);

        fclose(l_outFile);
    }

    return 0;
}
/*..........................................................................*/
void Q_onAssert(char const * const file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    exit(-1);
}
/*..........................................................................*/
void BSP_display(char const *msg) {
    fprintf(l_outFile, msg);
    fflush(l_outFile);
}
/*..........................................................................*/
void BSP_exit(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &l_oldt);
    printf("Bye, Bye!\n");
    exit(0);
}
/*..........................................................................*/
static void dispatch(QSignal sig) {
    QEvt e;
    Q_REQUIRE((A_SIG <= sig) && (sig <= I_SIG));
    e.sig = sig;
    fprintf(l_outFile, "\n%c:", 'A' + sig - A_SIG);
    QHSM_DISPATCH(the_msm, &e); /* dispatch the event */
}

/*--------------------------------------------------------------------------*/
#ifdef Q_SPY
#include "qs_port.h"

/*..........................................................................*/
void QF_onStartup(void) {
}
/*..........................................................................*/
void QF_onClockTick(void) {
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QS_onCleanup(void) {
}
/*..........................................................................*/
QSTimeCtr QS_onGetTime(void) {
    return (QSTimeCtr)clock();
}
#endif

