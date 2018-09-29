/**
* @file
* @brief QF/C stub for QUTEST unit testing
* @ingroup qs
* @cond
******************************************************************************
* Last updated for version 6.2.0
* Last updated on  2018-03-16
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
/* only build when Q_UTEST is defined */
#ifdef Q_UTEST

#define QP_IMPL           /* this is QP implementation */
#include "qf_port.h"      /* QF port */
#include "qf_pkg.h"       /* QF package-scope interface */
#include "qassert.h"      /* QP embedded systems-friendly assertions */
#include "qs_port.h"      /* include QS port */

Q_DEFINE_THIS_MODULE("qutest")

/* Global objects ==========================================================*/
uint8_t volatile QF_intNest;

/* QF functions ============================================================*/
void QF_init(void) {
    QF_maxPool_      = (uint_fast8_t)0;
    QF_subscrList_   = (QSubscrList *)0;
    QF_maxPubSignal_ = (enum_t)0;
    QF_intNest       = (uint8_t)0;

    QF_bzero(&QF_active_[0], (uint_fast16_t)sizeof(QF_active_));
    QF_bzero(&QS_rxPriv_.readySet, (uint_fast16_t)sizeof(QS_rxPriv_.readySet));
}
/*..........................................................................*/
void QF_stop(void) {
    QS_onReset();
}
/*..........................................................................*/
int_t QF_run(void) {
    /* function dictionaries for the standard API */
    QS_FUN_DICTIONARY(&QActive_post_);
    QS_FUN_DICTIONARY(&QActive_postLIFO_);

    QS_onTestLoop();   /* run the test loop */
    QS_onCleanup();    /* application cleanup */
    return (int_t)0;   /* return no error */
}

/*..........................................................................*/
void QActive_start_(QActive * const me, uint_fast8_t prio,
                    QEvt const *qSto[], uint_fast16_t qLen,
                    void *stkSto, uint_fast16_t stkSize,
                    QEvt const *ie)
{
    Q_REQUIRE_ID(500, ((uint_fast8_t)0 < prio) /* priority must be in range */
                 && (prio <= (uint_fast8_t)QF_MAX_ACTIVE));

    (void)stkSto;
    (void)stkSize;

    QEQueue_init(&me->eQueue, qSto, qLen); /* initialize the built-in queue */
    me->prio = (uint8_t)prio; /* set the current priority of the AO */
    QF_add_(me); /* make QF aware of this active object */

    QHSM_INIT(&me->super, ie); /* take the top-most initial tran. */
    QS_FLUSH();                /* flush the trace buffer to the host */
}
/*..........................................................................*/
void QActive_stop(QActive * const me) {
    QF_remove_(me); /* remove this active object from the framework */
}

