# Arrokoth-0

**Arrokoth-0**, name inspired by the trans-Neptunian object **[Arrokoth](https://en.wikipedia.org/wiki/486958_Arrokoth)** and inspired by the **[PL/0](https://en.wikipedia.org/wiki/PL/0)** programming language, is a very basic and simple programming language, created as a learning project about creating parsers, generating ASTs and writing a compiler. Arrokoth-0 generates **LLVM IR** code that can be then compiled into a binary for every CPU architecture that LLVM supports. Arrokoth-0 code shall be put in `.arr` files for aesthetic purposes, but you can use any file extension :)

## Grammar

In EBNF grammar form, the **Arrokoth-0** language can be defined as:

```ebnf
program = "program" block {block} "end" ;

block = var_creation | proc_creation ;

var_creation = "var" identifier {, identifier} ";" ;

proc_creation = "proc" identifier "{" statement ";" {statement ";"} "}" ;

statement = assignment | function_call | while_loop | if_branch | print_var ;

if_branch = "if" condition "{" statement ";" {statement ";"} "}" ;

while_loop = "while" condition "{" statement ";" {statement ";"} "}" ;

print_var = "#" identifier {, identifier} ;

assignment = identifier "=" expression ;

function_call = "run" identifier ;

cond_op = ("=="|"!="|">"|"<"|"<="|">=") ;

condition = expression cond_op expression ;

expression = term {("+"|"-") term} ;

term = factor {("*"|"/") factor} ;

factor = atom {"^" atom} ;

atom = identifier | number | "(" expression ")" ;
```

There are only single line comments, which shall start with `[`.

## Compiler stages

### Lexer

The program is broken up into lexical tokens, as defined in `lexer.c`, to be later parsed into the Abstract Syntax Tree.

### Parser

A recursive descent parser goes token by token, assembling the AST. This tree, however, is not perfect as it contains
residual `Block`, `Statement`, `Expression`, `Term`, `Factor`, `Atom` tokens.

### Tree transform

Gets rid of `Block`, `Expression`, `Term`, `Factor`, `Atom` and `Statement` tokens. This is done to make code generation a bit smoother.

### Semantic analysis

The tree is walked once to check existence of variables before their usage, existence of procedures before they are called, checks for math errors, checks if the `main` function exists and etc. Basically validates the program.

### Code generation

And the last stage, walking down the AST and emitting an LLVM IR file. It's then up to you to pass it through an optimizer, compile into object file and link.

## Compiler args

| Commandline Argument | Type | Description |
| -------------------- | ---- | ----------- |
| `-i` | string | Input file to process. |
| `-o` | string | Output file. |
| `-a` | bool | Generate a GraphViz script (`.dot`) that can be used to visualize the AST. |
| `-ntta` | bool | Same as `-a` except the Tree Transform stage is skipped. |

## Example program

Here is an example program that prints out the 16th Fibonacci number:

```c
[ Fibonacci sequence calculator

program

[ Define our variables:
var F,S,I,Pos,EndPos;

proc Fib
{
    while Pos < EndPos
    {
        [ `I` is for intermediate result.
        I = F + S;
        F = S;
        S = I;
        Pos = Pos + 1;
    };
}

proc main
{
    F = 0; [ fib(0) = 0
    S = 1; [ fib(1) = 1
    I = 0;
    Pos = 0;
    EndPos = 15; [ Set this to the index of the fibonacci number you want - 1
    [ Setting endpos to 15 will give us the 16th Fibonacci number (987)
    run Fib;
    # S;
}

end
```
