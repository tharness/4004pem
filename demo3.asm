;Erik Yu
;V00865663
;This demo3.asm is used for calculating the 2^3 using nested loops. The final value is stored in Register 0 memory 0.

clb         /clear accumulator and carry
fim 0p $23  /Base: 2 Exponent: 3
sub R0      
xch R2      
ldm 0
sub R1
iac
xch R4

;This part uses nested loops to add all the multiplications
looper,     /two loops start
loop,       /Loop starts
add R0      
isz 2 loop  
add R5
xch R5

sub R0
xch R2
isz 4 looper
add R5

ldm 0
xch R5

fim 6p %00  /Chip#0 Register: RC, RD Memory location:0000
src 6p      /Select the predefined chip and register
wrm         /Write memory
jun end

end,
KBP