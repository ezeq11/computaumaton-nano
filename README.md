# Computaumaton Nano

A visual simulator for finite-state automata, pushdown automata, and turing machines small enough to run on an Arduino Nano. (That's 2048 bytes of RAM with IO over serial!)  

![&lt;simulation animation&gt;](demo/preview.gif)  
2-symbol 5-state [busy beaver](https://en.wikipedia.org/wiki/Busy_beaver) simulation running on Windows  

## guiding principle: visual illustration
The intention is to elucidate the nature of automata in a visual format without shying away from the rigorous mathematical substrate through which they are formally described. In simpler terms: it's technical, but it's not confusing. (hopefully.)  
An instructor's aid is recommended for pedagogical use. This is a simulator alone, so unfortunately there is no lecture component. However, it should help actively illustrate the function of these automata, without the student having to infer it from static mathematical notation.  
For the tinkering hobbyist who isn't quite convinced that PDAs can't recognize (a^n)(b^n)(c^n) (that was me a few years ago): have fun!

## guiding principle: efficiency
It is my belief that software should be as accessible as possible, considerate of those whose economic situations limit their access to decent computers.  
In ensuring that this simulator runs smoothly on an Arduino Nano, it is guaranteed that no one will be unable to run this program. Not even someone trying to teach themselves how to code despite living in an area where formal computer science education doesn't exist, working with computers that are either antiquated or Chromebooks.  
(Once again, that was me quite a few more years ago.)  

# Compiling for Desktop

1) create the directories "bin/" and "obj/"  
![](demo/desktop_compile_1.png)

2) On Linux, install ncurses. On Windows, download and build PDCurses, and change the value of `WIN_PDCURSES_DIR` in the Makefile to PDCurses' location on your computer:  
![](demo/desktop_compile_2.png)

3) run make!  

# Compiling for Arduino Nano

1) create the directory "main/"  
![](demo/arduino_compile_1.png)

2) merge src/ and inc/ into main/  
![](demo/arduino_compile_2.png)

3) rename all .cpp files into .ino files  
![](demo/arduino_compile_3.png)

4) open the files in Arduino IDE and uncomment `#define ARDUINO_NANO_BUILD` in compile_config.hpp  
![](demo/arduino_compile_4.png)

5) upload the project to the Arduino Nano board!  
![](demo/arduino_compile_5.png)

  