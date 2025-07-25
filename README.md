# tinycalc
Tinycalc is a quick and dirty calculator REPL written in C99. It features a recursive-descent parser and tree-walk interpeter for math expressions.

Built mostly for educational purposes, and as a ground to build more complex interpreters.

## Features
    
- Binary operators (+, -, *, /, ^)
- Bit manipulation (<<, >>, &, |, ~)
- Parentheses "(", ")"
- Standard functions (sin, abs, etc.)
- Type-checking against integers and floating-point expressions
- Follows C operator precedence 

## Example usage
    
    > 1 - 2
    -1
    > (1+2)*4 + 10
    22
    > -1 * (-1 + 5)
    -4

## Grammar

    <expr>         = <bitor> 
    <bitor>        = <bitand> { "|" <bitand> }*
    <bitand>       = <shifts> { "&" <shifts> }*
    <shifts>       = <add_sub> { ("<<" | ">>") <add_sub> }*
    <add_sub>      = <mult_div> { ("+" | "-") <mult_div> }*
    <mult_div>     = <power> { ("*" | "/") <power> }*
    <power>        = <unary> { "^" <unary> }*
    <unary>        = ("-" | "~" | <func> ) <unary> | <primary>
    <primary>      = "(" <expr> ")" | <number>


 
            
