# Simplex
Simple Programming Language. Inspired by Lisp programming language.<br/>
It's a Lisp for everyone, and no one.

**[Get Simplex Here](https://github.com/MohammedRashad/Simplex/releases)**

## Introduction 


### Brief Intro

Simplex is a Lisp-based programming language (lisp-diaclet or lisp-variant), built for experimentation and educational purposes as a side project, it was meant to be different from the mainstream languages with some amazing features borrowed from many languages.<br/>

The motivation behind it was to push my limits in programming and in **C** especially, also as a part of my learning process to explore computer science and as a personal challenge.

More detailed documentation and tutorials is being added.

### What's simplex ?

Simplex is simple programming language. Inspired by Lisp, Python, Fortran and BASIC.<br/>
With many improvements and modifications, it's my trial to re-invent Lisp.
<br/><br/>


## Main Features 

- Functional
- Turing-Complete
- Imperative
- Dynamically-Typed
- Weakly-Typed
- Live Interpreter
- Batch Interpreter
- Implemented in C++


## What's working right now ?

- Variables
- Scopes
- Live Interpreter
- Functions
- First Order Functions 
- Recursion
- Standard Library
- Strings
- Floating Point Numbers
- Logical Operations
- Arithmetic Operations 
- Batch Interpreter
- User Input 
- Loading Files
- Library Loading
- If Conditional Statements 
- Case Statements
- Select Statements
- List operations

## Basic Tutorial 

**Printing :**

    (print "Hello World")
    
**Input :**

    (scan "x")
**Arithmetic :**

    (+ 3 4 5 6 7) == 25
    (- 3 2) == 1
    (* 3 4) == 12
    (/ 12 6) == 2
    
**Variables :**

    (define [a] 100)
    
**Lists :**

    (define [w] [1 2 3 4 5 6])
    
**Functions :**

    (fun [add x y] [+ x y])

**Comments:**

    # This is a comment 
    # Multiline comment
    
**Conditonal Statements :** 

    # If Statement 
    if (<condition>)
        [<condition true>]
        [<condition false>]
    
    # Select Statement
    select
        [ (<case>) <do stuff> ]
        [ (<other case>) <do other stuff> ]
        [ otherwise (<do other other stuff>) ]
    
    # Case Statement
    case x
        [0 "Monday"]
        [1 "Tuesday"]
        [2 "Wednesday"]
        [3 "Thursday"]
        [4 "Friday"]
        [5 "Saturday"]
        [6 "Sunday"]


   
## Documentation and Tutorials 

You can always refer to [Simplex Wiki](https://github.com/MohammedRashad/Simplex/wiki/) for in-depth details.

## License 

This project is signed under GNU Public License V3.0
