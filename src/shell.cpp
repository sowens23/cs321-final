// Group Members: Ivy Swenson, Spencer Baysinger, Kohlby Vierthaler, and Jack D.
// File Name: shell.cpp
// Date: 2025/04/19
// Description: This file contains the implementation of the shell class, which is responsible for managing the shell's command line interface. 
//              It handles user input, command execution, and output display. The shell class also manages the history of commands entered by 
//              the user and provides functionality for navigating through the command history.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <signal.h>


void parseInput(std::string &cmd, std::vector<std::string> &params) {
  std::istringstream stream(cmd);
  std::string arg;
  while (stream >> arg) {
    params.push_back(arg);
  }
}

int main() {
  
  std::string input;

  // Read commands until the user quits
  while (true) {

    std::cout << "<Enter a command> ~#";
    std::getline(std::cin, input);
    std::vector<std::string> args;

    if (/* input == SIGINT or */ input == "exit") {
      break;
    }

    if (input == "print") {
      std::cout << "Print process pid here." << std::endl;
    }

    if (input == "help") {
      std::cout << "\n" 
      << "Type a valid shell command, with a space between each argument."
      << "\n Have a nice day :)"
      << std::endl;
    }
  }


  return 0;
}