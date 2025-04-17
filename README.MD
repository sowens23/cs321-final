# CS-F321 Operating Systems Final Group Project
Here lies the source code of a final group project.  
Authors in no particular order: Ivy Swenson, Kohlby Vierthaler, Spencer Baysinger, and Jack Witt.
Below you will find
- Project Details
- Project Development Phases
- Program Compiling and Running
- Licensing
- Acknowledgements

## Project Details
This program is a shell command interpreter that provides Unix users with an interface to the underlying operating system.  
It interprets basic user commands and executes them as independent forked resources.  

## Project Development Phases
This project has been split into 6 phases/issues.   
Links to phases/issues are included for quick reference.   

### [Phase 0: Repo & Skeleton](https://github.com/sowens23/cs321-final/issues/1)
Owned by Spencer Baysinger.
- Create Repo + README
- Add .gitignore
- Skeleton Code (shell.cpp, Makefile, etc)
  - prints prompt "cs321%"
  - reads a line with std::getLine()
  - exists on EOF

### [Phase 1 – Input & Validation](https://github.com/sowens23/cs321-final/issues/2)
Owned by Kohlby Vierthaler.
- InputBuffer class
  - readLine() → returns trimmed std::string (max 100 chars: truncate + warn).
  - tokenize() → vectorstd::string with simple std::istringstream >>.
- isValidWord() per spec (A‑Z, a‑z, 0‑9, -._/).
  - On bad char: print error, return empty token list.
- (Optional) Add unit tests (GoogleTest or simple assertions).

### [Phase 2 – Built‑ins & Prompt](https://github.com/sowens23/cs321-final/issues/3)
Owned by Jack Witt.
- Builtin module (header + cpp) with enum + dispatch table:
  - exit — sets running=false.
  - print — prints shell PID (getpid()).
  - help — prints usage text.
- Prompt stays cs321% .
- Hook callbacks into the main loop after parsing.

### [Phase 3 – External Command Runner](https://github.com/sowens23/cs321-final/issues/4)
Owned by .
- Command struct { vector args; bool background; };
- fork() / execvp() implementation inside Executor class.
  - Handles ; vs. & (background) within one input line.
  - Waits (waitpid()) only when background == false.
- Split input line into token‑groups separated by ; or & (use owner A’s tokenizer).
- Rule: last command defaults to “;”.

### [Phase 4 – Advanced Features / Bonus](https://github.com/sowens23/cs321-final/issues/5)
Owned by .
- Pipe operator pipes output of one program into another (See project doc in Canvas)
- Verify cd is functional, if not program it in.
- (Optional) If unit test exists, verify program passes.

### [Phase 5 - Polish and Submit Project](https://github.com/sowens23/cs321-final/issues/6)
Owned by .
- Polish README
  - Include notes for shell commands like 'help'.
- Create demo video
- Zip files and submit in Canvas (One submission with all group member names)

## Program Compiling and Running
1) Clone repository.
2) Run Makefile ```make run``` in CS321-FINAL directory.
3) Play with Shell.
4) Close shell safetly with ```Ctrl+D``` EOF command.

## Licensing
This project is licensed under the MIT License.  
You are free to use, modify, and distribute this software, provided that proper credit is given to the original authors.  
For more details, see the LICENSE file included in this repository.

## Acknowledgements
- Instructor of our course CS-F321 Operating Systems: Arghya Das  
- Assistance with outlining phases of project development: OpenAI ChatGPT  
- Authors of project: Ivy Swenson, Kohlby Vierthaler, Spencer Baysinger, and Jack Witt.  
- Any reviewers. 

### Thanks