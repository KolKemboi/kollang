# Kollang

**Kollang** is a C-style programming language with `.kol` as its source file extension.

The language is designed to provide a familiar, low-level programming experience while using **LLVM** as its compiler backend. By targeting LLVM, Kollang can leverage LLVM's support for multiple architectures and benefit from its existing optimization and code-generation infrastructure.

The project is an exploration into compiler design, language implementation, and low-level programming.


## Example Conversion

![Example Conversion](https://github.com/KolKemboi/kollang/blob/d6c3e199600d5c8048f105038e7e89f256f81338/images/VarDeclRet.png)

The example above illustrates how basic Kollang variable declarations are represented and converted during compilation.

The currently supported basic types include:

```text
int
float
bool
string
```

## How the Compiler Works

![Compiler Flow](https://github.com/KolKemboi/kollang/blob/d6c3e199600d5c8048f105038e7e89f256f81338/images/Compiler.drawio.png)

The Kol Lang compiler takes raw text and transforms it into machine-executable code through a classic multi-stage pipeline. It progressively lowers the abstraction level, starting from human-readable syntax and ending with raw CPU instructions.

Here is the journey of a simple variable declaration: `int x = 10 + 10;`

**1. Lexing (Tokenization)**
First, the raw input string is scanned and broken down into a flat list of meaningful "tokens." The lexer ignores whitespace and categorizes the text into types like identifiers, keywords, and operators.
* *Example Output:* `INT_TYPE`, `IDENT(x)`, `EQUALS`, `INT_V(10)`, `PLUS`, `INT_V(10)`, `SEMICOLON`

**2. Parsing (Syntax Analysis)**
Next, the parser consumes this flat token stream and organizes it into a hierarchical tree structure (an Abstract Syntax Tree, or AST). It ensures the grammar makes sense—recognizing that this is a variable declaration where `x` is assigned a binary mathematical expression.
* *Example Output:* A `Variable Declaration` node (`name: x`) containing a `Binary Expression` node (`op: +`, `left: 10`, `right: 20`).

**3. IR Generation (Intermediate Representation)**
The AST is then translated into LLVM Intermediate Representation (IR). This acts as a bridge between the high-level language and machine code, allowing for platform-independent optimizations before the final translation.
* *Example Output:* `%tmp = add i32 10, 20`

**4. Assembly Generation (Code Gen)**
Finally, the IR is compiled down into target-specific x86 Assembly. The compiler has now optimized the math and resolved the instructions down to the lowest level before being assembled into a binary.
* *Example Output:* 
  ```assembly
  main:
      mov eax, 30
      ret
