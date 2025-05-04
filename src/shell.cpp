// Group Members: Ivy Swenson, Spencer Baysinger, Kohlby Vierthaler, and Jack Desrochers
// File Name: shell.cpp
// Date: 2025/04/23
// Description: This file contains the implementation of the shell class, which is responsible for managing the shell's command line interface.
//              It handles user input, built-in commands, external command execution, and output display.
//              The shell class also manages the history of commands entered by the user.

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

// ========================= Built-in Command Enum =============================
enum class BuiltinType { EXIT, PRINT, HELP, INVALID };

BuiltinType identifyBuiltin(const std::string& cmd) {
  if (cmd == "exit") return BuiltinType::EXIT;
  if (cmd == "print") return BuiltinType::PRINT;
  if (cmd == "help") return BuiltinType::HELP;
  return BuiltinType::INVALID;
}

bool runBuiltin(BuiltinType btype, bool& running) {
  switch (btype) {
    case BuiltinType::EXIT:
      running = false;
      return true;
    case BuiltinType::PRINT:
      std::cout << "Shell PID: " << getpid() << std::endl;
      return true;
    case BuiltinType::HELP:
      std::cout << "\nBuilt-in commands:\n"
                << "  exit  - Exit the shell\n"
                << "  print - Show current shell PID\n"
                << "  help  - Display help info\n" << std::endl;
      return true;
    case BuiltinType::INVALID:
      return false;
  }
  return false;
}

// ========================= Parse Command Line into Token-Groups =============================
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
  bool running = true;

  while (running) {
    std::cout << "cs321% ";
    std::getline(std::cin, input);

    if (!std::regex_match(input, validInput)) {
      std::cerr << "Error: Invalid input character(s). Only a-z, A-Z, 0-9, -, ., /, _, space, ;, & are allowed.\n";
      continue;
    }

    if (input.length() > 100) {
      std::cerr << "Error: Input limited to 100 characters.\n";
      input = input.substr(0, 100);
    }

    auto commands = parseCommandLine(input);

    // Built-in dispatch before exec
    if (!commands.empty()) {
      BuiltinType btype = identifyBuiltin(commands[0].args[0]);
      if (runBuiltin(btype, running)) continue;
    }

    executor.execute(commands);
  }

  return 0;
}
