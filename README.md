# MicroCodeCompiler
A c++ library for compiling old style micro code<br>
for background info either read "background info.md" or follow the instructions in "language docs.md"<br>
for documentation on the language itself, read "language docs.md"

the solution contains two projects, the library and the compiler.

to use the compiler, just open a .miccode file with it.

using the library:

build the library, note that in debug mode, the library makes debug checks, in release mode, the library assumes that the input is correct.

include the header file and make sure the .lib file is accessible to the project.

get a runtime using
```cpp
MicroCodeCompiler::Runtime r= MicroCodeCompiler::Compile(FilePath);
```
be aware that if the filepath uses \\, they will be converted to /, and not on a copy, on the array you have provided

then, to execute the runtime and get the output for a given eeprom, do:
```cpp
uint64_t* a;
size_t length=r.Run(Eeprom,a);
```
Eeprom is an int, signifying which Eeprom you want to evaluate (starts from 0)<br>
to get the Amount of eeproms use `r.EepromCount`.

to run a Runtime for all eeproms and write the output to files use:
```cpp
MicroCodeCompiler::Write(r,FilePath);
```
where r is the runtime and FilePath is the path to the code file,<br>
the output directory will be generated in the same directory as the code file
