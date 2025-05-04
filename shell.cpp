// Group Members: Ivy Swenson, Spencer Baysinger, Kohlby Vierthaler, and Jack Desrochers
// File Name: shell.cpp
// Date: 2025/04/23
// Description: This file contains the implementation of the shell class, which is responsible for managing the shell's command line interface. 
//              It handles user input, command execution, and output display. The shell class also manages the history of commands entered by 
//              the user and provides functionality for navigating through the command history.

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
  std::string modifiedInput = inputLine;

  // Append a semicolon if the input doesn't already end with one
  if (!modifiedInput.empty() && modifiedInput.back() != ';' && modifiedInput.back() != '&') {
    modifiedInput += ";";
  }

  std::vector<Command> commands;
  std::regex rgx(R"(([^;&]+)([;&])?)");
  auto it = std::sregex_iterator(modifiedInput.begin(), modifiedInput.end(), rgx);
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
};

// ========================= Executor Class =============================
class Executor {
  public:
  void execute(const std::vector<Command>& commands) {
    std::vector<pid_t> backgroundPids; // Track background process IDs

    for (const auto& cmd : commands) {
        if (cmd.args.empty()) continue;

        pid_t pid = fork();
        if (pid == 0) {
            // Child process: convert args to char* array
            std::vector<char*> argv;
            for (auto& arg : cmd.args)
                argv.push_back(const_cast<char*>(arg.c_str()));
            argv.push_back(nullptr);

            execvp(argv[0], argv.data()); // Execute the command
            perror("execvp failed");      // If execvp returns, there was an error
            exit(1);
        } else if (pid > 0) {
            // Parent process
            if (cmd.background) {
                // For background commands, store the PID and continue
                backgroundPids.push_back(pid);
            } else {
                // For foreground commands, wait for the process to finish
                int status;
                waitpid(pid, &status, 0);
            }
        } else {
            perror("fork failed");
        }
    }

    // Clean up background processes
    for (pid_t bgPid : backgroundPids) {
      int status;
      waitpid(bgPid, &status, WNOHANG); // Non-blocking wait for background processes
    }
  }
};

// ========================= Main Function =============================
int main() {
  std::string input;
  Executor executor;
  std::regex validInput("[a-zA-Z0-9-./_ ;&]+");

  bool running = true;

  while (running) {
    std::cout << "cs321% ";
    std::getline(std::cin, input);

    // Handle built-in "exit"
    if (input == "exit") {
      running = false;
      continue;
    }

    // Handle "print" command to display the shell's PID
    if (input == "print") {
      std::cout << "Shell PID: " << getpid() << std::endl;
      continue;
    }

    // Handle "help" command to display built-in commands
    if (input == "help") {
      std::cout << "\n"
                << "Type a valid shell command, with a space between each argument.\n"
                << "Built-in commands are:\n"
                << "'print' - show shell PID\n"
                << "'exit' - exit shell\n"
                << "\nHave a nice day :)\n"
                << std::endl;
      continue;
    }

    // Validate input
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
};