/ Erik Yu
/ V00865663
/ This demo2.asm basically multiply two numbers users set in R0 and R1, user can change the multiplier or multiplicand to play with this demo. 
/ In the example given below, we multiply 2 and 6, getting result 12 written in Register 0 memory 1

clb         /clear accumulator and carry
fim 0p $26  /Multiplicand: 2 Multiplier: 6
sub R1      /The accumulator subtract the value of R1
xch R2      /Exchange the value between R2 and accumulator
loop,       /Lopp starts
add R0      /Add the value of R0 to the accumulator
isz 2 loop  /Increment R2 and skip to loop

fim 2p %00  /Chip#0 Register: R4, R5 Memory location: 0000
src 2p      /Select the predefined chip and register
wrm         /Write memory

KBP
