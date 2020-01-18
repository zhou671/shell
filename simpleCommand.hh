#ifndef simplecommand_hh
#define simplecommand_hh

#include <string>
#include <vector>

struct SimpleCommand {

  // Simple command is simply a vector of strings
  //std::vector<std::string *> _arguments; // mod
  std::vector<char *> _arguments;
  //char** data;
  //std::vector<char *> argBuf;

  SimpleCommand();
  ~SimpleCommand();
  void insertArgument( std::string * argument ); // mod
  void print();
};

#endif
