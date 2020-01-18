/*
 * CS252: Systems Programming
 * Purdue University
 * Example that shows how to read one line with simple editing
 * using raw terminal.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_BUFFER_LINE 2048
#define MAX_HIST 32

extern void tty_raw_mode(void);

// Buffer where line is stored
int line_length;
char line_buffer[MAX_BUFFER_LINE];
int curser;
int adjust;

// Simple history array
// This history does not change. 
// Yours have to be updated.
int history_head = 0;
int history_tail = 31;
int history_curser;
char history [32][MAX_BUFFER_LINE] = {
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0",
  "\0"
};
int history_length = sizeof(history)/sizeof(char *);

void read_line_print_usage()
{
  char * usage = "\n"
    " ctrl-?       Print usage\n"
    " Backspace    Deletes last character\n"
    " up arrow     See last command in the history\n";

  write(1, usage, strlen(usage));
}

/* 
 * Input a line with some basic editing.
 */
char * read_line() {

	struct termios tty_attr;
  struct termios reset;
	tcgetattr(0,&tty_attr);
  reset = tty_attr;
  // Set terminal in raw mode
  tty_raw_mode();

  line_length = 0;
  curser = 0;
  history_curser = history_head;
  line_buffer[0] = '\0';
  // Read one line until enter is typed
  while (1) {

    // Read one character in raw mode.
    char ch;
    read(0, &ch, 1);

    if (ch>=32 && ch != 127) {
      // It is a printable character. 

      // Do echo
      //write(1,&ch,1);

      // If max number of character reached return.
      if (line_length==MAX_BUFFER_LINE-2) break; 

      // add char to buffer.
      if(curser == line_length)
      {
        write(1,&ch,1);
        line_buffer[curser]=ch;
        line_length++;
        curser++;
      }
      else
      {
        for (int i = line_length; i > curser; i--)
        {
          line_buffer[i] = line_buffer[i - 1];
        }        
        line_buffer[curser] = ch;
        for(int i = curser; i < line_length + 1; i++)
        {
          write(1, &(line_buffer[i]), 1);
        }
        char temp = 8;
        for(int i = curser; i < line_length; i++)
        {
          write(1,&temp,1);
        }
        line_length++;
        curser++;
      }
      history_curser = history_head;
    }
    else if (ch==10) {
      // <Enter> was typed. Return line
      
      // Print newline
      write(1,&ch,1);

      break;
    }
    else if (ch == 31) {
      // ctrl-?
      read_line_print_usage();
      line_buffer[0]=0;
      //break;
    }
    else if (ch == 5)
    {
      // ctrl e
      for(int i = curser; i < line_length; i++)
      {
        write(1, &(line_buffer[i]), 1);
      }
      curser = line_length;
    }
    else if (ch == 1)
    {
      // ctrl a
      char temp = 8;
      for(int i = 0; i < curser; i++)
      {
        write(1, &temp, 1);
      }
      curser = 0;
    }
    else if (ch == 4)
    {
      // ctrl d
      if(line_length == 0)
      {
        continue;
      }
      if(line_length == curser)
      {
        continue;
      }
      char temp;
      for (int i = curser; i < line_length; i++)
      {
        line_buffer[i] = line_buffer[i + 1];
      }
      line_buffer[line_length - 1] = '\0';
      for(int i = curser; i < line_length - 1; i++)
      {
        write(1, &(line_buffer[i]), 1);
      }
      temp = ' ';
      write(1,&temp,1);
      temp = 8;
      for(int i = curser; i < line_length; i++)
      {
        write(1,&temp,1);
      }
      line_length--;
      history_curser = history_head;
    }
    else if (ch == 8 || ch == 127) {
      // <backspace> was typed. Remove previous character read.
      if(curser == 0)
      {
        continue;
      }

      if(line_length == curser)
      {
        ch = 8;
        write(1,&ch,1);
        ch = ' ';
        write(1,&ch,1);
        ch = 8;
        write(1,&ch,1);
        line_length--;
        curser--;
        line_buffer[line_length] = '\0';
      }
      else
      {
        char temp;
        for (int i = curser - 1; i < line_length; i++)
        {
          line_buffer[i] = line_buffer[i + 1];
        }
        line_buffer[line_length - 1] = '\0';
        temp = 8;
        write(1,&temp,1);
        for(int i = curser - 1; i < line_length - 1; i++)
        {
          write(1, &(line_buffer[i]), 1);
        }
        temp = ' ';
        write(1,&temp,1);
        temp = 8;
        for(int i = curser - 1; i < line_length; i++)
        {
          write(1,&temp,1);
        }
        line_length--;
        curser--;
      }
      history_curser = history_head;
    }
    else if (ch==27) {
      // Escape sequence. Read two chars more
      //
      // HINT: Use the program "keyboard-example" to
      // see the ascii code for the different chars typed.
      //
      char ch1; 
      char ch2;
      read(0, &ch1, 1);
      read(0, &ch2, 1);
      if (ch1==91 && ch2==65) {
        // Up arrow. Print next line in history.

        // Erase old line
        // Print backspaces
        if(strlen(history[(history_curser + 31)%32]) == 0)
        {
          continue;
        }
        history_curser = (history_curser + 31)%32;
        int i = 0;
        for(i = curser; i <line_length; i++)
        {
          ch = ' ';
          write(1, &ch, 1);
        }
        for (i =0; i < line_length; i++) {
          ch = 8;
          write(1,&ch,1);
        }

        // Print spaces on top
        for (i =0; i < line_length; i++) {
          ch = ' ';
          write(1,&ch,1);
        }

        // Print backspaces
        for (i =0; i < line_length; i++) {
          ch = 8;
          write(1,&ch,1);
        }	

        // Copy line from history
        strcpy(line_buffer, history[history_curser]);
        line_length = strlen(line_buffer);
        curser = line_length;

        // echo line
        write(1, line_buffer, line_length);
      }
      if (ch1==91 && ch2==66)
      {
        // Down arrow.
        if(strlen(history[(history_curser + 1)%32]) == 0)
        {
          continue;
        }
        history_curser = (history_curser + 1)%32;
        int i = 0;
        for(i = curser; i <line_length; i++)
        {
          ch = ' ';
          write(1, &ch, 1);
        }
        for (i =0; i < line_length; i++) {
          ch = 8;
          write(1,&ch,1);
        }

        // Print spaces on top
        for (i =0; i < line_length; i++) {
          ch = ' ';
          write(1,&ch,1);
        }

        // Print backspaces
        for (i =0; i < line_length; i++) {
          ch = 8;
          write(1,&ch,1);
        }	

        // Copy line from history
        strcpy(line_buffer, history[history_curser]);
        line_length = strlen(line_buffer);
        curser = line_length;

        // echo line
        write(1, line_buffer, line_length);
      }
      else if(ch1==91 && ch2==68)
      {
        //left
        if(curser == 0)
        {
          continue;
        }
        // char temp;
        // temp = 27;
        // write(1, &temp, 1);
        // temp = 91;
        // write(1, &temp, 1);
        // temp = 68;
        // write(1, &temp, 1);
        char temp;
        temp = 8;
        write(1,&temp,1);
        curser--;
      }
      else if(ch1==91 && ch2==67)
      {
        //right
        if(curser == line_length)
        {
          continue;
        }
        // char temp;
        // temp = 27;
        // write(1, &temp, 1);
        // temp = 91;
        // write(1, &temp, 1);
        // temp = 67;
        // write(1, &temp, 1);
        write(1,&line_buffer[curser],1);
        curser++;
      }
    }

  }

  // Add eol and null char at the end of string
  
  //history_index=(history_index+1)%MAX_HIST;
  if(strlen(line_buffer) != 0)
  {
    if(history_head == history_tail)
    {
      history[history_head][0] = '\0';
      history_tail = (history_tail + 1) % 32;
    }
    strcpy(history[history_head], line_buffer);
    history_head=(history_head+1)%32;
  }
  line_buffer[line_length]=10;
  line_length++;
  line_buffer[line_length]=0;
  tcsetattr(0,TCSANOW,&reset);
  return line_buffer;
}

