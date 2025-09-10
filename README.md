# C Assembler Lab

Assembler project in C for university lab.  
Reads `.as` files and outputs `.ob`, `.ent`, and `.ext` files.

## 🛠️ Project Description

This project implements a **two-pass assembler** in the C programming language.  
It processes input assembly files written in a simplified format and generates the corresponding machine code.

Main features:
- Parses and processes `.as` files
- Generates output files:
  - `.ob` – object code  
  - `.ent` – entry symbols  
  - `.ext` – external symbols  
- Handles labels, macros, data directives, and instructions  
- Validates syntax and reports errors  


## 📁 Project Structure

- `assembler.c` – Main program logic  
- `macro_preproc.c/h` – Preprocessor for macros  
- `step1.c/h`, `pass2.c/h` – First and second pass  
- `label.c/h`, `instr_utils.c/h`, `line_parser.c/h`  
- `code_writer.c/h`, `data_directives.c/h`, `Errors.c/h`  
- `makefile` – For compiling the project

  
## 🧪 Example Usage

```bash
./assembler test_input/example1.as
```

Outputs:
```
example1.ob  
example1.ent  
example1.ext
```

## 🔧 Build Instructions

```bash
make
make clean
```

## 👩‍💻 Authors

Avital & Shira

