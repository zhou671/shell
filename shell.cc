#include <cstdio>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <regex>
#include <dirent.h>
#include <algorithm>
#include <string>
#include <ctype.h>
#include "shell.hh"

int yyparse(void);

void Shell::prompt() {
  if(isatty(0))
  {
    printf("myshell>");
    fflush(stdout);
  }
}

void Shell::expandWildcardsIfNecessary(std::string * argument, SimpleCommand * simplecommand)
{
  bool nece = false;
  std::string reg = std::string();
  for(int i = 0; i < argument->length(); i++)
  {
    if((*argument)[i] == '?')
    {
      nece = true;
      reg += ".";
    }
    else if ((*argument)[i] == '*')
    {
      reg += ".*";
      nece = true;
    }
    else if ((*argument)[i] == '.')
    {
      reg += "\\.";
    }
    else
    {
      reg += (*argument)[i];
    }
  }
  if(!nece)
  {
    simplecommand->insertArgument(argument);
    return;
  }
  std::vector<std::string> * results = new std::vector<std::string>();
  expandWildcards(std::string(), *argument, results);
  if(results->empty())
  {
    simplecommand->insertArgument(argument);
    return;
  }
  std::sort(results->begin(), results->end(), compareStr);
  for(auto & oneARG : *results)
  {
    simplecommand->insertArgument(&oneARG);
  }
  delete results;
}

bool Shell::compareStr(std::string str1, std::string str2)
{
  // for(int i = 0; i < str1.length(); i++)
  // {
  //   str1[i] = tolower(str1[i]);
  // }
  // for(int i = 0; i < str2.length(); i++)
  // {
  //   str2[i] = tolower(str2[i]);
  // }
  if(str1.compare(str2) < 0)
  {
    return true;
  }
  else
  {
    return false;
  }
  //return str1.compare(str2);
}

void Shell::expandWildcards(std::string prefix, std::string postfix, std::vector<std::string> * results)
{
  if(postfix.length() == 0)
  {
    results->push_back(prefix);
    return;
  }
  std::size_t slash = postfix.find("/");
  std::string component;
  std::string endslash = std::string();
  if(slash == std::string::npos)
  {
    component = postfix;
    postfix = std::string();
  }
  else
  {
    component = postfix.substr(0, slash);
    postfix = postfix.substr(slash+1, postfix.length() - slash - 1);
    endslash = std::string("/");
  }
  char firstchar = component[0];
  if(!complieReg(&component))
  {
    expandWildcards(prefix + component + endslash, postfix, results);
    return;
  }
  std::string dir;
  if(prefix.length() != 0)
  {
    dir = std::string(prefix);
  }
  else
  {
    dir = std::string(".");
  }
  struct dirent * ent;
  DIR * d=opendir(dir.c_str());
  if (d==NULL) 
  {
    return;
  }
  while (( ent = readdir(d))!= NULL) {
    std::string filename = std::string(ent->d_name);
    std::regex r(component);
    if (std::regex_match(filename, r)) {
      if(firstchar != '.')
      {
        if((ent->d_name)[0] != '.')
        {
          expandWildcards(prefix+filename + endslash,postfix, results);
        }
      }
      else
      {
        expandWildcards(prefix+filename + endslash,postfix, results);
      }
    }
    // if(filename[filename.length() - 1] == '/')
    // {
    //   printf("%s\n", filename);
    // }
  }
  closedir(d);
}

bool Shell::complieReg(std::string *wildcard)
{
  bool nece = false;
  std::string reg = std::string();
  for(int i = 0; i < wildcard->length(); i++)
  {
    if((*wildcard)[i] == '?')
    {
      nece = true;
      reg += ".";
    }
    else if ((*wildcard)[i] == '*')
    {
      reg += ".*";
      nece = true;
    }
    else if ((*wildcard)[i] == '.')
    {
      reg += "\\.";
    }
    else
    {
      reg += (*wildcard)[i];
    }
  }
  //reg += "$";
  if(nece)
  {
    // if(reg[reg.length()-1] == '/')
    // {
    //   reg = reg.substr(0, reg.length()-2);
    // }
    *wildcard = reg;
  }
  return nece;
}

int main(int argc, char **argv) {
  //Shell::_currentCommand._relativePath = new std::string(argv[0]);
  Shell::_currentCommand._pid = getpid();
  struct sigaction signalAction1;
  signalAction1.sa_handler = sigIntHandler;
  sigemptyset(&signalAction1.sa_mask);
  signalAction1.sa_flags = SA_RESTART;
  int error = sigaction(SIGINT, &signalAction1, NULL );
  if ( error ) {
    perror("ctrl c sig");
    Shell::_currentCommand.clear();
    exit(-1);
  }
  struct sigaction signalAction2;
  signalAction2.sa_handler = killzombie;
  sigemptyset(&signalAction2.sa_mask);
  signalAction2.sa_flags = SA_RESTART;
  error = sigaction(SIGCHLD, &signalAction2, NULL );
  if ( error ) {
    perror( "zombie sig" );
    Shell::_currentCommand.clear();
    exit( -1 );
  }
  Shell::prompt();
  yyparse();
}

extern "C" void sigIntHandler(int sig)
{
  if(!Shell::_currentCommand._inExe)
  {
    Shell::_currentCommand.clear();
    printf("\n");
    Shell::prompt();
  }
}

extern "C" void killzombie(int sig)
{
  while(waitpid(-1, NULL, WNOHANG) > 0);
}
Command Shell::_currentCommand;


