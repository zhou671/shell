
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%code requires 
{
#include <string>

#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREATGREATAMPERSAND GREATAMPERSAND GREATGREAT TWOGREAT GREAT NEWLINE PIPE AMPERSAND LESS

%{
//#define yylex yylex
#include <cstdio>
#include "shell.hh"

void yyerror(const char * s);
int yylex();

%}

%%

goal:
  commands
  ;

commands:
  command
  | commands command
  ;

command: simple_command
       ;

simple_command:	
  pipe_list iomodifier_opt_list background_optional NEWLINE {  
  //pipe_list iomodifier_opt_list NEWLINE {
    //printf("   Yacc: Execute command\n");
    //printf("   Yacc: Execute command\n");
    Shell::_currentCommand.execute();
  }
  | NEWLINE 
  | error NEWLINE { yyerrok; }
  ;

pipe_list:
  pipe_list PIPE command_and_args
  | command_and_args
  ;

command_and_args:
  command_word argument_list {
    Shell::_currentCommand.
    insertSimpleCommand( Command::_currentSimpleCommand );
  }
  ;

argument_list:
  argument_list argument
  | /* can be empty */
  ;

argument:
  WORD {
    //printf("   Yacc: insert argument \"%s\"\n", $1->c_str());
    //Command::_currentSimpleCommand->insertArgument( $1 );
    Shell::expandWildcardsIfNecessary($1, Command::_currentSimpleCommand);
  }
  ;

command_word:
  WORD {
    //printf("   Yacc: insert command \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  ;

background_optional:
  AMPERSAND {
    //printf("   Yacc: insert background\n");
    Shell::_currentCommand._background = true;
  }
  | /* can be empty */
  ;

iomodifier_opt_list:
  iomodifier_opt_list iomodifier_opt
  | /* can be empty */
  ;

iomodifier_opt:
  GREATGREAT WORD {
    //printf("   Yacc: insert appendout \"%s\"\n", $2->c_str());
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._outappend = true;
    Shell::_currentCommand._outCount++;
  }
  | GREATGREATAMPERSAND WORD {
    //printf("   Yacc: insert appendoutAerr \"%s\"\n", $2->c_str());
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
    Shell::_currentCommand._outappend = true;
    Shell::_currentCommand._errappend = true;
    Shell::_currentCommand._outCount++;
    Shell::_currentCommand._errCount++;
  }
  | GREATAMPERSAND WORD {
    //printf("   Yacc: insert outAerr \"%s\"\n", $2->c_str());  
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
    Shell::_currentCommand._outCount++;
    Shell::_currentCommand._errCount++;
  }
  | GREAT WORD {
    //printf("   Yacc: insert output \"%s\"\n", $2->c_str());
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._outCount++;
  }
  | TWOGREAT WORD {  
    //printf("   Yacc: insert err \"%s\"\n", $2->c_str());
    Shell::_currentCommand._errFile = $2;
    Shell::_currentCommand._errCount++;
  }
  | LESS WORD {
    //printf("   Yacc: insert input \"%s\"\n", $2->c_str());
    Shell::_currentCommand._inFile = $2;
    Shell::_currentCommand._inCount++;
  }
  ;
%%

void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}

#if 0
main()
{
  yyparse();
}
#endif
