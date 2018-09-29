# test-script for QUTest unit testing harness
# see https://www.state-machine.com/qtools/qutest.html

# preamble...
proc on_reset {} {
    glb_filter UA
    current_obj SM the_hsm
}

# tests...
test "QHsmTst init"
init
expect "%timestamp BSP_DISPLAY top-INIT;"
expect "%timestamp BSP_DISPLAY s-ENTRY;"
expect "%timestamp BSP_DISPLAY s2-ENTRY;"
expect "%timestamp BSP_DISPLAY s2-INIT;"
expect "%timestamp BSP_DISPLAY s21-ENTRY;"
expect "%timestamp BSP_DISPLAY s211-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

#------------------
test "QHsmTst dispatch" -noreset

dispatch A_SIG
expect "%timestamp BSP_DISPLAY s21-A;"
expect "%timestamp BSP_DISPLAY s211-EXIT;"
expect "%timestamp BSP_DISPLAY s21-EXIT;"
expect "%timestamp BSP_DISPLAY s21-ENTRY;"
expect "%timestamp BSP_DISPLAY s21-INIT;"
expect "%timestamp BSP_DISPLAY s211-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch B_SIG
expect "%timestamp BSP_DISPLAY s21-B;"
expect "%timestamp BSP_DISPLAY s211-EXIT;"
expect "%timestamp BSP_DISPLAY s211-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch D_SIG
expect "%timestamp BSP_DISPLAY s211-D;"
expect "%timestamp BSP_DISPLAY s211-EXIT;"
expect "%timestamp BSP_DISPLAY s21-INIT;"
expect "%timestamp BSP_DISPLAY s211-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch E_SIG
expect "%timestamp BSP_DISPLAY s-E;"
expect "%timestamp BSP_DISPLAY s211-EXIT;"
expect "%timestamp BSP_DISPLAY s21-EXIT;"
expect "%timestamp BSP_DISPLAY s2-EXIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch I_SIG
expect "%timestamp BSP_DISPLAY s1-I;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch F_SIG
expect "%timestamp BSP_DISPLAY s1-F;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-EXIT;"
expect "%timestamp BSP_DISPLAY s2-ENTRY;"
expect "%timestamp BSP_DISPLAY s21-ENTRY;"
expect "%timestamp BSP_DISPLAY s211-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch I_SIG
expect "%timestamp BSP_DISPLAY s2-I;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch I_SIG
expect "%timestamp BSP_DISPLAY s-I;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch F_SIG
expect "%timestamp BSP_DISPLAY s2-F;"
expect "%timestamp BSP_DISPLAY s211-EXIT;"
expect "%timestamp BSP_DISPLAY s21-EXIT;"
expect "%timestamp BSP_DISPLAY s2-EXIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch A_SIG
expect "%timestamp BSP_DISPLAY s1-A;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-EXIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s1-INIT;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch B_SIG
expect "%timestamp BSP_DISPLAY s1-B;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch D_SIG
expect "%timestamp BSP_DISPLAY s1-D;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-EXIT;"
expect "%timestamp BSP_DISPLAY s-INIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch D_SIG
expect "%timestamp BSP_DISPLAY s11-D;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-INIT;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch E_SIG
expect "%timestamp BSP_DISPLAY s-E;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-EXIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch G_SIG
expect "%timestamp BSP_DISPLAY s11-G;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-EXIT;"
expect "%timestamp BSP_DISPLAY s2-ENTRY;"
expect "%timestamp BSP_DISPLAY s21-ENTRY;"
expect "%timestamp BSP_DISPLAY s211-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch H_SIG
expect "%timestamp BSP_DISPLAY s211-H;"
expect "%timestamp BSP_DISPLAY s211-EXIT;"
expect "%timestamp BSP_DISPLAY s21-EXIT;"
expect "%timestamp BSP_DISPLAY s2-EXIT;"
expect "%timestamp BSP_DISPLAY s-INIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch H_SIG
expect "%timestamp BSP_DISPLAY s11-H;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-EXIT;"
expect "%timestamp BSP_DISPLAY s-INIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch C_SIG
expect "%timestamp BSP_DISPLAY s1-C;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-EXIT;"
expect "%timestamp BSP_DISPLAY s2-ENTRY;"
expect "%timestamp BSP_DISPLAY s2-INIT;"
expect "%timestamp BSP_DISPLAY s21-ENTRY;"
expect "%timestamp BSP_DISPLAY s211-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch G_SIG
expect "%timestamp BSP_DISPLAY s21-G;"
expect "%timestamp BSP_DISPLAY s211-EXIT;"
expect "%timestamp BSP_DISPLAY s21-EXIT;"
expect "%timestamp BSP_DISPLAY s2-EXIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s1-INIT;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch C_SIG
expect "%timestamp BSP_DISPLAY s1-C;"
expect "%timestamp BSP_DISPLAY s11-EXIT;"
expect "%timestamp BSP_DISPLAY s1-EXIT;"
expect "%timestamp BSP_DISPLAY s2-ENTRY;"
expect "%timestamp BSP_DISPLAY s2-INIT;"
expect "%timestamp BSP_DISPLAY s21-ENTRY;"
expect "%timestamp BSP_DISPLAY s211-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

dispatch C_SIG
expect "%timestamp BSP_DISPLAY s2-C;"
expect "%timestamp BSP_DISPLAY s211-EXIT;"
expect "%timestamp BSP_DISPLAY s21-EXIT;"
expect "%timestamp BSP_DISPLAY s2-EXIT;"
expect "%timestamp BSP_DISPLAY s1-ENTRY;"
expect "%timestamp BSP_DISPLAY s1-INIT;"
expect "%timestamp BSP_DISPLAY s11-ENTRY;"
expect "%timestamp Trg-Done QS_RX_EVENT"

# the end
end

