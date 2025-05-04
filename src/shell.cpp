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

// ========================= Define Command Structure =============================
struct Command {
  std::vector<std::string> args;
  bool background = false;
};

// ========================= Parse Input Line into Multiple Token-Groups =============================
std::vector<Command> parseCommandLine(const std::string& inputLine) {
  std::vector<Command> commands;
  std::regex rgx(R"(([^;&]+)([;&])?)");
  auto it = std::sregex_iterator(inputLine.begin(), inputLine.end(), rgx);
  auto end = std::sregex_iterator();

  for (; it != end; ++it) {
    std::string commandText = (*it)[1];
    std::string delim = (*it)[2];

    std::istringstream cmdStream(commandText);
    std::string arg;
    Command cmd;

    while (cmdStream >> arg) {
      cmd.args.push_back(arg);
    }

    cmd.background = (delim == "&");
    commands.push_back(cmd);
  }

  if (!commands.empty()) {
    commands.back().background = false;
  }

  return commands;
}

// ========================= Optional Input Token Parser =============================
void parseInput(std::string &cmd, std::vector<std::string> &params) {
  std::istringstream stream(cmd);
  std::string arg;
  while (stream >> arg) {
    params.push_back(arg);
  }
}

// ========================= Executor Class =============================
class Executor {
public:
  void execute(const std::vector<Command>& commands) {
    for (const auto& cmd : commands) {
      if (cmd.args.empty()) continue;

      pid_t pid = fork();
      if (pid == 0) {
        std::vector<char*> argv;
        for (auto& arg : cmd.args)
          argv.push_back(const_cast<char*>(arg.c_str()));
        argv.push_back(nullptr);
        execvp(argv[0], argv.data());
        perror("execvp failed");
        exit(1);
      } else if (pid > 0) {
        if (!cmd.background) {
          int status;
          waitpid(pid, &status, 0);
        }
      } else {
        perror("fork failed");
      }
    }
  }
};

// ========================= Main Function =============================
int main() {
  std::string input;
  Executor executor;
  std::regex validInput("[a-zA-Z0-9-./_ ;&]+");

  while (true) {
    std::cout << "cs321% ";
    std::getline(std::cin, input);

    if (input == "exit") break;

    if (!std::regex_match(input, validInput)) {
      std::cerr << "Error: Invalid input character(s). Only a-z, A-Z, 0-9, -, ., /, _, space, ;, & are allowed.\n";
      continue;
    }

    if (input.length() > 100) {
      std::cerr << "Error: Input limited to 100 characters.\n";
      input = input.substr(0, 100);
    }

    auto commands = parseCommandLine(input);
    executor.execute(commands);
  }

  return 0;
}
