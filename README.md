# KNL

## What it is

KNL (KiminoLang) is a functional, weakly-typed, interpereted language, designed for the [HorizonOS](https://github.com/notsomeidiot123/horizon-os) Operating System. the token interpreter is easy enough to port, with the hardest part to port being the Lexical Analyzer. 

The language has a pretty small spec, as it's meant to be a BASIC-like programming language, just more powerful, and significantly faster. Almost like the ViM of shell languages, however, depending on it's enviornment, the language should behave slightly differently

The four "Operating modes" are as follows:

* Shell: Behave similar to bash, execute things on a input line-by-line basis, instead of reading from file. 
* Shell Script: Exactly the same as Shell, but read from a file. This must be specifed by the directive `#shell`, however it must be a trimmed-down version of the language itself. features such as Stdlib, `#includes` and functions are not present, as to allow for faster execution, but allows for labels to be defined, just like you normally can
* Safe RTE: Language should execute in a safe runtime enviornment, with full features, including functions, arguments, full operations, `#include`s, and the Stdlib. The RTE provides more type-saftey, but is still by default weakly-typed
  * NOTE: the `#strong` directive enforces full type saftey, enables "lose-casting," and enforces compatibility checks. be sure to use this for the safest code possible. Pointers in a Safe RTE file are disabled, and the attempt to use them will throw `SYS_SAFE_VIOLATION` error
* Unsafe: Language should execute in a open enviornment. While Enabling Pointers, and keeping automatic Out-of-scope variable deallocation, Garbage collection is disabled, and all types are enforced as ints by default. This is to allow the language to execute code to the fullest extent, With direct access to memory, with the trade-off of some Safe RTE features, to ensure no carefully-planned pointers are deleted by accident. 

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