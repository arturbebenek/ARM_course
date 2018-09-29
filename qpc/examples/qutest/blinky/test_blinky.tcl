# test-script for QUTest unit testing harness
# see https://www.state-machine.com/qtools/qutest.html

# preamble...
proc on_reset {} {
    expect_pause
    glb_filter ON
    continue
    expect "%timestamp TE0-Arm  Obj=l_blinky.timeEvt,AO=l_blinky,*"
    expect "===RTC===> St-Init  Obj=l_blinky,State=QHsm_top->Blinky_off"
    expect "%timestamp LED 0"
    expect "===RTC===> St-Entry Obj=l_blinky,State=Blinky_off"
    expect "%timestamp Init===> Obj=l_blinky,State=Blinky_off"
    current_obj SM_AO l_blinky
}

# tests...
test "TIMEOUT_SIG->l_blinky"
post TIMEOUT_SIG
expect "%timestamp QF-New   Sig=TIMEOUT_SIG,*"
expect "%timestamp MP-Get   Obj=smlPoolSto,*"
expect "%timestamp AO-Post  Sdr=QS_RX,Obj=l_blinky,Evt<Sig=TIMEOUT_SIG,*"
expect "%timestamp AO-GetL  Obj=l_blinky,Evt<Sig=TIMEOUT_SIG,*"
expect "%timestamp Disp===> Obj=l_blinky,Sig=TIMEOUT_SIG,State=Blinky_off"
expect "%timestamp LED 1"
expect "===RTC===> St-Entry Obj=l_blinky,State=Blinky_on"
expect "%timestamp ===>Tran Obj=l_blinky,Sig=TIMEOUT_SIG,State=Blinky_off->Blinky_on"
expect "%timestamp QF-gc    Evt<Sig=TIMEOUT_SIG,*"
expect "%timestamp MP-Put   Obj=smlPoolSto,*"
expect "%timestamp Trg-Done QS_RX_EVENT"

test "timeEvt->Blinky_off (tick)"
current_obj TE l_blinky.timeEvt
tick
expect "%timestamp TE0-Post Obj=l_blinky.timeEvt,Sig=TIMEOUT_SIG,AO=l_blinky"
expect "%timestamp AO-Post  Sdr=QS_RX,Obj=l_blinky,Evt<Sig=TIMEOUT_SIG,*"
expect "%timestamp AO-GetL  Obj=l_blinky,Evt<Sig=TIMEOUT_SIG,*"
expect "%timestamp Disp===> Obj=l_blinky,Sig=TIMEOUT_SIG,State=Blinky_off"
expect "%timestamp LED 1"
expect "===RTC===> St-Entry Obj=l_blinky,State=Blinky_on"
expect "%timestamp ===>Tran Obj=l_blinky,Sig=TIMEOUT_SIG,State=Blinky_off->Blinky_on"
expect "%timestamp Trg-Done QS_RX_TICK"

test "timeEvt->Blinky_on (tick)" -noreset
tick
expect "%timestamp TE0-Post Obj=l_blinky.timeEvt,Sig=TIMEOUT_SIG,AO=l_blinky"
expect "%timestamp AO-Post  Sdr=QS_RX,Obj=l_blinky,Evt<Sig=TIMEOUT_SIG,*"
expect "%timestamp AO-GetL  Obj=l_blinky,Evt<Sig=TIMEOUT_SIG,*"
expect "%timestamp Disp===> Obj=l_blinky,Sig=TIMEOUT_SIG,State=Blinky_on"
expect "%timestamp LED 0"
expect "===RTC===> St-Entry Obj=l_blinky,State=Blinky_off"
expect "%timestamp ===>Tran Obj=l_blinky,Sig=TIMEOUT_SIG,State=Blinky_on->Blinky_off"
expect "%timestamp Trg-Done QS_RX_TICK"


# the end
end