/****************************************************************************/
QTimeEvtCtr QTimeEvt_ctr(QTimeEvt const * const me) {
    QTimeEvtCtr ret;
    QF_CRIT_STAT_

    QF_CRIT_ENTRY_();
    ret = me->ctr;
    QF_CRIT_EXIT_();

    return ret;
}
/*..........................................................................*/
void QTimeEvt_ctorX(QTimeEvt * const me, QActive * const act,
                    enum_t const sig, uint_fast8_t tickRate)
{
    /** @pre The signal must be valid and the tick rate in range */
    Q_REQUIRE_ID(300, (sig >= (enum_t)Q_USER_SIG)
                      && (tickRate < (uint_fast8_t)QF_MAX_TICK_RATE));

    me->next      = (QTimeEvt *)0;
    me->ctr       = (QTimeEvtCtr)0;
    me->interval  = (QTimeEvtCtr)0;
    me->super.sig = (QSignal)sig;

    me->act       = act;
    me->super.poolId_ = (uint8_t)0;
    me->super.refCtr_ = (uint8_t)tickRate;
}
/*..........................................................................*/
void QTimeEvt_armX(QTimeEvt * const me,
                   QTimeEvtCtr const nTicks, QTimeEvtCtr const interval)
{
    uint_fast8_t tickRate = (uint_fast8_t)me->super.refCtr_
                                & (uint_fast8_t)0x7F;
    QTimeEvtCtr ctr = me->ctr;
    QF_CRIT_STAT_

    /** @pre the host AO must be valid, time evnet must be disarmed,
    * number of clock ticks cannot be zero, and the signal must be valid.
    */
    Q_REQUIRE_ID(400, (me->act != (void *)0)
                      && (ctr == (QTimeEvtCtr)0)
                      && (nTicks != (QTimeEvtCtr)0)
                      && (tickRate < (uint_fast8_t)QF_MAX_TICK_RATE)
                      && (me->super.sig >= (QSignal)Q_USER_SIG));

    QF_CRIT_ENTRY_();
    me->ctr = nTicks;
    me->interval = interval;

    QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_ARM, QS_priv_.locFilter[TE_OBJ], me)
        QS_TIME_();                /* timestamp */
        QS_OBJ_(me);               /* this time event object */
        QS_OBJ_(me->act);          /* the active object */
        QS_TEC_(nTicks);           /* the number of ticks */
        QS_TEC_(interval);         /* the interval */
        QS_U8_((uint8_t)tickRate); /* tick rate */
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
}
/*..........................................................................*/
bool QTimeEvt_disarm(QTimeEvt * const me) {
    bool wasArmed;
    QF_CRIT_STAT_

    QF_CRIT_ENTRY_();

    /* is the time evt running? */
    if (me->ctr != (QTimeEvtCtr)0) {
        wasArmed = true;

        QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_DISARM, QS_priv_.locFilter[TE_OBJ], me)
            QS_TIME_();            /* timestamp */
            QS_OBJ_(me);           /* this time event object */
            QS_OBJ_(me->act);      /* the target AO */
            QS_TEC_(me->ctr);      /* the number of ticks */
            QS_TEC_(me->interval); /* the interval */
            QS_U8_((uint8_t)(me->super.refCtr_ & (uint8_t)0x7F));/*tick rate*/
        QS_END_NOCRIT_()

        me->ctr = (QTimeEvtCtr)0;  /* schedule removal from the list */
    }
    /* the time event was already not running */
    else {
        wasArmed = false;

        QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_DISARM_ATTEMPT,
                         QS_priv_.locFilter[TE_OBJ], me)
            QS_TIME_();            /* timestamp */
            QS_OBJ_(me);           /* this time event object */
            QS_OBJ_(me->act);      /* the target AO */
            QS_U8_((uint8_t)(me->super.refCtr_ & (uint8_t)0x7F));/*tick rate*/
        QS_END_NOCRIT_()

    }
    QF_CRIT_EXIT_();
    return wasArmed;
}
/*..........................................................................*/
bool QTimeEvt_rearm(QTimeEvt * const me, QTimeEvtCtr const nTicks) {
    uint_fast8_t tickRate = (uint_fast8_t)me->super.refCtr_
                            & (uint_fast8_t)0x7F;
    bool isArmed;
    QF_CRIT_STAT_

    /** @pre AO must be valid, tick rate must be in range, nTicks must
    * not be zero, and the signal of this time event must be valid
    */
    Q_REQUIRE_ID(600, (me->act != (void *)0)
                      && (tickRate < (uint_fast8_t)QF_MAX_TICK_RATE)
                      && (nTicks != (QTimeEvtCtr)0)
                      && (me->super.sig >= (QSignal)Q_USER_SIG));

    QF_CRIT_ENTRY_();

    /* is the time evt not running? */
    if (me->ctr == (QTimeEvtCtr)0) {
        isArmed = false;

        /* is the time event unlinked?
        * NOTE: For a duration of a single clock tick of the specified
        * tick rate a time event can be disarmed and yet still linked into
        * the list, because unlinking is performed exclusively in the
        * QF_tickX() function.
        */
        if ((me->super.refCtr_ & (uint8_t)0x80) == (uint8_t)0) {
            me->super.refCtr_ |= (uint8_t)0x80;  /* mark as linked */
        }
    }
    /* the time event is armed */
    else {
        isArmed = true;
    }
    me->ctr = nTicks; /* re-load the tick counter (shift the phasing) */

    QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_REARM, QS_priv_.locFilter[TE_OBJ], me)
        QS_TIME_();            /* timestamp */
        QS_OBJ_(me);           /* this time event object */
        QS_OBJ_(me->act);      /* the target AO */
        QS_TEC_(me->ctr);      /* the number of ticks */
        QS_TEC_(me->interval); /* the interval */
        QS_2U8_((uint8_t)tickRate,
                ((isArmed != false) ? (uint8_t)1 : (uint8_t)0));
    QS_END_NOCRIT_()

    QF_CRIT_EXIT_();
    return isArmed;
}
/*..........................................................................*/
void QF_tickX_(uint_fast8_t const tickRate, void const * const sender) {
    QF_CRIT_STAT_

    QF_CRIT_ENTRY_();
    if (QS_rxPriv_.currObj[TE_OBJ] != (void *)0) {
        QTimeEvt *t = (QTimeEvt *)QS_rxPriv_.currObj[TE_OBJ];
        QActive *act = (QActive *)t->act; /* temp. for volatile */
        if (t->interval == (QTimeEvtCtr)0) { /* single-shot TE? */
            t->ctr = (QTimeEvtCtr)0; /* auto-disarm */
            t->super.refCtr_ &= (uint8_t)0x7F; /* mark as unlinked */

            QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_AUTO_DISARM,
                             QS_priv_.locFilter[TE_OBJ], t)
                QS_OBJ_(t);            /* this time event object */
                QS_OBJ_(act);          /* the target AO */
                QS_U8_((uint8_t)tickRate); /* tick rate */
            QS_END_NOCRIT_()
        }

        QS_BEGIN_NOCRIT_(QS_QF_TIMEEVT_POST,
                         QS_priv_.locFilter[TE_OBJ], t)
            QS_TIME_();                /* timestamp */
            QS_OBJ_(t);                /* the time event object */
            QS_SIG_(t->super.sig);     /* signal of this time event */
            QS_OBJ_(act);              /* the target AO */
            QS_U8_((uint8_t)tickRate); /* tick rate */
        QS_END_NOCRIT_()

        QF_CRIT_EXIT_(); /* exit critical section before posting */

        QACTIVE_POST(act, &t->super, sender); /* asserts if queue overflows */
    }
    else {
        QF_CRIT_EXIT_();
    }
}

