4004pem (2019)

Authors:
    Trevor Harness, Akash Charitar

The 4004 Pipelined EMulator is an emulator of the Intel 4004 CPU with an optional two-stage pipeline.
It currently supports 19 of the 46 instructions available to the 4004. With memory access,
arithmetic, and conditional branching, it is capable of running any algorithm within the limitations
of memory. The memory configuration is 1 bank containing 1 chip.

Build Instructions:

    Requirements:
        - pthreads
        - gcc
        - make
    
    Build:
        $ make

Usage Instructions:

    Requirements:
        - assembled 4004 program using only the 19 supported instructions and terminated with special
          instruction KBP
        - built executable 4004pem
    
    Running:
        $ ./4004pem [-p] programName
        - The -p option enable pipelining


Notes:
    - Three demo programs are included: Demo1_hex, Demo2_hex, Demo3_hex
    - The resource used as an instruction set reference, assembler, and emulator for testing purposes
      can be found at: http://e4004.szyc.org/index_en.html