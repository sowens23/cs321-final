// Group Members: Ivy Swenson, Spencer Baysinger, Kohlby Vierthaler, and Jack Desrochers
// File Name: shell.cpp
// Date: 2025/04/23
// Description: This shell implementation handles command input, built-in functions, and
//              executes piped or background external commands using fork, execvp, and wait.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

struct Command {
  std::vector<std::string> args;
  bool background = false;
  bool pipe_to_next = false;
};

// =================== PARSE ===================
std::vector<Command> parseCommandLine(const std::string& inputLine) {
  std::vector<Command> commands;
  std::istringstream stream(inputLine);
  std::string token;
  Command currentCommand;

  std::string currentToken;
  char c;
  while (stream >> std::ws && stream.peek() != EOF) {
    c = stream.peek();
    if (c == '"' || c == '\'') {
      char quoteChar = stream.get();
      std::getline(stream, currentToken, quoteChar);
    } else if (c == ';' || c == '|' || c == '&') {
      if (!currentCommand.args.empty()) {
        if (c == '|') currentCommand.pipe_to_next = true;
        if (c == '&') currentCommand.background = true;
        commands.push_back(currentCommand);
        currentCommand = Command();
      }
      stream.get(); // Consume the delimiter
      continue;
    } else {
      stream >> currentToken;
    }

    if (!currentToken.empty()) {
      currentCommand.args.push_back(currentToken);
      currentToken.clear();
    }
  }

  if (!currentCommand.args.empty()) {
    commands.push_back(currentCommand);
  }

  return commands;
}

// =================== EXECUTOR ===================
class Executor {
public:
  void execute(const std::vector<Command>& commands) {
    size_t numCommands = commands.size();
    int pipefd[2];
    int prevPipeRead = -1;

    for (size_t i = 0; i < numCommands; ++i) {
      const auto& cmd = commands[i];
      if (cmd.args.empty()) continue;

      if (cmd.pipe_to_next && pipe(pipefd) == -1) {
        perror("pipe failed");
        return;
      }

      pid_t pid = fork();
      if (pid == 0) {
        // Child
        if (prevPipeRead != -1) {
          dup2(prevPipeRead, STDIN_FILENO);
          close(prevPipeRead);
        }
        if (cmd.pipe_to_next) {
          dup2(pipefd[1], STDOUT_FILENO);
          close(pipefd[0]);
          close(pipefd[1]);
        }

        std::vector<char*> argv;
        for (auto& arg : cmd.args) argv.push_back(const_cast<char*>(arg.c_str()));
        argv.push_back(nullptr);

        std::cerr << "Trying to run: " << argv[0] << std::endl; // Debug print
        execvp(argv[0], argv.data());
        perror("execvp failed");
        exit(1);
      } else if (pid > 0) {
        // Parent
        if (prevPipeRead != -1) close(prevPipeRead);
        if (cmd.pipe_to_next) {
          close(pipefd[1]);
          prevPipeRead = pipefd[0];
        } else {
          prevPipeRead = -1;
        }
        if (!cmd.background) waitpid(pid, nullptr, 0);
      } else {
        perror("fork failed");
      }
    }

    if (prevPipeRead != -1) close(prevPipeRead);
  }
};

// =================== MAIN ===================
int main() {
  std::string input;
  Executor executor;
  std::regex validInput(R"([a-zA-Z0-9._/';|&\"\\ -]*)");

  while (true) {
    std::cout << "cs321% ";
    std::getline(std::cin, input);

    if (std::cin.eof()) {
      std::cout << "\nExiting shell.\n";
      break;
    }

    if (input == "exit") break;
    if (input == "print") {
      std::cout << "Shell PID: " << getpid() << std::endl;
      continue;
    }
    if (input == "help") {
      std::cout << "Built-in commands:\n  print - show shell PID\n  exit - exit shell\n  help - show this message\n";
      continue;
    }
    if (input.substr(0, 3) == "cd ") {
      std::string path = input.substr(3);
      if (chdir(path.c_str()) != 0) perror("cd failed");
      continue;
    }
    if (!std::regex_match(input, validInput)) {
      std::cerr << "Invalid characters in input.\n";
      continue;
    }
    if (input.length() > 100) {
      std::cerr << "Input too long; truncated.\n";
      input = input.substr(0, 100);
    }

    auto commands = parseCommandLine(input);
    executor.execute(commands);
  }

  return 0;
}