/*..........................................................................*/
void QS_processTestEvts_(void) {
    while (QPSet_notEmpty(&QS_rxPriv_.readySet)) {
        QEvt const *e;
        QActive *a;
        uint_fast8_t p;

        QPSet_findMax(&QS_rxPriv_.readySet, p);
        a = QF_active_[p];

        /* perform the run-to-completion (RTC) step...
        * 1. retrieve the event from the AO's event queue, which by this
        *    time must be non-empty and The "Vanialla" kernel asserts it.
        * 2. dispatch the event to the AO's state machine.
        * 3. determine if event is garbage and collect it if so
        */
        e = QActive_get_(a);
        QHSM_DISPATCH(&a->super, e);
        QF_gc(e);

        if (a->eQueue.frontEvt == (QEvt const *)0) { /* empty queue? */
            QPSet_remove(&QS_rxPriv_.readySet, p);
        }
    }
}

/****************************************************************************/
void Q_onAssert(char_t const * const module, int_t loc) {
    QS_BEGIN_NOCRIT_(QS_ASSERT_FAIL, (void *)0, (void *)0)
        QS_TIME_();
        QS_U16_((uint16_t)loc);
        QS_STR_((module != (char_t *)0) ? module : "?");
    QS_END_NOCRIT_()
    QS_onFlush(); /* flush the assertion record to the host */
    QS_onTestLoop(); /* loop to wait for commands (typically reset) */
    QS_onReset(); /* in case the QUTEST loop ever returns, reset manually */
}

#endif /* Q_UTEST */

