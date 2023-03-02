# KNL

## What it is

KNL (KiminoLang) is a functional, weakly-typed, interpereted language, designed for the [HorizonOS](https://github.com/notsomeidiot123/horizon-os) Operating System. the token interpreter is easy enough to port, with the hardest part to port being the Lexical Analyzer. 

The language has a pretty small spec, as it's meant to be a BASIC-like programming language, just more powerful, and significantly faster. Almost like the ViM of shell languages, however, depending on it's enviornment, the language should behave slightly differently

The four "Operating modes" are as follows:

* Shell: Behave similar to bash, execute things on a input line-by-line basis, instead of reading from file. 
* Shell Script: Exactly the same as Shell, but read from a file. This must be specifed by the directive `#shell`, however it must be a trimmed-down version of the language itself. features such as Stdlib, `#includes` and functions are not present, as to allow for faster execution, but allows for labels to be defined, just like you normally can
* RTE: Language should execute in a semi-safe runtime enviornment, with full features. The RTE provides more type-saftey, but is still weakly typed. 

NOTE: Sorry about the awful code formatting, Replit's IDE is god-awful, and I've been using it for the majority of this project, as I have been working on it in my free time at school. The auto-formatting is the worst
format that i've seen, and more than half the time, I have to manually enter 7 tabs just to get to the right indentation level, and I'm kinda lazy.  
However, I just reformatted the entire thing in VS Code, so, hopefully formatting should be better (And more readable) from now on. Though, to be fair, if you understand the code, I'm honestly impressed, and surprised. Great job :)

## Dependencies:

### Build:

- make
- g++

### Run: 

- Compiled program
- Any shared-object libraries 

## How to use:

To enter Shell mode:

    knl shell

To enter File modes (Safe RTE, Shell Script, etc)
    
    knl {file} [-w|-u|-t]

## Credits

[Notsomeidiot123](https://github.com/notsomeidiot123) - Kimi

- Designer, Main Programmer and Maintainer

## Latest Version:

Here, There will be the changelog for the most recent (Major) version. Currently version 0.1.0 is in development, and will be released on [Github](https://github.com/notsomeidiot123/KNL/releases).