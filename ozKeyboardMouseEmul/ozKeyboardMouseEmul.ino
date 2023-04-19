#include <SoftwareSerial.h>

const byte rxPin = 2;
const byte txPin = 3;
SoftwareSerial mySerial (rxPin, txPin);

#define SPTR_SIZE       10

/* ---------------------------------------------- */

void displayHelp()
{
  mySerial.println("Commands:");
  mySerial.println("    mouse <X> <Y>");
  mySerial.println("    key <KEYS>");
  mySerial.println("    str <STRING>");
  mySerial.println("");
  mySerial.println("KEYS: Space separated (Max 8), it can be:");
  mySerial.println("    MODIFIERS: CTRL, SHIFT, ALT");
  mySerial.println("    TOUCHS: UP, DOWN, LEFT, RIGHT,...");
}

/* ---------------------------------------------- */

typedef struct key_mapping_s
{
  const char *key_str;
  uint16_t value;
}key_mapping;

key_mapping keys_modifier_map[] = 
{
  {"CTRL", MODIFIERKEY_CTRL},
  {"SHIFT", MODIFIERKEY_SHIFT},
  {"ALT", MODIFIERKEY_ALT}
};

key_mapping keys_touch_map[] = 
{
  {"ENTER", KEY_ENTER},
  {"ESC", KEY_ESC},
  {"BACKSPACE", KEY_BACKSPACE},
  {"TAB", KEY_TAB},
  {"UP", KEY_UP},
  {"DOWN", KEY_DOWN},
  {"RIGHT", KEY_RIGHT},
  {"LEFT", KEY_LEFT}
};

/* ---------------------------------------------- */

void setKeyboard(char **keys, int len)
{
    uint16_t key_modifiers = 0x0000;
    for (int i=1; i<len; i++)
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
        }
      }
    }

    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
}

/* ---------------------------------------------- */

void setMouse(long x, long y)
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

#define SPTR_MAX_SIZE   20

