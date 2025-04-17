// Group Members: Ivy Swenson, Spencer Baysinger, Kolby, and Jack D.
// File Name: shell.cpp
// Date: 2025/04/16
// Description: This file contains the implementation of the shell class, which is responsible for managing the shell's command line interface. 
//              It handles user input, command execution, and output display. The shell class also manages the history of commands entered by 
//              the user and provides functionality for navigating through the command history.

// Phase Zero Notes
// Coded by: Spencer
// Compile using: "make"
// Run using: "make run" or "./shell"
// Usage Notes: Skeleton shell simply echoes back user input and captures EOF input. 
//              Ctrl+D to exit the shell.

#include <iostream>
#include <string>

using namespace std;

int main() {
  string input;

  while(true) {
    cout << "cs321% "; // Prompt for user input
    getline(cin, input); // Read a line of input from the user

    if (cin.eof()) { // Check for EOF (Ctrl+D)
      cout << endl << "EOF (Ctrl+D) detected. Exiting shell..." << endl;
      break;
    }

    cout << "You entered: " << input << endl; // Echo the input back to the user
  }

  return 0;
}