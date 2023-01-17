# srclone

A simple multidirectional shoot 'em up game written in C.

## How to compile and run

### Music

Sadly the music cannot be included with the project due to licensing differences.
Feel free to put your own music in `data/sound/game_st0.opus`.
The game works without sound files.

### Compile

**Requires `SDL2`, `SDL2_image` and `SDL2_mixer`.**

The remaining libraries are installed with `SDL2*`, don't worry about them.
The code expects SDL2 headers in the `SDL2/SDL*.h` directory,
so you might have to adjust your include directories.

**Compile like this:**

Create a build directory:

`mkdir build && cd build`

Generate build files with `cmake`:

`cmake ..`

Build using your favorite build tools.

##### Linux / MSYS2:

`make`

That's it.

##### Microsoft Visual Studio:

Open the solution file, set up the include directories and library directories for
`SDL2`, `SDL2_image` and `SDL2_mixer` and build the project.

## Run

After successfully compiling, you should have a working
game executable (`sr_clone`/`sr_clone.exe`).

**Copy the `data` folder into your current working directory** and run the game.

Enjoy!