int splitStr (char *str, char *p[SPTR_MAX_SIZE])
{
    int  n = 0;
    
    *p++ = strtok (str, " ");
    for (n = 1; NULL != (*p++ = strtok (NULL, " ")); n++)
        if (n == SPTR_SIZE)
            break;

    return n;
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
  static char sBuffer[128];
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

/* ---------------------------------------------- */

void handleCommand(char *str)
{
  char *str_splitted[SPTR_SIZE];
  int strCount = splitStr (str, str_splitted);

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

    Keyboard.print(&str[3]); 
    mySerial.println("OK");
  }
  else if (strcmp(str_splitted[0], "mouse") == 0)
  {
    if (strCount < 3)
    {
      mySerial.println("ERR=2");
      return;
    }
    
    setMouse(atol(str_splitted[1]), atol(str_splitted[2]));
    mySerial.println("OK");
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

/*
#define MODIFIERKEY_CTRL        ( 0x01 | 0xE000 )
#define MODIFIERKEY_SHIFT       ( 0x02 | 0xE000 )
#define MODIFIERKEY_ALT         ( 0x04 | 0xE000 )
#define MODIFIERKEY_GUI         ( 0x08 | 0xE000 )
#define MODIFIERKEY_LEFT_CTRL   ( 0x01 | 0xE000 )
#define MODIFIERKEY_LEFT_SHIFT  ( 0x02 | 0xE000 )
#define MODIFIERKEY_LEFT_ALT    ( 0x04 | 0xE000 )
#define MODIFIERKEY_LEFT_GUI    ( 0x08 | 0xE000 )
#define MODIFIERKEY_RIGHT_CTRL  ( 0x10 | 0xE000 )
#define MODIFIERKEY_RIGHT_SHIFT ( 0x20 | 0xE000 )
#define MODIFIERKEY_RIGHT_ALT   ( 0x40 | 0xE000 )
#define MODIFIERKEY_RIGHT_GUI   ( 0x80 | 0xE000 )

#define KEY_A                   (   4  | 0xF000 )
#define KEY_B                   (   5  | 0xF000 )
#define KEY_C                   (   6  | 0xF000 )
#define KEY_D                   (   7  | 0xF000 )
#define KEY_E                   (   8  | 0xF000 )
#define KEY_F                   (   9  | 0xF000 )
#define KEY_G                   (  10  | 0xF000 )
#define KEY_H                   (  11  | 0xF000 )
#define KEY_I                   (  12  | 0xF000 )
#define KEY_J                   (  13  | 0xF000 )
#define KEY_K                   (  14  | 0xF000 )
#define KEY_L                   (  15  | 0xF000 )
#define KEY_M                   (  16  | 0xF000 )
#define KEY_N                   (  17  | 0xF000 )
#define KEY_O                   (  18  | 0xF000 )
#define KEY_P                   (  19  | 0xF000 )
#define KEY_Q                   (  20  | 0xF000 )
#define KEY_R                   (  21  | 0xF000 )
#define KEY_S                   (  22  | 0xF000 )
#define KEY_T                   (  23  | 0xF000 )
#define KEY_U                   (  24  | 0xF000 )
#define KEY_V                   (  25  | 0xF000 )
#define KEY_W                   (  26  | 0xF000 )
#define KEY_X                   (  27  | 0xF000 )
#define KEY_Y                   (  28  | 0xF000 )
#define KEY_Z                   (  29  | 0xF000 )
#define KEY_1                   (  30  | 0xF000 )
#define KEY_2                   (  31  | 0xF000 )
#define KEY_3                   (  32  | 0xF000 )
#define KEY_4                   (  33  | 0xF000 )
#define KEY_5                   (  34  | 0xF000 )
#define KEY_6                   (  35  | 0xF000 )
#define KEY_7                   (  36  | 0xF000 )
#define KEY_8                   (  37  | 0xF000 )
#define KEY_9                   (  38  | 0xF000 )
#define KEY_0                   (  39  | 0xF000 )
#define KEY_ENTER               (  40  | 0xF000 )
#define KEY_ESC                 (  41  | 0xF000 )
#define KEY_BACKSPACE           (  42  | 0xF000 )
#define KEY_TAB                 (  43  | 0xF000 )
#define KEY_SPACE               (  44  | 0xF000 )
#define KEY_MINUS               (  45  | 0xF000 )
#define KEY_EQUAL               (  46  | 0xF000 )
#define KEY_LEFT_BRACE          (  47  | 0xF000 )
#define KEY_RIGHT_BRACE         (  48  | 0xF000 )
#define KEY_BACKSLASH           (  49  | 0xF000 )
#define KEY_NON_US_NUM          (  50  | 0xF000 )
#define KEY_SEMICOLON           (  51  | 0xF000 )
#define KEY_QUOTE               (  52  | 0xF000 )
#define KEY_TILDE               (  53  | 0xF000 )
#define KEY_COMMA               (  54  | 0xF000 )
#define KEY_PERIOD              (  55  | 0xF000 )
#define KEY_SLASH               (  56  | 0xF000 )
#define KEY_CAPS_LOCK           (  57  | 0xF000 )
#define KEY_F1                  (  58  | 0xF000 )
#define KEY_F2                  (  59  | 0xF000 )
#define KEY_F3                  (  60  | 0xF000 )
#define KEY_F4                  (  61  | 0xF000 )
#define KEY_F5                  (  62  | 0xF000 )
#define KEY_F6                  (  63  | 0xF000 )
#define KEY_F7                  (  64  | 0xF000 )
#define KEY_F8                  (  65  | 0xF000 )
#define KEY_F9                  (  66  | 0xF000 )
#define KEY_F10                 (  67  | 0xF000 )
#define KEY_F11                 (  68  | 0xF000 )
#define KEY_F12                 (  69  | 0xF000 )
#define KEY_PRINTSCREEN         (  70  | 0xF000 )
#define KEY_SCROLL_LOCK         (  71  | 0xF000 )
#define KEY_PAUSE               (  72  | 0xF000 )
#define KEY_INSERT              (  73  | 0xF000 )
#define KEY_HOME                (  74  | 0xF000 )
#define KEY_PAGE_UP             (  75  | 0xF000 )
#define KEY_DELETE              (  76  | 0xF000 )
#define KEY_END                 (  77  | 0xF000 )
#define KEY_PAGE_DOWN           (  78  | 0xF000 )
#define KEY_RIGHT               (  79  | 0xF000 )
#define KEY_LEFT                (  80  | 0xF000 )
#define KEY_DOWN                (  81  | 0xF000 )
#define KEY_UP                  (  82  | 0xF000 )
#define KEY_NUM_LOCK            (  83  | 0xF000 )
#define KEYPAD_SLASH            (  84  | 0xF000 )
#define KEYPAD_ASTERIX          (  85  | 0xF000 )
#define KEYPAD_MINUS            (  86  | 0xF000 )
#define KEYPAD_PLUS             (  87  | 0xF000 )
#define KEYPAD_ENTER            (  88  | 0xF000 )
#define KEYPAD_1                (  89  | 0xF000 )
#define KEYPAD_2                (  90  | 0xF000 )
#define KEYPAD_3                (  91  | 0xF000 )
#define KEYPAD_4                (  92  | 0xF000 )
#define KEYPAD_5                (  93  | 0xF000 )
#define KEYPAD_6                (  94  | 0xF000 )
#define KEYPAD_7                (  95  | 0xF000 )
#define KEYPAD_8                (  96  | 0xF000 )
#define KEYPAD_9                (  97  | 0xF000 )
#define KEYPAD_0                (  98  | 0xF000 )
#define KEYPAD_PERIOD           (  99  | 0xF000 )
#define KEY_NON_US_BS           ( 100  | 0xF000 )
#define KEY_MENU          ( 101  | 0xF000 )
#define KEY_F13                 ( 104  | 0xF000 )
#define KEY_F14                 ( 105  | 0xF000 )
#define KEY_F15                 ( 106  | 0xF000 )
#define KEY_F16                 ( 107  | 0xF000 )
#define KEY_F17                 ( 108  | 0xF000 )
#define KEY_F18                 ( 109  | 0xF000 )
#define KEY_F19                 ( 110  | 0xF000 )
#define KEY_F20                 ( 111  | 0xF000 )
#define KEY_F21                 ( 112  | 0xF000 )
#define KEY_F22                 ( 113  | 0xF000 )
#define KEY_F23                 ( 114  | 0xF000 )
#define KEY_F24                 ( 115  | 0xF000 )
*/
