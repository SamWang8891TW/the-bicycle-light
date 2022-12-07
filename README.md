# the-bicycle-light
A project which provides users to make their easy-carry, easy-charge, and rich-function bicycle light.
中文使用者請點選上方的連結觀看中文版讀我檔案。


Current version: v0.1.0 (unfinished)
目前版本：v0.1.0 （尚未完成）

Introduction:
This is a bicycle light designed for easy install and remove from most of the bikes. With


PART I, I AM THE USER OF THE PRODUCT AND WANT TO UNDERSTAND HOW TO OPERATE:

1.Main Parts(detailed components press HERE):
    -Front light
        Installed on the front of the bicycle, equipped with a main light.
    -Rear light
        Installed on the rear of the bicycle, equipped with a main light and a pair of signal light.
    -Main control
        Installed on the handle of the bicycle, being as the control(remote) of the whole system.

2.Functionalities:
  Front light: It has a light strip which can light up the road infront of you. It has a button and a indicate led light. The button is used as single-operation mode which user decided to use it without a main control connected.  

3.Using instructions:

Note the manual is avilable as a pdf file in the repo(will be uploaded).






Please kindly check the pcb version before doing any operation.
Files:

-pcb:
    Contained the every version of the pcb. Please notice:
      1.Version 0.0.1 has the BJT be as the switch, but failed. So the pin C and E is shorted via soldering.
      2.Resistor and other components naming and ordering may not be as same across multiple versions.

-program:
    Contained the Arduino programm of the device. Currently the programm is suitale for all versions of the PCB.
    
-case:
    Contained f3d and stl files of the case for the lights. This is not necessary since you can designed your own version of it. *f3d is for Fusion360 use only!
    
    
      
