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
test "Peek/Poke/Fill uint8_t"
current_obj AP buffer
fill 0 1 100 0x1A
peek 0 1 5
expect "%timestamp Trg-Peek Offs=0,Size=1,Num=5,Data=<1A,1A,1A,1A,1A>"
peek 95 1 5
expect "%timestamp Trg-Peek Offs=95,Size=1,Num=5,Data=<1A,1A,1A,1A,1A>"
fill 2 1 95 0x1B
peek 0 1 5
expect "%timestamp Trg-Peek Offs=0,Size=1,Num=5,Data=<1A,1A,1B,1B,1B>"
peek 95 1 5
expect "%timestamp Trg-Peek Offs=95,Size=1,Num=5,Data=<1B,1B,1A,1A,1A>"
fill 0 1 100 0xA1
poke 2 1 [binary format cccc 0xB1 0xC1 0xD1 0xE1]
peek 0 1 7
expect "%timestamp Trg-Peek Offs=0,Size=1,Num=7,Data=<A1,A1,B1,C1,D1,E1,A1>"

#----------
test "Peek/Poke/Fill uint16_t" -noreset
fill 0 2 50 0x2A2B
peek 0 2 3
expect "%timestamp Trg-Peek Offs=0,Size=2,Num=3,Data=<2A2B,2A2B,2A2B>"
peek 94 2 3
expect "%timestamp Trg-Peek Offs=94,Size=2,Num=3,Data=<2A2B,2A2B,2A2B>"
fill 2 2 48 0x2C2D
peek 0 2 3
expect "%timestamp Trg-Peek Offs=0,Size=2,Num=3,Data=<2A2B,2C2D,2C2D>"
peek 94 2 3
expect "%timestamp Trg-Peek Offs=94,Size=2,Num=3,Data=<2C2D,2C2D,2A2B>"
fill 0 2 50 0xA2B2
poke 2 2 [binary format ss 0xB2C2 0xD2E2]
peek 0 2 4
expect "%timestamp Trg-Peek Offs=0,Size=2,Num=4,Data=<A2B2,B2C2,D2E2,A2B2>"

#----------
test "Peek/Poke/Fill uint32_t" -noreset
fill 0 4 25 0x4A4B4C4D
peek 0 4 3
expect "%timestamp Trg-Peek Offs=0,Size=4,Num=3,Data=<4A4B4C4D,4A4B4C4D,4A4B4C4D>"
peek 88 4 3
expect "%timestamp Trg-Peek Offs=88,Size=4,Num=3,Data=<4A4B4C4D,4A4B4C4D,4A4B4C4D>"
fill 4 4 23 0x4C4D4E4F
peek 0 4 3
expect "%timestamp Trg-Peek Offs=0,Size=4,Num=3,Data=<4A4B4C4D,4C4D4E4F,4C4D4E4F>"
peek 88 4 3
expect "%timestamp Trg-Peek Offs=88,Size=4,Num=3,Data=<4C4D4E4F,4C4D4E4F,4A4B4C4D>"
fill 0 4 25 0xA4B4C4D4
poke 4 4 [binary format ii 0xB4C4D4E4 0xB5C5D5E5]
peek 0 4 4
expect "%timestamp Trg-Peek Offs=0,Size=4,Num=4,Data=<A4B4C4D4,B4C4D4E4,B5C5D5E5,A4B4C4D4>"


# the end
end

