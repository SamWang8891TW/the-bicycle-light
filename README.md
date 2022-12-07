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

2.Functionalities(
Note the manual is avilable as a pdf file in the repo(will be uploaded).):

  Front light: 
    -It has a white light strip which can light up the road infront of you. It has a button and a indicate led light. 
    -The button is used as single-operation mode which user decided to use it without a main control connected. When the button is pressed, the light can be switched on or off without the main control.
    -The led is a multi-funtional indicate light. 
        LED ON --> Light is on or the device is charging(when plugged). 
        LED OFF --> Light is off or fully charged(when plugged).
        LED FLAHING --> Battery low.
       
  Rear light:
    -It has three light strip, main read light(red) and left/right light(yellow) for turing singals. It has a button and a indicate led light. 
    -The button is used as single-operation mode which user decided to use it without a main control connected. When the button is pressed, it then disable all ongoing turing signal light and proceed being a switch of the main light. Turning singla light is not accessable while is single-operation mode.
    -The led is a multi-funtional indicate light. 
        LED ON --> Main light is on or the device is charging(when plugged). 
        LED OFF --> Main light is off or fully charged(when plugged).
        LED FLAHING --> Battery low.
   
  Main control:
    -User can control the whole system using is control.
    -It has 4 buttons: Main lights switch, left turning signal, right turning signal and dual flash(warning light).
    -It has 5 LEDs, indicating: Head(Front) light on/off, rear light on/off, left turning signal on/off and right turning signal on/off.
    -When the button is pressed, the vibrator vibrates(and the buzzor beeps if the user has enabled via the code), it then turn the light on or off depend on which button user pressed. To confirm the operation, user can view the indicate LEDs to make sure that they did the operation correctly. Note that the indicate LED does not flash as the real light, constantly on instead.
    
    
3.Instructions








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
    
    
      
