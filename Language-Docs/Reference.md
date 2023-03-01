# Reference

## Table of contents:

[1.0](#10-getting-started) Getting Started  
[1.1](#11-really-getting-started) Really Getting Started  
[1.2](#12-crash-course-to-rpn) Crash Course to RPN  

## 1.0: Getting Started

Welcome to KNL! I'm so glad you decided to attempt to learn this awful, awful language.
Before I say any more, I would like to say a couple of things.

1. KNL Is meant to be EASILY PORTABLE, not necesarily easy to use or learn.
	 I wanted it to be easy to port over, so I could have a couple of basic test programs, that would work on both
	 Linux and my OS without any  changes to the source code
2. KNL Is meant to be the shell language for my [Homebrew OS](https://github.com/notsomeidiot123/Horizon-OS)
3. KNL Is meant to be FLEXIBLE. Depending on the run time paramaters, and how the interpreter itself is written,
   KNL can either be your standard, run of the mill,
   Interpereted language, or a fully fledged language with raw, unsafe access
   to memory and hardware
4. KNL does things that make sense to the STACK, not the programmer

Fun fact: KNL executes 10x better than Python, even with an unoptimized build (-O0)

### 1.1 Really getting started:

Sorry about all that rambling at the beginning, I promise we're getting into the actual Documentation.

as mentioned before, KNL does things that make the most sense to the computer-
The way the stack works. 
As such, it does things that are the most natural for the CPU and Stack to handle.
KNL uses [Reverse Polish Notation](https://en.wikipedia.org/wiki/reverse_polish_notation)

#### 1.2 Crash course to RPN

When writing expressions or equations in KNL, We use RPN, which follows the structure of

 	operand1 operand2 operation
for example, to subtract 3 from 7, we would do
	
 	3 7 -
as opposed to [Infix Notation](https://en.wikipedia.org/wiki/Infix_notation)  
A quick way to remeber it is, take the operand on the left hand side of the operator, and
shove it in front of the whole expression.

### 1.1 Really getting started - Continued

So now that we know how mathematical expressions are written 
we can start to take a look at getting started

1st, to download simply go to https://github.com/notsomeidiot123/KNL/releases
and download the latest release of the KNL Interpreter. 

OR

run the following command:

	git clone https://github.com/notsomeidiot123/KNL/tree/stable && make

To be able to use the interpreter outside of the folder it is stored in, run

	cp knl /bin/knl

To start the interactive shell, run 

	knl shell

Or to execute a file, run 
	
 	knl {path/to/file}

To see all available switches, run 

	knl -h

More information on the command line later.