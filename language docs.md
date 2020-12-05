this is for programming an EEPRom like Ben Eater did for his computer.<br>
You should watch Ben Eater's 8 bit computer series on youtube, it gives background info to the project, though you will be fine without it.<br>
notice, comments in all files are like in C/C++/C#/JS



## descriptor file {.micdesc}

this file will contain a description of the machine.<br>
each line starts with a keyword, a colon and a definition.<br>
keyword capitalization does not matter.<br>
all whitespace is ignored apart from \n which separates keyword:definition pairs.

keywords:<br>
EepromCount: the amount of eeproms.<br>
EepromAdressLength: self explanatory (in bits). must be smaller or equal to 64, also limitted by the amount of virtual ram you have<br>
EepromOutputLength: self explanatory (in bits). must be smaller or equal to 64

Address: this will contain a description of the composition of the address line.<br>
the composition's size must be smaller than or equal to EepromAdressLength.<br>
the composition's structure is: (partname)*[length]*,(partname)*[length]*,(partname)*[length]*...<br>
if length is 0, the part is ignored.<br>
if the length is not mentioned and there are no [], length is assumed to be one.<br>
if length is 1, the part can be used like a bool.<br>
there are 2 special parts, 'instruction' and 'step'.<br>
in the main file, address parts that are not special can be used as conditionals.

Adress Example<br>
```
{
eepromaddresslength:16
address: step[4],instruction[4],flags[6],debug
}
```
as you may notice, the line can be shorter then eepromaddresslength<br>
also note that the least significant bit is on the left.

Output: this describes the composition of the EEPRoms' control output.<br>
its size must be smaller than `(EepromCount * EepromOutputLength)`.<br>
structure: (controlname)*[length]*,(controlname)*[length]*,(controlname)*[length]*...<br>
the structure works exactly like Address's structure,<br>
except, a semicolon between two control parts means that<br>
the rest of the output line of the eeprom that has the first control part is all zeros.<br>
An exclamation mark before a name means it is active low.<br>
the output line must be declared after its length is declared!<br>
an output line part may not be partially in 1 eeprom and partially in the other.

Output Example
```
{
EepromCount: 2
EepromOutputLength: 4
output: !HLT,AI; BO,MAGIC[3]
}
```
because of the semicolon, the output line becomes:<br>
{!HLT,AI,0,0},{BO,MAGIC[3]}<br>
the curly brackets indicate the two EEPRoms.


## code file {.miccode}

this file describes the instructions in terms of control parts and address parts.<br>
the first line must be
```
#def "*path to definition file*"
```
path must use /, not \\<br>
the basic structure of an instruction is:
```
*(function name): (instruction){
code
}
```
`instruction` is the value of the instruction address part.<br>
the function's name does not matter, it's just to make the code easier to understand<br>
there is one function with a different structure:
```
*fetch{
*code*
}
```
fetch is meant for the fetch cycle which is the same,<br>
no matter the instruction and thus the instruction is not specified.<br>
notice that every function declaration starts with a *.

now for the *code* part<br>
the code includes setting instruction, which set parts of the control word and semicolons, which seperate different control words.<br>
for bool values (length of 1), to set them to true you just need to write their name.<br>
you can set a few bools on the same line by separating them with an "or" line (|).<br>
to set a value with length greater then 1 you type.<br>
(name)= *value/expression*.<br>
the value can be an integer (3), a binary number (b11), a hex number (x3) or an address part.<br>
you can also access a certain part of any value longer then 1 using []<br>
example, using values from previous examples.
```

*fetch{
   BO //notice, this line does not end with a semicolon so this line and the next line are of the same step.
   MAGIC[1]=1;
   HLT;
}

*random: b0010{
    AI|BO;
    AI //notice, this line does not end with a semicolon so this line and the next line are of the same step.
    MAGIC=flags[0,2];//this will set MAGIC to bits 0, 1 and 2 of flags

}
```
while these examples are illogical, they convey the concept well enough.

conditionals:
I think you'll understand from an example.
```
*func: 5{//you can also just type the number normally
    AI;
    if(flags[0]){
        HLT;
    }
    BO;
}

*func2: b0111{
    AI;
    if(debug){
        HLT
    };
    BO;
}
```
notice that different location of the semicolon.<br>
the first function will become.
```
if(flags[0]){
    AI;
    HLT;
    BO;
}
else{
    AI;
    BO;
}
```
while the second becomes.
```
if(debug){
    AI;
    HLT;
    BO;
}
else{
    AI;
    *nothing*;
    BO;
}
```
you can use if/else, there is no switch statement as I am too lazy to implement that.

in an if you can also do
`if(*address part*==*constant value or address part*)`
you can't use boolean logic operators like | and &


### miscellaneous info:
file encoding: utf-8<br>
newline: LF ('\n') or CRLF ("\r\n")

About the output files:<br>
the compiler will generate a folder names "out" that will contain one output file per each Eeprom<br>
every control word in the output file is always 64 bits.

when you compile the code file, it will compile into multiple files (one per EEPRom).<br>
I'll make a folder that has code that emulates ben eater's code.
