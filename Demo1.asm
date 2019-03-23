; Erik Yu
; V00865663
; demo1.asm is a simple demo related to memory manipulation which basically 
; uses nested loops to fill in every memory with the same number in Bank #0 and Chip #0

clb				/ Clear Accumulator & Carry
dcl				/ Operate CM-RAM PINs on CPU
fim 0p $00		/ Chip#0 Register: R0, R1 Memory location: 0000
fim 2p $c0		/ Chip#0 Register: R4, R5 Memory location: 0000
ldm 15			/ Load immediate 15 as the number to fill in every memory space

loop,			
src 0p			/ Select the predefined chip and register
wrm				/ write memory taking a value from accumulator
isz 1, loop		/ increment R1 and skip to loop

wr0				/ write status 0
wr1				/ write status 1
wr2				/ write status 2
wr3				/ write status 3

inc 0			/ increment R0
isz 4, loop		/ increment R4 and skip to loop
wmp				/ write 15 to RAM port
wrr  			/ write 15 to ROM port

NOP