#ifndef shell_hh
#define shell_hh

#include "command.hh"

struct Shell {

  static void prompt();
  static void expandWildcardsIfNecessary(std::string * argument, SimpleCommand * simplecommand);
  static void expandWildcards(std::string prefix, std::string postfix, std::vector<std::string> * results);
  static bool compareStr(std::string str1, std::string str2);
  static bool complieReg(std::string *wildcard);
  static Command _currentCommand;
};

extern "C" void sigIntHandler(int sig);
extern "C" void killzombie(int sig);
//bool compareStr(std::string str1, std::string str2);

#endif