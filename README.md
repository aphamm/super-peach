# Super Peach

Super-Peach is a game simulation inspired by Super Mario Bros run in XQuartz. The project comes with different levels in which the user can customize their own gameplay by directly editing the following source files: `level01`, `level02`, `level03` located in the `Assets` directory. 

Move peach around using standard arrow keys or `WASD`. Peach can obtain three powerups: ShootPower, JumpPower, StarPower. With ShootPower activated, press `space` to shoot fireballs. End the game prematurely by pressing `q`.

<p align="center">
  <img src=./peach/Assets/demo.gif height="600">
</p>

## Tech Stack

This application uses `C++`. Using principles of OOP, a majority of this code focuses around using classes, inheritance, polymorphism, constructors, destructors, and STL containers.

## Usage

To run locally, first clone the repository to your local device using the following command. Enter the repository and install all necessary dependencies.

```
$ git clone git@github.com:onlypham/super-peach.git
$ cd super-peach
$ cd peach
```

While inside the `peach` directory run `make` to create all the corresponding binary files needed to compile the program. Finally, run the `./SuperPeachSisters` to run the game.

```
$ make
$ ./SuperPeachSisters
$ make clean # remove binaries
```
