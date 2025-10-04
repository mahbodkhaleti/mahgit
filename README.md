# mahgit
mahgit is a Git-like version control system written in pure C as part of a programming project by Mahbod Khaleti. It is designed to help users initialize repositories, stage changes, commit files, and manage project history, all within a command-line interface inspired by core Git functionality.

About the Project
This project was developed entirely in C to both deepen my understanding of version control logic and demonstrate proficiency with low-level system operations such as file, directory, and attribute management. I aimed to recreate the essential version control workflow, focusing on usability and core principles found in mainstream tools like Git while showcasing careful OS-level handling.

Features
Repository Initialization:
Initialize a new .mahgit repository directory structure in your working directory using an init command.

Staging Area:
Add files or directories to the staging area using add. Handles both files and directories recursively.

Commit System:
Save staged changes using commit, writing essential commit history with author and message.

Branch Management:
Support for simple branching (master branch default, additional files for managing other branches).

Config User Info:
Configure local or global username and email to associate authorship with commits.

File Reset/Remove:
Remove messages from the staging or undo recent actions for files and directories.

Tree View:
Print structured directory trees up to a specified depth with the -n option.

Cross-Platform Windows Support:
Utilizes Windows API for directory attributes, hidden files, and command-line operations (xcopy, etc).

Basic Hash Functionality:
Includes simple hashing for some operations.

Error Handling and Messaging:
Comprehensive standard output to inform users during each step or on errors.

English and Persian Output:
Some output messages are in Persian for user-friendliness, reflecting my background and target audience.

How It Works
The code is contained in a single C file: mahgit.c

Relies on direct interaction with the OS (file/directory manipulation, attribute management)

Stores repository data in a hidden .mahgit folder with structured subfolders for tracking changes

Commands are processed via command line with arguments checked for validity

Uses Windows-specific system calls and commands

Getting Started
Clone the repository:

text
git clone https://github.com/mahbodkhaleti/mahgit.git
Compile the source:

text
gcc mahgit.c -o mahgit
Run and use commands:

text
./mahgit init
./mahgit add -f yourfile.c
./mahgit commit -m "Initial commit"
(see code for more command details)

Limitations and Future Plans
Currently supports Windows OS only (due to API and command usage)

Lacks advanced Git features (merging, remote operations, etc.)

Project is for educational purposes and demonstration of core VCS logic in C

Author
Mahbod Khaleti
Student & Developer
Fop Project Assignment
