# Tiny AST Generator

This is a simple AST generator for the Tiny programming language. See the textbook "Compiler Construction: Principles and Practice" for more details.

## The EBNF Grammar

```
program := stmts
stmts := stmt (";" stmt)*
stmt := if-stmt | repeat-stmt | assign-stmt
      | read-stmt | write-stmt

if-stmt := "if" expr "then" stmts ["else" stmts] "end"
repeat-stmt := "repeat" stmts "until" expr
assign-stmt := identifier ":=" expr
read-stmt := "read" identifier
write-stmt := "write" expr

expr := single-expr (("<" | "=") single-expr)*
single-expr := term (("+" | "-") term)*
term := factor (("*" | "/") factor)*
factor := "(" expr ")" | identifier | number

identifier := letter (letter | digit)*
number := digit+

letter := "A" | "B" | "C" | "D" | "E" | "F" | "G"
        | "H" | "I" | "J" | "K" | "L" | "M" | "N"
        | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
        | "V" | "W" | "X" | "Y" | "Z" | "a" | "b"
        | "c" | "d" | "e" | "f" | "g" | "h" | "i"
        | "j" | "k" | "l" | "m" | "n" | "o" | "p"
        | "q" | "r" | "s" | "t" | "u" | "v" | "w"
        | "x" | "y" | "z"
digit := "0" | "1" | "2" | "3" | "4"
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
    { Sample program
    in TINY language -
    computes factorial
    }
    read x; { input an integer }
    fact := 1;
    if 0 < x then { don't compute if x <= 0 }
    fact := 1;
    repeat
        fact := fact * x;
        x := x - 1
    until x = 0;
    write fact { output factorial of x }
    end
    ```

- Run `./bin/tiny test/example.tny`:

    ```
    [==== AST ====]
        Read: x
        Assign to: fact
            Const: 1
        If
            Op: <
                Const: 0
                ID: x
            Assign to: fact
                Const: 1
            Repeat
                Assign to: fact
                    Op: *
                        ID: fact
                        ID: x
                Assign to: x
                    Op: -
                        ID: x
                        Const: 1
                Op: =
                    ID: x
                    Const: 0
            Write
                ID: fact
    ```

## Example 2: Detecting Syntax Error

- [test/example-error.tny](./test/example-error.tny)

    ```
    { Sample program
    in TINY language -
    computes factorial
    }
    read x; { input an integer }
    fact := 1;
    if 0 < x then { don't compute if x <= 0 }
    fact := 1;
    repeat
        fact = fact * x;
        x := x - 1
    until x = 0;
    write fact { output factorial of x }
    end
    ```

- Run `./bin/tiny test/example-error.tny`:

    ```
    Syntax error at line 10: Unexpected Token -> =
    ```