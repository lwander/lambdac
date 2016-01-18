# Lambda Calculus Compiler in C 

An attempt at writing a lambda calculus compiler in C for educational purposes.

___

## Compilation


```
$ sudo apt-get install gcc-multilib # for <assert.h>
$ make
```

___

While it would be fun to dive immediately into the compilation process for 
a lambda calculus, I think we need to first examine how a lambda calculus 
interpreter works to get a feel for the dynamics and semantics of the language.

## Semantics

```
<var> ::= [a-zA-Z0-9]*
<lambda> ::= (\<var>.<expression>)
<application> ::= (<expression> <expression>)
<expression> ::= <var> | <lambda> | <application>
```

What's written above is the [BNF](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_Form) 
for our lambda calculus. Essentially, it's saying that our language is composed
of `var`, `lambda`, `application`,  and `expression` terms.
