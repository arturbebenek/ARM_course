/**
* @file
* @brief QF/C port to POSIX API with cooperative QV scheduler (posix-qv)
* @ingroup ports
* @cond
******************************************************************************
* Last updated for version 6.3.2
* Last updated on  2018-06-18
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
******************************************************************************
* @endcond
*/
#define QP_IMPL           /* this is QP implementation */
#include "qf_port.h"      /* QF port */
#include "qf_pkg.h"
#include "qassert.h"
#ifdef Q_SPY              /* QS software tracing enabled? */
    #include "qs_port.h"  /* include QS port */
#else
    #include "qs_dummy.h" /* disable the QS software tracing */
#endif /* Q_SPY */

#include <limits.h>       /* for PTHREAD_STACK_MIN */
#include <sys/mman.h>     /* for mlockall() */

Q_DEFINE_THIS_MODULE("qf_port")

/* Global objects ==========================================================*/
pthread_mutex_t QF_pThreadMutex_;
QPSet  QV_readySet_;        /* QV-ready set of active objects */
pthread_cond_t QV_condVar_; /* Cond.var. to signal events */

/* Local objects ===========================================================*/
static bool l_isRunning;
static struct timespec l_tick;
static int_t l_tickPrio;
enum { NANOSLEEP_NSEC_PER_SEC = 1000000000 }; /* see NOTE05 */

static void *ticker_thread(void *arg);

/* QF functions ============================================================*/
void QF_init(void) {
    extern uint_fast8_t QF_maxPool_;
    extern QTimeEvt QF_timeEvtHead_[QF_MAX_TICK_RATE];

    /* lock memory so we're never swapped out to disk */
    /*mlockall(MCL_CURRENT | MCL_FUTURE);  uncomment when supported */

    /* init the global mutex with the default non-recursive initializer */
    pthread_mutex_init(&QF_pThreadMutex_, NULL);

    /* clear the internal QF variables, so that the framework can (re)start
    * correctly even if the startup code is not called to clear the
    * uninitialized data (as is required by the C Standard).
    */
    QF_maxPool_ = (uint_fast8_t)0;
    QF_bzero(&QF_timeEvtHead_[0], (uint_fast16_t)sizeof(QF_timeEvtHead_));
    QF_bzero(&QF_active_[0],      (uint_fast16_t)sizeof(QF_active_));

    l_tick.tv_sec = 0;
    l_tick.tv_nsec = NANOSLEEP_NSEC_PER_SEC/100L; /* default clock tick */
    l_tickPrio = sched_get_priority_min(SCHED_FIFO); /* default tick prio */
}
/****************************************************************************/

