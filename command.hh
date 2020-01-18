#ifndef command_hh
#define command_hh

#include "simpleCommand.hh"

// Command Data Structure

struct Command {
  std::vector<SimpleCommand *> _simpleCommands;
  std::string * _outFile;
  std::string * _inFile;
  std::string * _errFile;
  //std::string _relative_path;
  bool _background;
  bool _outappend;
  bool _errappend;
  bool _inExe;
  bool _badrule;
  int _errCount;
  int _outCount;
  int _inCount;
  int _pid;
  //std::string * _relativePath;
  int _returnCode;
  std::string _lastarg;
  int _backgroundPid;

  Command();
  void insertSimpleCommand( SimpleCommand * simpleCommand );

  void clear();
  void print();
  void execute();
  void updateLastArg(char* lastarg);

  static SimpleCommand *_currentSimpleCommand;
};

#endif
