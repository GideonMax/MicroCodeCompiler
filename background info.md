This compiler is for microcode for old cpus.

in cpus, there are components and every component has control signals.<br>
all the components' control signals together make up the control line.<br>
possible values for the control line are called control words.<br>
the part of the cpu which outputs control words onto the control line is calles the control unit.<br>

The control unit in older cpus:<br>
The control unit has a rom/eprom/eeprom/anything with the same function.

the rom has 2 registers connected to its address line:<br>
The instruction register, this register holds the opcode of the current instruction.<br>
A step register, since most instruction take multiple steps, there needs to be a register that stores the current step.<br>
These two register, along with (optionally) some flags, go into the rom's address line and the output of the rom is the control word.<br>

important to mention, the first few steps are the fetch cycle,<br>
this fetch cycle is in every single instruction and it fetches the current instruction,<br>
during the fetch cycle, the instruction register has the previous instruction.<br>
at the end of the cycle, the instruction register has the current instruction.<br>
