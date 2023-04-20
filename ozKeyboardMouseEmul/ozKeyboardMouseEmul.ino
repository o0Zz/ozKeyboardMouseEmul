#include <SoftwareSerial.h>

const byte rxPin = 2; //D2
const byte txPin = 3; //D3

SoftwareSerial mySerial (rxPin, txPin);

#define SPTR_MAX_SIZE   10
#define CMD_MAX_SIZE    128

/* ---------------------------------------------- */

void displayHelp()
{
  mySerial.println("Commands:");
  mySerial.println("    mouse move <X> <Y>");
  mySerial.println("    mouse click");
  mySerial.println("    mouse dblclick");
  mySerial.println("    mouse press");
  mySerial.println("    mouse release");
  mySerial.println("    key <KEYS>");
  mySerial.println("    str <STRING>");
  mySerial.println("");
  mySerial.println("KEYS: Space separated (Max 8), it can be:");
  mySerial.println("    MODIFIERS: CTRL, SHIFT, ALT");
  mySerial.println("    KEYSS: UP, DOWN, LEFT, RIGHT, ENTER, ESC, ...");
}

/* ---------------------------------------------- */

typedef struct key_mapping_s
{
  const char *key_str;
  uint16_t value;
}key_mapping;

key_mapping keys_modifier_map[] = 
{
  {"CTRL",  MODIFIERKEY_CTRL},
  {"SHIFT", MODIFIERKEY_SHIFT},
  {"ALT",   MODIFIERKEY_ALT}
};

key_mapping keys_touch_map[] = 
{
  {"ENTER",     KEY_ENTER},
  {"ESC",       KEY_ESC},
  {"BACKSPACE", KEY_BACKSPACE},
  {"TAB",       KEY_TAB},
  {"UP",        KEY_UP},
  {"DOWN",      KEY_DOWN},
  {"RIGHT",     KEY_RIGHT},
  {"LEFT",      KEY_LEFT},
  {"F1",        KEY_F1},
  {"F2",        KEY_F2},
  {"F3",        KEY_F3},
  {"F4",        KEY_F4},
  {"F5",        KEY_F5},
  {"F6",        KEY_F6},
  {"F7",        KEY_F7},
  {"F8",        KEY_F8},
  {"F9",        KEY_F9},
  {"F10",       KEY_F10},
  {"F11",       KEY_F11},
  {"F12",       KEY_F12},
  {"NUM_LOCK",  KEY_NUM_LOCK},
  {"INSERT",    KEY_INSERT},
  {"HOME",      KEY_HOME},
  {"PAGE_UP",   KEY_PAGE_UP},
  {"DELETE",    KEY_DELETE},
  {"END",       KEY_END},
  {"PAGE_DOWN", KEY_PAGE_DOWN}
};

/* ---------------------------------------------- */

void setKeyboard(char **keys, int len)
{
  uint16_t key_modifiers = 0x0000;
  
  for (int i=0; i<len; i++)
  {
    for (size_t k=0; k<sizeof(keys_modifier_map)/sizeof(key_mapping); k++)
    {
      if (strcmp(keys[i], keys_modifier_map[k].key_str) == 0)
      {
        key_modifiers |= keys_modifier_map[k].value;
      }
    }

    for (size_t k=0; k<sizeof(keys_touch_map)/sizeof(key_mapping); k++)
    {
      if (strcmp(keys[i], keys_touch_map[k].key_str) == 0)
      {
        Keyboard.set_modifier(key_modifiers);
        Keyboard.send_now();
        
        Keyboard.set_key1(keys_touch_map[k].value);
        Keyboard.send_now();
        
        delay(10);

        Keyboard.set_modifier(0);
        Keyboard.set_key1(0);
        Keyboard.send_now();
      }
    }
  }
}

/* ---------------------------------------------- */

