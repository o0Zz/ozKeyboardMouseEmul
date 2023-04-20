# ozKeyboardMouseEmul
This project rely on Teensy board in order to build a keyboard/mouse emulator over UART (Usefull for automated tests)

# Prerequisite for developement
1) Download and install arduino 1.8.19 https://downloads.arduino.cc/arduino-1.8.19-windows.exe
2) Download and install https://www.pjrc.com/teensy/td_158/TeensyduinoInstall.exe

# Quick start
## Flash it
1) Download and install https://www.pjrc.com/teensy/teensy.exe
2) Start and load .hex file from this projet
3) Plug your board and press the button
4) Done the board is flash.

## Use it.

Plug an UART on the Teensy 2.0 D2(RX) and D3(TX)
Open a Serial Terminal with parameters: 9600 8N1
Type "help"

## Example
Set the mouse pointer to absolute position 150x150 and double click
```
mouse move 150 150
mouse dblclick
```

Send string "Hello world"
```
str hello world
```

Send key "ENTER"
```
key ENTER
```

Send "CTRL+ALT+SUPPR"
```
key CTRL ALT DELETE
```

# Teensy 2.0 Pinout
![image](https://user-images.githubusercontent.com/3352109/233207892-8d6975df-c32d-4483-8391-dd93fbc8934f.png)


# Reference
https://www.pjrc.com/teensy/
https://fr.aliexpress.com/item/1005005343794027.html
