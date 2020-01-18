/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <iostream>

#include "command.hh"
#include "shell.hh"


Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _lastarg = std::string();
    _errCount = 0;
    _outCount = 0;
    _inCount = 0;
    _returnCode = 0;
    _background = false;
    _outappend = false;
    _errappend = false;
    _inExe = false;
    _badrule = false;
    _backgroundPid = -1;
    //_relativePath = NULL;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

    if ( _outFile == _errFile)
    {
        if (_outFile != NULL)
        {
            delete _outFile;
            _outFile = NULL;
            _errFile = NULL;
        }
    }
    else
    {
        if ( _outFile != NULL) {
            delete _outFile;
        }
        _outFile = NULL;

        if ( _inFile != NULL) {
            delete _inFile;
        }
        _inFile = NULL;

        if ( _errFile != NULL) {
            delete _errFile;
        }
        _errFile = NULL;
    }

    _background = false;
    _outappend = false;
    _errappend = false;
    _inExe = false;
    _badrule = false;
    _outCount = 0;
    _inCount = 0;
    _errCount = 0;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    /*if ( _simpleCommands.size() == 0 ) {
        Shell::prompt();
        return;
    }*/

    // Print contents of Command data structure
    //print();
    
    // struct sigaction signalAction;
    // signalAction.sa_handler = sigIntHandler;
    // sigemptyset(&signalAction.sa_mask);
    // //signalAction.sa_flags = SA_RESTART;
    // signalAction.sa_flags = SA_RESTART;
    // int error = sigaction(SIGINT, &signalAction, NULL );
    // if ( error ) {
    //     clear();
    //     if(isatty(0))
    //     {
    //         Shell::prompt();
    //     }
    //     exit(-1);
    // }

    //printf("%d %d %d\n", _outCount, _inCount, _errCount);
    _inExe = true;
    if(_outCount > 1 || _inCount > 1 || _errCount > 1)
    {
        printf("Ambiguous output redirect.\n");
        clear();
        Shell::prompt();      
        return;
    }
    if(_badrule)
    {
        printf("bad rule: \\ at the end of argument\n");
        clear();
        Shell::prompt();      
        return;
    }
    int errflag = O_CREAT|O_WRONLY|O_TRUNC;
    int outflag = O_CREAT|O_WRONLY|O_TRUNC;
    if(_outappend)
    {
        outflag = O_CREAT|O_WRONLY|O_APPEND;
    }
    if(_errappend)
    {
        errflag = O_CREAT|O_WRONLY|O_APPEND;
    }
    int tmpin = dup(0);
    int tmpout = dup(1);
    int tmperr = dup(2);
    int fdin;
    int fderr;
    //printf("%d, %d, %d\n",tmpin, tmpout, tmperr);
    if (_inFile != NULL)
    {
        fdin = open(_inFile->c_str(), O_RDONLY, 0666); //
        if(fdin < 0)
        {
            perror("inFile");
            clear();
            Shell::prompt();
            return;
        }
    }
    else
    {
        fdin = dup(tmpin);
    }

    if (_errFile != NULL)
    {
        fderr = open(_errFile->c_str(), errflag, 0666); //
    }
    else
    {
        fderr = dup(tmperr);
    }
    int fdout;
    int ret;
    int i = 0;
    dup2(fderr,2);
    close(fderr);
    // Add execution here
    
    for(auto & oneCMD : _simpleCommands)
    {
        if ( _simpleCommands.size() == 0 ) {
            clear();
            Shell::prompt();
            return;
        }
        if(strcmp(_simpleCommands.front()->_arguments.front(), "exit\0") == 0)
        {
            if(isatty(0))
            {
                printf("Thanks for using Joe's Shell!\n");
                printf("C U next time!\n");
            }
            close(fdin);
            dup2(tmpin,0);
            dup2(tmpout,1);
            dup2(tmperr,2);
            close(tmpin);
            close(tmpout);
            close(tmperr);
            clear();
            exit(0);
        }
        dup2(fdin, 0);        
        close(fdin);
        if (i == (int)(_simpleCommands.size())-1){
            // Last simple command
            if(_outFile != NULL){
                fdout=open(_outFile->c_str(), outflag, 0666); //
            }
            else {
                // Use default output
                fdout=dup(tmpout);
            }
            updateLastArg(oneCMD->_arguments[oneCMD->_arguments.size()-1]);
        }
        else {
            // Not last
            //simple command
            //create pipe
            int fdpipe[2];
            pipe(fdpipe);
            fdout=fdpipe[1];
            fdin=fdpipe[0];
        }// if/else
        // Redirect output
        dup2(fdout,1);
        close(fdout);

        if ( !strcmp( _simpleCommands[i]->_arguments[0], "setenv" ) ) {
            setenv(_simpleCommands[i]->_arguments[1], _simpleCommands[i]->_arguments[2], 1);
            clear();
            Shell::prompt();
            return;
        }
        if ( !strcmp( _simpleCommands[i]->_arguments[0], "unsetenv" ) ) {
            unsetenv(_simpleCommands[i]->_arguments[1]);
            clear();
            Shell::prompt();
            return;
        }
        if ( !strcmp( _simpleCommands[i]->_arguments[0], "cd" ) )
        {
            if(_simpleCommands[i]->_arguments.size() != 1)
            {
                int suc = chdir(_simpleCommands[i]->_arguments[1]);
                if(suc != 0)
                {
                    perror("cd: can't cd to notfound");
                }
            }
            else
            {
                chdir(getenv("HOME"));
            }
            clear();
            Shell::prompt();
            
            close(fdin);
            dup2(tmpin,0);
            dup2(tmpout,1);
            dup2(tmperr,2);
            close(tmpin);
            close(tmpout);
            close(tmperr);
	    return;
        }
        i++;
        ret = fork();
        if(ret == 0)
        {
            // For every simple command fork a new process
            // Setup i/o redirection
            // and call exec
            oneCMD->_arguments.push_back(NULL);
            char ** temp = oneCMD->_arguments.data();
            if (!strcmp(temp[0],"printenv")) {
                char **p=environ;
                while (*p!=NULL) {
                    printf("%s\n",*p);
                    p++;
                }
                exit(0);
            }        
            execvp(temp[0], temp);
            perror("execvp");
            exit(1);
        }
        else if(ret < 0)
        {
            perror("fork");
            exit(2);
        }
    }

    dup2(tmpin,0);
    dup2(tmpout,1);
    dup2(tmperr,2);
    close(tmpin);
    close(tmpout);
    close(tmperr);

    if(!_background)
    {
        int wstatus;
        waitpid(ret, &wstatus, 0);
        if (WIFEXITED(wstatus)) {
            _returnCode = WEXITSTATUS(wstatus);
        } else if (WIFSIGNALED(wstatus)) {
            _returnCode =  WTERMSIG(wstatus);
        } else if (WIFSTOPPED(wstatus)) {
            _returnCode =  WSTOPSIG(wstatus);
        }
    }
    else
    {
        _backgroundPid = ret;
    }

    clear();
    Shell::prompt();
    

    // Clear to prepare for next command
    // clear();

    // Print new prompt
    //Shell::prompt();
}

void Command::updateLastArg(char* lastarg)
{
    // if(_lastarg == NULL)
    // {
    //     _lastarg = new std::string(lastarg);
    // }
    // else
    // {
    //     delete _lastarg;
    //     _lastarg = new std::string(lastarg);
    // }
    _lastarg = std::string(lastarg);
}

SimpleCommand * Command::_currentSimpleCommand;