int_t QF_run(void) {
    struct sched_param sparam;

    QF_onStartup();  /* invoke startup callback */

    /* try to set the priority of the ticker thread, see NOTE01 */
    sparam.sched_priority = l_tickPrio;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sparam) == 0) {
        /* success, this application has sufficient privileges */
    }
    else {
        /* setting priority failed, probably due to insufficient privieges */
    }

    l_isRunning = true; /* QF is running */

    /* system clock tick configured? */
    if ((l_tick.tv_sec != 0) || (l_tick.tv_nsec != 0)) {
        pthread_attr_t attr;
        struct sched_param param;
        pthread_t idle;

        /* SCHED_FIFO corresponds to real-time preemptive priority-based
        * scheduler.
        * NOTE: This scheduling policy requires the superuser priviledges
        */
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        param.sched_priority = sched_get_priority_min(SCHED_FIFO);

        pthread_attr_setschedparam(&attr, &param);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (pthread_create(&idle, &attr, &ticker_thread, 0) != 0) {
            /* Creating the p-thread with the SCHED_FIFO policy failed.
            * Most probably this application has no superuser privileges,
            * so we just fall back to the default SCHED_OTHER policy
            * and priority 0.
            */
            pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
            param.sched_priority = 0;
            pthread_attr_setschedparam(&attr, &param);
            if (pthread_create(&idle, &attr, &ticker_thread, 0) == 0) {
                return false;
            }
        }
        pthread_attr_destroy(&attr);
    }

    /* the combined event-loop and background-loop of the QV kernel */
    QF_INT_DISABLE();
    while (l_isRunning) {
        QEvt const *e;
        QActive *a;
        uint_fast8_t p;

        /* find the maximum priority AO ready to run */
        if (QPSet_notEmpty(&QV_readySet_)) {

            QPSet_findMax(&QV_readySet_, p);
            a = QF_active_[p];
            QF_INT_ENABLE();

            /* the active object 'a' must still be registered in QF
            * (e.g., it must not be stopped)
            */
            Q_ASSERT_ID(320, a != (QActive *)0);

            /* perform the run-to-completion (RTS) step...
            * 1. retrieve the event from the AO's event queue, which by this
            *    time must be non-empty and The "Vanialla" kernel asserts it.
            * 2. dispatch the event to the AO's state machine.
            * 3. determine if event is garbage and collect it if so
            */
            e = QActive_get_(a);
            QHSM_DISPATCH(&a->super, e);
            QF_gc(e);

            QF_INT_DISABLE();

            if (a->eQueue.frontEvt == (QEvt const *)0) { /* empty queue? */
                QPSet_remove(&QV_readySet_, p);
            }
        }
        else {
            /* the QV kernel in embedded systems calls here the QV_onIdle()
            * callback. However, the POSIX-QV port does not do busy-waiting
            * for events. Instead, the POSIX-QV port efficiently waits until
            * QP events become available.
            */
            while (QPSet_isEmpty(&QV_readySet_)) {
                pthread_cond_wait(&QV_condVar_, &QF_pThreadMutex_);
            }
        }
    }
    QF_INT_ENABLE();
    QF_onCleanup();  /* cleanup callback */
    QS_EXIT();       /* cleanup the QSPY connection */

    pthread_cond_destroy(&QV_condVar_);       // cleanup the condition variable
    pthread_mutex_destroy(&QF_pThreadMutex_); // cleanup the global mutex

    return (int_t)0; /* return success */
}
/*..........................................................................*/
void QF_setTickRate(uint32_t ticksPerSec, int_t tickPrio) {
    if (ticksPerSec != (uint32_t)0) {
        l_tick.tv_nsec = NANOSLEEP_NSEC_PER_SEC / ticksPerSec;
    }
    else {
        l_tick.tv_nsec = 0; /* means NO system clock tick */
    }
    l_tickPrio = tickPrio;
}
/*..........................................................................*/
void QF_stop(void) {
    uint_fast8_t p;
    l_isRunning = false; /* terminate the main event-loop thread */

    /* unblock the event-loop so it can terminate */
    p = (uint_fast8_t)1;
    QPSet_insert(&QV_readySet_, p);
    pthread_cond_signal(&QV_condVar_);
}
/*..........................................................................*/
void QActive_start_(QActive * const me, uint_fast8_t prio,
                    QEvt const *qSto[], uint_fast16_t qLen,
                    void *stkSto, uint_fast16_t stkSize,
                    QEvt const *ie)
{
    Q_REQUIRE_ID(600, ((uint_fast8_t)0 < prio) /* priority...*/
        && (prio <= (uint_fast8_t)QF_MAX_ACTIVE) /*... in range */
        && (stkSto == (void *)0));    /* statck storage must NOT...
                                       * ... be provided */

    QEQueue_init(&me->eQueue, qSto, qLen);
    me->prio = (uint8_t)prio;
    QF_add_(me); /* make QF aware of this active object */

    QHSM_INIT(&me->super, ie); /* take the top-most initial tran. */

    (void)stkSize; /* avoid the "unused parameter" compiler warning */
}
/*..........................................................................*/
void QActive_stop(QActive * const me) {
    QActive_unsubscribeAll(me);
    QF_remove_(me);
}

//****************************************************************************
static void *ticker_thread(void *arg) { /* for pthread_create() */
    (void)arg; /* unused parameter */
    while (l_isRunning) { /* the clock tick loop... */
        QF_onClockTick(); /* clock tick callback (must call QF_TICK_X()) */

        nanosleep(&l_tick, NULL); /* sleep for the number of ticks, NOTE05 */
    }
    return (void *)0; /* return success */
}

/*****************************************************************************
* NOTE01:
* In Linux, the scheduler policy closest to real-time is the SCHED_FIFO
* policy, available only with superuser privileges. QF_run() attempts to set
* this policy as well as to maximize its priority, so that the ticking
* occurrs in the most timely manner (as close to an interrupt as possible).
* However, setting the SCHED_FIFO policy might fail, most probably due to
* insufficient privileges.
*
* NOTE02:
* On some Linux systems nanosleep() might actually not deliver the finest
* time granularity. For example, on some Linux implementations, nanosleep()
* could not block for shorter intervals than 20ms, while the underlying
* clock tick period was only 10ms. Sometimes, the select() system call can
* provide a finer granularity.
*
* NOTE03:
* Any blocking system call, such as nanosleep() or select() system call can
* be interrupted by a signal, such as ^C from the keyboard. In this case this
* QF port breaks out of the event-loop and returns to main() that exits and
* terminates all spawned p-threads.
*
* NOTE04:
* According to the man pages (for pthread_attr_setschedpolicy) the only value
* supported in the Linux p-threads implementation is PTHREAD_SCOPE_SYSTEM,
* meaning that the threads contend for CPU time with all processes running on
* the machine. In particular, thread priorities are interpreted relative to
* the priorities of all other processes on the machine.
*
* This is good, because it seems that if we set the priorities high enough,
* no other process (or thread running within) can gain control over the CPU.
*
* However, QF limits the number of priority levels to QF_MAX_ACTIVE.
* Assuming that a QF application will be real-time, this port reserves the
* three highest Linux priorities for the ISR-like threads (e.g., the ticker,
* I/O), and the rest highest-priorities for the active objects.
*
* NOTE05:
* In some (older) Linux kernels, the POSIX nanosleep() system call might
* deliver only 2*actual-system-tick granularity. To compensate for this,
* you would need to reduce (by 2) the constant NANOSLEEP_NSEC_PER_SEC.
*/

