
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <cstring>
#include <string>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#include "y.tab.hh"
#include "simpleCommand.hh"
#include "shell.hh"

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

extern "C" char * read_line();


int mygetc(FILE * f) {
 static char *p;
 char ch;

 if (!isatty(0)) {
 // stdin is not a tty. Call real getc
   return getc(f);
 }

 // stdin is a tty. Call our read_line.
 if (p==NULL || *p == 0) {
   char * s = read_line();
   p = s;
 }

 ch = *p;
 p++;

 return ch;
}

#undef getc
#define getc(f) mygetc(f)


%}

%option noyywrap

%%

[s][o][u][r][c][e][ ][^ \t\n]* {
  std::string temp = std::string(yytext + 7);
  FILE * fptr = fopen(temp.c_str(), "r");
  YY_BUFFER_STATE buffer = yy_create_buffer(fptr, YY_BUF_SIZE);
  yypush_buffer_state(buffer);
  yyparse();
  yypop_buffer_state();
  fclose(fptr);
}

[^ \t\n]*[$][{][^ \t\n]*[}][^ \t\n]* {
  std::string temp = std::string(yytext);
  //temp = temp.substr(2, temp.length()-3);
  std::string buffer = std::string();
  int firstend = -1;
  while(1)
  {
    int startpoint = firstend;
    int i;
    bool flag = false;
    for(i = firstend + 1; i < temp.length(); i++)
    {
      if(temp[i] == 125)
      {
        firstend = i;
        break;
      }
    }
    if(i == temp.length())
    {
      flag = true;
    }
    for(i = startpoint + 1; i < firstend; i++)
    {
      if(temp[i] == '$' && temp[i+1] == 123)
      {
        std::string val_name = temp.substr(i + 2, firstend - i - 2);
        if(val_name.compare("SHELL") == 0)
        {
          //buffer += realpath(Shell::_currentCommand._relative_path->c_str(), NULL);
          buffer += realpath("../shell", NULL);
        }
        else if(val_name.compare("$") == 0)
        {
          buffer += std::to_string(Shell::_currentCommand._pid);
        }
        else if(val_name.compare("?") == 0)
        {
          buffer += std::to_string(Shell::_currentCommand._returnCode);
        }
        else if(val_name.compare("_") == 0)
        {
          buffer += (Shell::_currentCommand._lastarg);
        }
        else if(val_name.compare("!") == 0)
        {
          buffer += std::to_string(Shell::_currentCommand._backgroundPid);
        }
        else
        {
          buffer += getenv(val_name.c_str());
        }
        break;
      }
      buffer += temp[i];
    }
    if(flag)
    {
      break;
    }
  }
  if(firstend != temp.length() - 1)
  {
    for(int i = firstend + 1; i < temp.length(); i++)
    {
      buffer += temp[i];
    }
  }
  yylval.cpp_string = new std::string(buffer.c_str());
  return WORD;
}

[$][(][^\n]*[)] {
  std::string * temp = new std::string(yytext);
  std::string input = std::string(yytext);
  if(temp->length() == 3)
  {
    input = std::string();
  }
  else
  {
    input = std::string(temp->substr(2, temp->length()-3));
  }
  int pin[2], pout[2];
  pipe(pin); 
  pipe(pout);
  input += "\nexit\n\0";
  write(pin[1], input.c_str(), input.length());
  close(pin[1]);

  int ret = fork();

  if(ret == 0)
  {
    dup2(pin[0], 0);
    close(pin[0]);
    dup2(pout[1],1);
    close(pout[1]);
    execvp("/proc/self/exe", NULL);
    _exit(1);
  }
  else if (ret < 0)
  {
    perror("SubShell fork");
    exit(1);
  }

  close(pout[1]);
  char buffer;
  std::string out = std::string();
  while(read(pout[0], &buffer, 1) > 0)
  {
    if(buffer == '\n')
    {
      out += ' ';
    }
    else
    {
      out += buffer;
    }
  }
  close(pout[0]);
  for(int i = out.length() - 1; i>=0; i--)
  {
    myunputc(out[i]);
  }
  delete temp;
}

[^\n\t ]*[\\]+[^\n\t ]* {
  std::string * temp = new std::string(yytext);
  yylval.cpp_string = new std::string(yytext);
  int j = 0;
  for(int i = 0; i < temp->length(); i++)
  {
    if(temp->at(i) == '\\')
    {
      if(i != temp->length() - 1)
      {
        (*yylval.cpp_string)[j] = temp->at(i+1);
        i++;
        j++;
        continue;
      }
      break;
    }
    (*yylval.cpp_string)[j] = temp->at(i);
    j++;
  }
  (*yylval.cpp_string)[j] = '\0';
  delete temp;
  return WORD;
}

[\A\"][^\"]*[\Z\"] {
  std::string * temp = new std::string(yytext);
  if(temp->length() == 2)
  {
    yylval.cpp_string = new std::string();
  }
  else
  {
    yylval.cpp_string = new std::string(temp->substr(1, temp->length()-2));
  }
  delete temp;
  return WORD;
}

(~[^ \t\n~]*|~) {
  std::string temp = std::string(yytext);
  if(temp.length() == 1)
  {
    yylval.cpp_string = new std::string(getenv("HOME"));
    return WORD;
  }
  else if(temp[1] != '/')
  {
    yylval.cpp_string = new std::string("/homes/" + temp.substr(1, temp.length() - 1));
    return WORD;
  }
  else
  {
    yylval.cpp_string = new std::string("/homes/" + temp.substr(2, temp.length() - 2));
    return WORD;
  }
}

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

">>&" {
  return GREATGREATAMPERSAND;
}

">>" {
  return GREATGREAT;
}

">&" {
  return GREATAMPERSAND;
}

"2>" {
  return TWOGREAT;
}

">" {
  return GREAT;
}

"<" {
  return LESS;
}

"|" {
  return PIPE;
}

"&" { 
  return AMPERSAND;
}

[^ \t\n|>][^ \t\n|>]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}
