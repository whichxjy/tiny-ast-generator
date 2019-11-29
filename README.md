# Tiny AST Generator

This is a simple AST generator for the extended Tiny programming language. See the textbook "Compiler Construction: Principles and Practice" for more details.

## The EBNF Grammar

```
<program> := <proc-defs> <stmts>

<proc-defs> := [ <proc-def> ] { <proc-def> }
<proc-def> := "proc" <id> "begin" <stmts> "end"

<stmts> := [ <stmt> ";" ] {  <stmt> ";" }
<stmt> := <read-stmt> | <write-stmt> | <if-stmt>
        | <repeat-stmt> | <break-stmt> | <continue-stmt>
        | <assign-stmt> | <proc-call-stmt>

<read-stmt> := "read" <id>
<write-stmt> := "write" <expr>

<if-stmt> := "if" <expr> "then" <stmts>
             [ "else" <stmts> ] "end"

<repeat-stmt> := "repeat" <stmts> "until" <expr>
<break-stmt> := "break"
<continue-stmt> := "continue"

<assign-stmt> := <id> ":=" <expr>
<proc-call-stmt> := "call" <id>

<expr> := <single-expr> { ("<" | "=") <single-expr> }
<single-expr> := <term> { ("+" | "-") <term> }
<term> := <factor> { ("*" | "/") <factor> }
<factor> := "(" <expr> ")" | <id> | <integer> | <float>

<integer> := <digit> { <digit> }
<float> := <digit> { <digit> } "." <digit> { <digit> }

<letter> := "A" | "B" | "C" | "D" | "E" | "F" | "G"
          | "H" | "I" | "J" | "K" | "L" | "M" | "N"
          | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
          | "V" | "W" | "X" | "Y" | "Z" | "a" | "b"
          | "c" | "d" | "e" | "f" | "g" | "h" | "i"
          | "j" | "k" | "l" | "m" | "n" | "o" | "p"
          | "q" | "r" | "s" | "t" | "u" | "v" | "w"
          | "x" | "y" | "z"
<digit> := "0" | "1" | "2" | "3" | "4"
         | "5" | "6" | "7" | "8" | "9"
```

## Usage

``` makefile
# Compile the code
make
# Run the program
./bin/tiny /path/to/the/source/code.tny
```

## Example 1: Generating AST

- [test/example.tny](./test/example.tny)

    ```
    {
    Sample program
    in TINY language
    }

    proc print123 begin
        k := 123;
        write k;
    end

    read x; { input an integer }

    fact := 1;

    if 0 < x then { don't compute if x <= 0 }
        fact := 1;

        repeat
            fact := fact * x;
            x := x - 1;
        until x = 0;

        write fact; { output factorial of x }
    end;

    { --------------------------------------- }

    i := 0;

    repeat
        if 55.5 < i then
            call print123;
            break;
        else
            i := (i + 2.4) * 3;
        end;
    until i < 70.5;
    ```

- Run `./bin/tiny test/example.tny`:

    ```
    [========== AST ==========]
    Function Definition
        ID: print123
        Assign to: k
            Integer: 123
        Write
            ID: k
    Read: x
    Assign to: fact
        Integer: 1
    If
        Op: <
            Integer: 0
            ID: x
        Assign to: fact
            Integer: 1
        Repeat
            Assign to: fact
                Op: *
                    ID: fact
                    ID: x
            Assign to: x
                Op: -
                    ID: x
                    Integer: 1
            Op: =
                ID: x
                Integer: 0
        Write
            ID: fact
    Assign to: i
        Integer: 0
    Repeat
        If
            Op: <
                Float: 55.500000
                ID: i
            Call Procedure: print123
            Break
            Assign to: i
                Op: *
                    Op: +
                        ID: i
                        Float: 2.400000
                    Integer: 3
        Op: <
            ID: i
            Float: 70.500000
    ```

## Example 2: Detecting Syntax Error

- [test/example-error.tny](./test/example-error.tny)

    ```
    {
    Sample program
    in TINY language
    }

    proc print123 begin
        k := 123;
        write k;
    end

    read x; { input an integer }

    fact := 1;

    if 0 < x then { don't compute if x <= 0 }
        fact := 1;

        repeat
            fact := fact * x;
            x = x - 1;
        until x = 0;

        write fact; { output factorial of x }
    end;

    { --------------------------------------- }

    i := 0;

    repeat
        if 55.5 < i then
            call print123;
            break;
        else
            i := (i + 2.4) * 3;
        end;
    until i < 70.5;
    ```

- Run `./bin/tiny test/example-error.tny`:

    ```
    Syntax error at line 20: Unexpected Token -> =
    ```