void setMousePtr(long x, long y)
{
  //Make sure mouse is top left corner
  for (int i=0; i<64; i++) //64*128=8192 => Handle screen lower than 8192x8192
    Mouse.move(-128, -128);

  long x_iter = (x/127);
  long y_iter = (y/127);

  for (long i=0; i<y_iter; i++)
      Mouse.move(0, 127);

  for (long i=0; i<x_iter; i++)
      Mouse.move(127, 0);
  
  Mouse.move((x % 127), (y % 127));
}

/* ---------------------------------------------- */

int splitStr (char *str, char *p[SPTR_MAX_SIZE])
{
    int  n = 0;
    
    *p++ = strtok (str, " ");
    for (n = 1; NULL != (*p++ = strtok (NULL, " ")); n++)
        if (n == SPTR_MAX_SIZE)
            break;

    return n;
}

/* ---------------------------------------------- */

void handleCommand(char *str)
{
  char  cmd[CMD_MAX_SIZE];
  char *str_splitted[SPTR_MAX_SIZE];
  
  strncpy(cmd, str, sizeof(cmd));
  
  int strCount = splitStr(str, str_splitted);

  if (strCount == 0)
  {
    mySerial.println("ERR=1");
    return;
  }

  if (strcmp(str_splitted[0], "key") == 0)
  {
    if (strCount < 2)
    {
      mySerial.println("ERR=2");
      return;
    }

    setKeyboard(&str_splitted[1], strCount - 1);
    mySerial.println("OK");
  }
  else if (strcmp(str_splitted[0], "str") == 0)
  {
    if (strCount < 2)
    {
      mySerial.println("ERR=2");
      return;
    }

    Keyboard.print(&cmd[4]); 
    mySerial.println("OK");
  }
  else if (strcmp(str_splitted[0], "mouse") == 0)
  {
    if (strCount < 2)
    {
      mySerial.println("ERR=2");
      return;
    }
      
    if (strcmp(str_splitted[1], "move") == 0)
    {
      if (strCount < 4)
      {
        mySerial.println("ERR=2");
        return;
      }
      
      setMousePtr(atol(str_splitted[2]), atol(str_splitted[3]));
      mySerial.println("OK");
    }
    else if (strcmp(str_splitted[1], "click") == 0)
    {
      Mouse.click();
      mySerial.println("OK");
    }
    else if (strcmp(str_splitted[1], "dblclick") == 0)
    {
      Mouse.click();
      delay(100);
      Mouse.click();
      mySerial.println("OK");
    }
    else if (strcmp(str_splitted[1], "press") == 0)
    {
      Mouse.press();
      mySerial.println("OK");
    }
    else if (strcmp(str_splitted[1], "release") == 0)
    {
      Mouse.release();
      mySerial.println("OK");
    }
    else
    {
      mySerial.println("ERR=4");
      mySerial.println(str_splitted[1]);
    }  
  }
  else if (strcmp(str_splitted[0], "version") == 0)
  {
    mySerial.println("OK=1.0.0");
  }
  else if (strcmp(str_splitted[0], "help") == 0)
  {
    displayHelp();
  }
  else
  {
    mySerial.println("ERR=3");
  }
}

/* ---------------------------------------------- */

void setup() 
{
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    mySerial.begin(9600);

    mySerial.println("************************************");
    mySerial.println("      Keyboard/mouse over UART      ");
    mySerial.println("************************************");
    displayHelp();
}

/* ---------------------------------------------- */

void loop() 
{
  static char sBuffer[CMD_MAX_SIZE];
  static int  sBufferCount = 0;

  if (mySerial.available() > 0)
  {
    sBuffer[sBufferCount] = mySerial.read();
    mySerial.write(sBuffer[sBufferCount]);
    //mySerial.println(sBuffer[sBufferCount], HEX);
    
    if (sBuffer[sBufferCount] == '\r' || sBuffer[sBufferCount] == '\n')
    {
      sBuffer[sBufferCount] = '\0';
      sBufferCount = 0;  
      handleCommand(sBuffer); 
    }
    else
    {
      sBufferCount++;
    }
  }
}
