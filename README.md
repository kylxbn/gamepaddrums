# GamepadDrums 4.0

This software was made just for fun (and for SDL2 coding practice).

This software lets you play the drums using a generic USB game controller (more specifically, a game controller similar to the PlayStation 2 controller).

If you have a Dance Dance Revolution-like game controller, you may also plug it in together with your normal game controller to let you play the kick drum and hi-hat with your feet.

## Building instructions

To build this, you need:

* SDL2
* SDL2_mixer
* libflac

Once that is all ready, just run

```
$ make
$ sudo make install
```

and you can then run the app from the terminal using

```
$ gamepaddrums
```

This was primarily built from Windows several years ago, but now that I primarily use Linux, this has been adopted to also build on Linux, specifically, Arch Linux. I don't know if it still builds on Windows.