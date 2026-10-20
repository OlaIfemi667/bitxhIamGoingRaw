#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


//-----define---------------

#define CTRL_KEY(k) ( (k) & 0x1f)

//-----------------------entity-------------------------------
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct{
  int len;
  char *start;
} String;

typedef struct {
  enum{
    TYPE_MONSTER,
    TYPE_HUMANOIDE,
    TYPE_PET
  }kind;
  String name;
  int isAlive;
}Entity;

void printString(String myString)
{
  for (i8 i = 0; i < myString.len; i++)
  {
    printf("%c", *(myString.start + i));
  }
  printf("\r\n");
}


void displayEntity(Entity myEntity)
{
  printf("My name is ");
  printString(myEntity.name);

  if (myEntity.kind == TYPE_MONSTER)
  {
    printString((String){.len = 8, .start = "BAKIMONO"});
  }

  else if (myEntity.kind == TYPE_HUMANOIDE)
  {
    printString((String){.len = 6, .start = "HOOMAN"});
  }
  else if (myEntity.kind == TYPE_PET)
  {
    printString((String){.len = 3, .start = "PET"});
  }
  else {
    printString((String){.len = 14, .start = "UNKNOWN ENTITY"});
  }

  if(myEntity.isAlive)
    printf("I'm ALIVE XD\n");
  else
   printf("RIP\n");
}

void killEntity(Entity *anEntity)
{
  anEntity->isAlive = false;
}



//-----------------------terminal-----------------------------


struct termios orig_termios;

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}


void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}


void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}



char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

/*** output ***/
void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}


/*** input ***/
void editorProcessKeypress(Entity *anEntity) {
  char c = editorReadKey();
  switch (c) {
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);

      exit(0);
      break;
    case 'k':
      killEntity(anEntity);
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);

      displayEntity(*anEntity);
  }
}







int main(void)
{
  /**String *a;
   * a->len = 3;
   * a->start = "ola";*/


  Entity theEntity = {
    .name = (String){.len = 8, .start = "Minouche"},
    .kind = TYPE_PET,
    .isAlive = 1
  };


  //----------Entrer en Raw Mode
  enableRawMode();
  editorRefreshScreen();

  displayEntity(theEntity);

  while (1) {
    editorProcessKeypress(&theEntity);
  }
  
  
  return 0;
}
