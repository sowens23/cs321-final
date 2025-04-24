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

// Split user input into tokens
void parseInput(std::string &cmd, std::vector<std::string> &params) {
  std::istringstream stream(cmd);
  std::string arg;
  while (stream >> arg) {
    params.push_back(arg);
  }
}

int main() {
  
  std::string input;
  std::regex validInput("[a-zA-Z0-9-./_ ]+");

  // Read commands until the user quits
  while (true) {
    
    std::cout << "cs321% ";
    std::getline(std::cin, input);
    std::vector<std::string> args;

    // Check to ensure input is valid, then check for specific cmds
    if (std::regex_match(input, validInput) && input.length() <= 100) {

      parseInput(input, args);
      //args.push_back(nullptr);
      pid_t pid = fork();

      if (/* input == SIGINT or */ input == "exit") {
        break;
      }

      if (input == "print") {
        std::cout << pid << std::endl;
      }

      if (input == "help") {
        std::cout << "\n" 
        << "Type a valid shell command, with a space between each argument."
        << "\n Have a nice day :)"
        << std::endl;
      }

      // Child process code
      if (pid == 0) {

        //execvp(args[0], args.data());
        exit(1);
      
      // Parent process code
      } else {

        //wait();
      }

    // An invalid input character is detected
    } else if (!std::regex_match(input, validInput)) {

      std::cerr << "Error: Invalid input character(s). Valid characters are a-z, A-Z, 0-9, -, ., /, or _" 
      << std::endl;

    // Truncate input if 100-character limit has been exceeded
    } else {

      std::cerr << "Error: Inputs are limited to 100 characters."
      << std::endl;

      input = input.substr(0, 100);
    }
 }

  return 0;
}