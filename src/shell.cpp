// Group Members: Ivy Swenson, Spencer Baysinger, Kohlby Vierthaler, and Jack Desrochers
// File Name: shell.cpp
// Date: 2025/04/23
// Description: This file contains the implementation of the shell class, which is responsible for managing the shell's command line interface. 
//              It handles user input, command execution, and output display. The shell class also manages the history of commands entered by 
//              the user and provides functionality for navigating through the command history.




// =========================== Notes from the current programmer ============================
// Author of phase 3: Ivy Swenson
// Started: 04-26-2025
// Updated: 04-28-2025
// Goal: Refine and simplify the code from the previous programmer
// Result: The code runs perfectly
// Next goal: Proceed to phase 4




#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>



// ========================= Define Command Sructure =============================
struct Command {
  std::vector<std::string> args; // Arguments of the command
  bool background = false;       // Whether the command runs in background
};

// ========================= Parse InputLine into multipule Token-Groups =============================
std::vector<Command> parseCommandLine(const std::string& inputLine) {
  std::vector<Command> commands;
  std::regex rgx(R"(([^;&]+)([;&])?)"); // Regex: Match command and optional separator
  auto it = std::sregex_iterator(inputLine.begin(),inputLine.end(), rgx);
  auto end = std::sregex_iterator();

  for (; it != end; ++it) {
    std::string commandText = (*it)[1];
    std::string delim = (*it)[2];

    std::istringstream cmdStream(commandText);
    std::string arg;
    Command cmd;

  // Split command into arguments
    while (cmdStream >> arg) {
      cmd.args.push_back(arg);
    }
  // Set background flag if delimiter is '&'
    cmd.background = (delim == "&");
    commands.push_back(cmd);
  }
 
  // Last command should be foreground by default
  if (!commands.empty()) {
    commands.back().background = false;
  }

  return commands;
}


// ========================= Execute Commands with fork() and execvp() =========================
class Executor {
  public:
    void execute(const std::vector<Command>& commands) {
      for (const auto& cmd : commands) {
        if(cmd.args.empty()) continue; // Skip empty commands

        pid_t pid = fork();
        if (pid == 0) {
          // Child process: convert args to char* array
          std::vector<char*> argv;
          for (auto& arg : cmd.args)
            argv.push_back(const_cast<char*>(arg.c_str()));
          argv.push_back(nullptr);

          execvp(argv[0], argv.data()); // execute the command
          perror("execvp failed");      // If execvp returns, there was an error
          exit(1);
        } else if (pid > 0) {
          // Parent process 
          if (!cmd.background) {
            int status;
            waitpid(pid, &status, 0); // Wait for foregound processes 
          }
        } else {
          perror("fork failed");
        }
      }
    }
  };
  

// ========================= Main function ==========================
int main() {
    std::string input;
    Executor executor;
    std::regex validInput("[a-zA-Z0-9-./_ ;&]+"); // Allow ; and & also
  
    while (true) {
      std::cout << "cs321% ";
      std::getline(std::cin, input);
  
      // Handle built-in "exit"
      if (input == "exit") break;
  
      // Check input validity
      if (!std::regex_match(input, validInput)) {
        std::cerr << "Error: Invalid input character(s). Only a-z, A-Z, 0-9, -, ., /, _, space, ;, & are allowed.\n";
        continue;
      }
      if (input.length() > 100) {
        std::cerr << "Error: Input limited to 100 characters.\n";
        input = input.substr(0, 100);
      }
  
      // Parse and execute the input
      auto commands = parseCommandLine(input);
      executor.execute(commands);
    }
  
    return 0;
  }