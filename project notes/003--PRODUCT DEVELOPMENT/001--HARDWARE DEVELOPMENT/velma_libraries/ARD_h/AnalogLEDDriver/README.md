# Arduino non-blocking LED driver

## Why?

This is my first attempt at creating non-blocking [blink](https://docs.arduino.cc/built-in-examples/basics/Blink) (the Hello, World! in microcontroller land).
Also, most example code on Arduino tutorials is procedural, and this project serves to better my OOP skills in C++ as well.


### How should I use it in my own project?

1. Set up your LED:

    ```C++
    // The PWM output that connects your LED is the param to the LedDriver constructor.
    LedDriver redLed(2);
    ```

1. Switch your LED to the intended action. Choose between `LED_OFF` (the default), `LED_ON`, `LED_BLINK`, `LED_GLOW` and `LED_SINGLE_PULSE`:

    ```C
    redLed.switchTo(LED_BLINK);
    ```

    - Note: Single pulse is just that; a single pulse. After that, the action changes to `LED_OFF`. The rest of the actions will stay in that state until `switchTo` is invoked.

1. In your loop, allow the LED driver to do its magic. Do this by calling `maintain`.

    ```C
    void loop() {
        // all your other code
        redLed.maintain();
    }
    ```

### The meaning of the different modes an LED can switch to

1. The (default) mode is `LED_OFF`. This will turn the LED off.
1. `LED_ON` will turn (and keep) your LED on.
1. `LED_BLINK` will briefly turn your LED on, and off for about a second. And repeat.
1. `LED_GLOW` will increase and then decrease the LED, and then keep it off for the same duration. And repeat.
1. `LED_SINGLE_PULSE` will _very_ briefly turn on the LED, and then *switch to `LED_OFF`. This is the only mode that switches to a different mode automagically.

Apart from the on and off values, I had 3 different states I wanted to make visible.
The blinking LED is handy to inform your user all is hunky dory, as it results in a constant stream of affirmations (especially with a green LED).
The glowing LED informs the user the Arduino is doing something. Possibly scanning for networks, waiting on a message to arrive on Serial, or some external condition to be met.
The single pulse is handy if an event happens irregularly, but want to be informed when/if they happen. A bit like the LED on an Ethernet port, or a HDD usage indicator light.


## What?

I ran this using an Arduino MKR 1010. Any Arduino with 1+ (analogue) PWM port will do.

- An LED (pick your favorite color)
- a resistor (220 Ohm)
- wires
- your Arduino

See the [examples](libraries/LedDriver/examples) directory for working code.

## Now What?

This library is somewhat optimised for speed. Possibly it can be improved, but meh.
Also, this is not examplary C++ object oriented programming, as I know OOP in a different language, and I'm still learning.

## So What?

Indeed. ¯\\\_(ツ)_/¯

## Contributing

Thank you!

- Fork it.
- Put your code in a branch.
- Open a merge request.

For all interactions, see the [Code of Conduct](CodeOfConduct.txt).

### To do

(amongst other things...)

- Convert this into a proper library so others can `#include` it.
- Add unit tests.
- Whatever you can think of (feel free to contact me prior, so we can discuss implementation). Please open an issue for your feature request!

## License

[MIT](LICENSE.txt).
