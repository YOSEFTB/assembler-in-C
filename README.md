# assembler-in-C
a C project designed to implement an assembler

## Overview

This project is an assembler written in C. It reads assembly source files, processes macros, and performs a two-pass assembly process to generate machine code or intermediate files. The assembler supports handling multiple input files and provides feedback on missing or invalid files.

## Features

- Reads assembly source files (`.as`).
- Processes macros in the source files, generating `.am` files.
- Performs two-pass assembly to handle labels, symbols, and instructions.
- Command-line interface to assemble one or more files at once.
- Error handling for missing or invalid files.

