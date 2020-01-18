#include <cstdio>
#include <cstdlib>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string>
#include <cstring>
#include <iostream>

#include "simpleCommand.hh"

SimpleCommand::SimpleCommand() {
  //_arguments = std::vector<std::string *>(); //mod
  _arguments = std::vector<char *>();
  //argBuf = std::vector<char*>();
}

SimpleCommand::~SimpleCommand() {
  // iterate over all the arguments and delete them
  for (auto & arg : _arguments) {
      free(arg);
  }
  
}

void SimpleCommand::insertArgument(std::string * argument ) { //mod
  // simply add the argument to the vector
  //char * temp = new char[argument->length()+1];
  char * temp = (char*)malloc(sizeof(char) * (argument->length()+1));
  strcpy(temp, argument->c_str());
  temp[argument->length()] = '\0';
  _arguments.push_back(temp);
  //argBuf.push_back((char*) argument);
}

// Print out the simple command
void SimpleCommand::print() {
  for (auto & arg : _arguments) {
    std::cout << "\"" << arg << "\" \t";
  }
  // effectively the same as printf("\n\n");
  std::cout << std::endl;
}
