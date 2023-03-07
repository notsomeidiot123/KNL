# Reference

## Table of contents:

[1.0](#10-getting-started) Getting Started  
[1.1](#11-really-getting-started) Really Getting Started  
[1.2](#12-crash-course-to-rpn) Crash Course to RPN  
[1.3](#13-your-first-program) Your first program
[1.4](#14-expressions) Expressions
[1.5](#15-keywords) Keywords
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

### 1.1: Really getting started:

Sorry about all that rambling at the beginning, I promise we're getting into the actual Documentation.

as mentioned before, KNL does things that make the most sense to the computer-
The way the stack works. 
As such, it does things that are the most natural for the CPU and Stack to handle.
KNL uses [Reverse Polish Notation](https://en.wikipedia.org/wiki/reverse_polish_notation)

#### 1.2: Crash course to RPN

When writing expressions or equations in KNL, We use RPN, which follows the structure of

 	operand1 operand2 operation
for example, to subtract 3 from 7, we would do
	
 	3 7 -
as opposed to [Infix Notation](https://en.wikipedia.org/wiki/Infix_notation)  
A quick way to remeber it is, take the operand on the left hand side of the operator, and
shove it in front of the whole expression.

### 1.1: Really getting started - Continued

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

## 1.3: Your first program!

Alright, so now that we have the interpreter, we can start programming in KNL. How about we start with the classic, "Hello, world!"?

### What we will learn in this section

- print
- strings

This doesnt need much explaining, and it's very straightforward... In concept at least.   
Of course, `print` is the basis for all (text) output. `print` instead of being a function, is instead a keyword with it's own behavior defined.

`print`	has two different "modes", in a sense. The first mode is "Immediate". If there is a printable token directly after a `print` statement, it will- you guessed it- print the immediate, however, it will NOT print the result of expressions following the `print` statement, which leads us to the second mode.  
In the second "mode", the top of the stack is popped, then written to STDOUT. The value is then pushed onto the top of the stack. 
`print`	ALWAYS writes a newline. If you wish for output WITHOUT a newline, see [write](not-found "Not Yet Implemented?")

Now, strings. Strings aren't anything special, and they work the same as any other string in any other language (that's sane). Strings are defined by double quotes (") and terminated by double quotes (string behavior on the stack is still being defined)

And now for the code you've all been waiting for:

	print "Hello, World!"

That's it. No 'main', no

```c#
using System;
class Program{
	public static void Main(string[] args){
		Console.WriteLine("Hello, World!");
	}
}
```
no
```java
class Program{
	public static void main(string[] args){
		System.out.println("Hello, World!");
	}
}
```
no
```c
#include<stdio.h>

int main(int argc char **argv){
	printf("Hello, World!\n");
}
```
not even
```python
print("Hello, World");
```

just one simple keyword, followed by a string. Congrats! you've written your first KNL Program!

## 1.4: Expressions

Here's the list of all valid operators, and their names.

|Symbol|Name|
|------|----|
|+|Addition|
|-|Subtraction|
|*|Multiplication|
|/|Division|
|%|Modulo|
|&|And|
|\||Or|
|^|Xor|
|!|Not|
|~|Negate|
|@|Reader|
|$|Writer|
|<|Less than|
|>|Greater than|
|=|Equal to|
|:|Dupe|

From those, you may be asking a couple of questions. What is a reader/writer? Why is = the equality operator, and not ==? What about complex
boolean expressions? Where's && and ||?

And here are the answers, in order:

- A Reader (@) simply reads from memory. This is necesary to get the value of the variables themselves, rather than the pointer in virtual memory to the variable. A Writer ($) writes the second highest operand on the stack to the highest operand on the stack.
- I made this decision, because I wanted two things: Interpreter code simplicty, and unambiguous meanings to each of the operators, much like phonetic consistency in a spoken language: Each operater does EXACTLY one operation, no matter the context.
- And for the last two, answered at the same time, Be a little bit patient, that parts coming up next.

If statements either respond to the top value on the stack, or evaluate an expression. If you would like to chain multiple boolean expressions, you simply need to apply the "Boolify", or "Bang-Bang" (!!) operator to whatever result or variable you want to. 

The Not operator (!) simply converts any value to the BOOLEAN opposite of the value, so any non-zero value becomes zero, and zero becomes one. 

By chaining two of these together, you get the simple one-bit boolean value of the expression you wish to evaluate so that you may jump. This is not required, unless chaining multiple expressions together.

While you will learn more about the `if` statement later, I will give you a small example

```
//code
if num1 @ then
	//more code
end
//even more code
```
If num1 is non-zero, the if statement will execute. However, if it is zero, it will not, just like any if statement in any language you've used before (If this is your first language, I'm warning you, get out now, while you still can)

however using `if num1 @ num2 @ & then` may not always work, as there is a chance, especially as one of the variables values increases, that the bitwise `&` will return 0, despite both num1 and num2 being non-zero. In this situation, it will be necesary to 'Boolify' the operators, as to ensure that both are checked properly. To do this, simply add a !! after every segment of the expression.

Examples:

```
if num1 @ !! num2 @ !! & then
	//code
end
```
```
num1 num2 - then if !! & num2 0 >;
	//code
end
```

It may seem contradictory to what I just said, right?
> because I wanted two things: Interpreter code simplicty, and unambiguous meanings to each of the operators

Well, It is consistent. Since the not operator converts it into a binary boolean value, adding a not, right after a not, just converts it back into the boolean value of the *original* form.

Examples:

```c
//INFIX NOTATION
!5 == 0 //true
!!5 == 0 //false
!0 == 1//true
!!0 == 0//true
```
```knl
//RPN
5 !! 1 = //true
5 ! 1 = // false
0 !! 0 = //true
0 ! 0 = //false
0 ! 1 = //true
0 !! 0 = //true
5 ! 0 = //true
```

Lastly, the `:` (dupe) operator does exactly what it's name says. It duplicates the top of the stack
so, if, for example, the stack looked like

|P|D|
|-|-|
|2|3|
|1|0|
|0|9|

and you used the dupe operator, the stack would then look like

|P|D|
|-|-|
|3|3|
|2|3|
|1|0|
|0|9|

## 1.5: Keywords