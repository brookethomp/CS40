
UM:
  The purpose of this assignment is to implement a software version of a virtual machine, referred to as the "Universal Machine" (UM), 
  to understand virtual-machine code and machine code. The goal is to design and document a program with a clean modular structure, 
  considering the impact of structural choices on program performance. The UM has components such as eight general-purpose registers, 
  a segmented address space, an I/O device using stdin and stdout, and a 32-bit program counter. The assignment involves building an 
  executable named "um" that emulates the UM, taking a file with machine instructions as input. The UM's specifications include machine 
  state components, notation for registers and segments, initial state conditions, and an execution cycle. The UM recognizes 14 instructions, 
  coded by the four most significant bits of the instruction word, with specific semantics for each instruction. 
  The assignment emphasizes clean structure and achieving performance goals for credit.

  Profiling: 
    In this assignment, the focus is on optimizing the performance of the um emulator using code-tuning techniques. The optimization 
    process involves running the large "sandmark" benchmark and using qcachegrind to identify bottlenecks. Profiling is recommended 
    on small inputs initially, ensuring measurable execution times and exercising relevant code paths. Documented the initial state 
    and each stage of improvement, emphasizing a thorough understanding of system changes. Additionally, an analysis of the assembly 
    code for the most resource-intensive procedure in the final program. The goal is not only code improvement but also demonstrating 
    a comprehensive approach to documenting changes and understanding their effects.

  RPN:
    The objective of this assignment is to implement a Reverse Polish Notation (RPN) calculator in Universal Machine Assembly language. 
    The calculator reads commands from standard input and produces results on standard output, mimicking the behavior of the reference 
    implementation in the provided calc40.c file. The calculator employs a value stack, where each value is one Universal Machine word. 
    The implementation involves managing a finite-state machine with waiting and entering states, allowing the calculator to process 
    numerals character by character. The primary states include waiting, entering, and performing commands associated with nondigits. 
    For instance, upon encountering a digit in the waiting state, the machine initiates a numeral, and in the entering state, it 
    continues building the numeral. Nondigits trigger the execution of associated commands, transitioning the machine back to the 
    waiting state. The assignment requires a careful replication of the output produced by the reference implementation, ensuring 
    accurate handling of digits, spaces, and commands to maintain the integrity of the value stack.
