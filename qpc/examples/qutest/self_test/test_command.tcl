# test-script for QUTest unit testing harness
# see https://www.state-machine.com/qtools/qutest.html

# preamble...
proc on_setup {} {
    expect "%timestamp ON_TEST_SETUP"
}
proc on_teardown {} {
    expect "%timestamp ON_TEST_TEARDOWN"
}

# tests...

#----------
test "Command"
command COMMAND_X 1 2 3
expect "%timestamp COMMAND_X 0"
expect "%timestamp Trg-Done QS_RX_COMMAND"

# the end
end
