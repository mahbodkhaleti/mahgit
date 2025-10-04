# mahgit

**mahgit** is a Git-like version control system written in pure C as part of a programming project by Mahbod Khaleti. It allows users to initialize repositories, stage changes, commit files, and manage simple project history from the command line, mimicking core Git functionality.

## About the Project

This project was developed entirely in C to strengthen my understanding of version control concepts and demonstrate proficiency with low-level system operations such as file, directory, and attribute management. The goal was to recreate the essential workflow of version control with a focus on usability and the key features found in mainstream tools, while working close to the OS level.

## Features

- **Repository Initialization**
  - Initialize a new `.mahgit` repository in your working directory (`init` command).
- **Staging Area**
  - Add files or directories to the staging area via `add`. Supports recursive directory addition.
- **Commit System**
  - Commit staged changes with author and message tracking (`commit`).
- **Branch Management**
  - Simple branch management. Defaults to `master`; additional branches can be managed via files.
- **Config User Info**
  - Set username/email locally or globally for commit attribution.
- **File Reset/Remove**
  - Remove messages from staging or revert recent actions on files and directories.
- **Tree View**
  - Print directory structure up to a specified depth with `-n` option.
- **Cross-Platform Windows Support**
  - Leverages Windows API for attribute/hidden files and system commands like `xcopy`.
- **Basic Hash Functionality**
  - Simple hashing is included for core operations.
- **Error Handling**
  - Detailed messages inform the user during each step.
- **English and Persian Output**
  - Output messages are available in both English and Persian.

## How It Works

- Single-file C source: `mahgit.c`
- Direct file and folder manipulation using OS/system calls
- Repository data stored in hidden `.mahgit` directory
- Command-line arguments handle all actions and features
- Focused on Windows OS APIs and utilities

## Getting Started

1. **Clone the repository:**
   git clone https://github.com/mahbodkhaleti/mahgit.git
2. **Compile the source:**
   gcc mahgit.c -o mahgit
3. **Run and use commands:**
   ./mahgit init
   ./mahgit add -f yourfile.c
   ./mahgit commit -m "Initial commit"
*(See source code for detailed command usage.)*

## Limitations and Future Work

- Supports Windows OS only (uses Windows-specific APIs and commands)
- Does not include advanced Git features (merge, remote repositories)
- Intended for educational/demo purposes—core C implementation of basic VCS logic

## Author

**Mahbod Khaleti**  
Student & Developer  
*Fop Project Assignment*

