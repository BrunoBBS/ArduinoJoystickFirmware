# Arduino(ATmega32u4) Joystick firmware

First of all, this project uses the ArduinoJoystickLibrary, available at [https://github.com/MHeironimus/ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary).

This firmware aims to add a bit more of control to the values read and sent
to the computer.

To this end, this firmware offers the following functionality:
 - 10bit (0 to 1023) precision, from the Pro micro's [ADC](https://en.wikipedia.org/wiki/Analog-to-digital_converter).
 - Persistent Calibration
 - Custom Curves
 - Filtering
 - Analog stick to HAT switch conversion (is a bit old)

> Beware that you will need to **alter the source code** in order to configure
> it. As this project is compiled, it is a bit laborious to make a "configurator",
> so you will have to change some lines in the code to customize it to your
> input configuration, as well as your curves and filtering likings.

## Configuration (really more like a customization)
To customize this firmware to you setup, search the `Joystick.ino` for the `<CUSTOM>` comments.

There you wil find more information on how to add, remove or change those entries.

In the sections below you will find information about what each functionality
configuration, but for information on what to edit and where to edit, please
search the `.ino` file.

### Setting up buttons
Search for `<BUTTON>`.

This is the one that should change the most. The only configuration needed is
the array of button pins. There you should insert all the pins that
correspond to buttons in your arduino. The order will determine the order
they appear at the computer (e.g. if the fist button of the array is `pin 4`,
then in the computer the `button 1` will be controlled by `pin 4` of the
arduino).

### Setting up axes
Search for `<AXIS>`.

The axes are set the same way as the buttons, but you also have to **specify
the number of axes present**. Just add the pins corresponding to you axes to
the array of axes pins and set the `AXES_ARRAY_SIZE` to the number of pins
you entered.

### Setting up custom curves
Search for `<CURVE>`.

The curves used are
[Bézier Curves](https://en.wikipedia.org/wiki/B%C3%A9zier_curve), so to configure it
you will need to inform 4 numbers from 0 to 1023.

There is a good website that I use to tune curves:
[cubic-bezier.com](cubic-bezier.com) by [Lea Verou](http://lea.verou.me/).
There you can see how the curve will respond to the input value.

To export a curve just multiply the values shown in the site by 1023 and put
them, in order, in the two center points of the bézier.

Example:
``` cpp
// Site curve: cubic-bezier(0,.9,1,.1)
// Approximated multiplied values: 0, 900, 1, 100
// Line of code:
calibration_data.curves[0] = Bezier(Point(0, 0), Point(0, 900), Point(1023, 100), Point(1023, 1023));
// Site curve: cubic-bezier(.31,.91,.6,.15)
// Approximated multiplied values: 310, 910, 600, 150
// Line of code:
calibration_data.curves[0] = Bezier(Point(0, 0), Point(310, 910), Point(600, 150), Point(1023, 1023));
```

The two outer values define a cap for the axis range.

### Setting up hat conversion
Search for `<HAT>`.

To use axes as a hat switch, first add your axes as normal axes.

Then at the end of the `.ino` file, uncomment the section to use HAT as DIGITAL.
Then adjust the index of each axis according to your `axes_pins` array.

## Calibration
The procedure to calibrate the joystick is easier to be done the first time
with a serial monitor. But it can be done without one too.

The calibration just needs to be done the first time you change some axis.
After that the max and min values will be stored in the [EEPROM](https://en.wikipedia.org/wiki/EEPROM).
If you want to recalibrate it, just follow the normal calibration procedure.

### Without serial monitor
1. As you plug the joystick, hold your designated calibration button until the Tx Rx LEDs in the board turn off;
2. Move all your axes to their full extension both ways;
3. Hold the calibration button for 1 second to finish calibration.

### With serial monitor
1. As you plug the joystick, you will see this message:
``` h
=================================
Joystick Initializing - baud 9600
=================================

Hold calibration button to force calibration...
##################################################
```

2. Hold your designated calibration button, before the bar stops filling up, until the this message appears:
``` h
Force calibration
Release calibration button to proceed...
```

3. Release your designated calibration button and this message appears repeatedly:
``` h
Move axes now
========= Calibrating =========
Axis: 0 - Raw Value: 172
Axis: 1 - Raw Value: 150
Axis: 2 - Raw Value: 187
===============================
Hold calibration button to finish...
```
4. Move all your axes to their full extension both ways;

5. Hold the calibration button for 1 second to finish calibration.

## Structure
``` c
.
├── Bezier.cpp        // Source file for the curves. You'll probably never touch this file.
├── Bezier.hpp        // Neither this one.
├── FlightStick.ino   // This is the main file. Here you will make all the changes needed.
├── LICENSE
└── README.md         // You are here.

```

## Compiling and flashing
To use this project you will need the Arduino IDE installed.

Install the [ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary#installation-instructions).

Put the contents of this folder inside a folder called `Joystick`, as this enables you to import it into arduino IDE as a sketch.

Make the modifications needed to the main file.

Upload it to the arduino.