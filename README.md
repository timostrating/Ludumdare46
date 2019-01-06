# GBA Project Template
This repository contains a basic setup for building structured and test-driven Game Boy Advance (GBA) applications.

## Project structure
The actual content of the GBA application is stored in `source`:

- `app` : This is where all code resides in `.c` and `.h` files. Tests are named `*.test.c`.
- `assets` : All sound, images, sprites and maps are stored here.
- `data` : All game data, like entity information and default settings are stored here.

The `tools` folder contains all tools that ease development, and the `lib` folder contains all external libraries used.

## Setting up
This project has been configured for **Linux**, and will most probably not work in other environments without tweaking.
In order to be able to build this project, you will need to perform the following steps:

1. **Install DevkitPro** for the GBA and setup the right environment variables (see [this guide](https://www.coranac.com/tonc/text/setup.htm))
2. **Make sure all submodules are present** by calling `git submodule update --init`.
3. **Build libtonc**, which is the core GBA library the project uses.
  1. Open a terminal in `lib\libtonc`.
  2. Build the library by calling `make`.
4. **Build Tiled2GBA**, which is the map converted for .tmx files.
  1. Open the `tools\tiled2gba` folder.
  2. Follow it's readme instructions.
  3. You should have the compiled binary in `tools\tiled2gba\build`.

After this, you should be ready to build the project.

## Building
Once the project has been setup properly, the only thing a developer needs to do is to call the right `make` commands from the root of the project:

- `make` : builds the game and exports a `.elf` file.
- `make test` : builds both the game and its tests in the `.test.c` files, exporting a `.elf` file that runs all these tests. This is made possible by the `minunit_test_builder` in the tools folder and the `minunit` testing library.
- `make clean` : removes all junk created by the building processes above.
