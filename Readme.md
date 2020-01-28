[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# Terms of Enrampagement (Working Title)

Since I like games, why not make one. The main thing I want to achieve with this is to enjoy coding, discovering and solving some Problems. Experiment with Network & Performance Programming.

## What kind of game should I make

I actually started my first programming steps with a game called "Warcraft 3" it came with a map editor and a very strange (or cumbersome) programming language called *JASS*.

Back then I made a game called *Archer Wars* which was a simple Battle Arena kind of game. You control one unit which has a set of abilities and your goal is to get points by killing other players.

There were some items and interactions with the terrain possible which made the game more interesting.

So why not try to revive this game? But this time lets try to code it from scratch!

These are the things I want to have a look at:

* Rendering - Lets write a basic engine to get a feel for graphics programming
* Network Coding - Can we have a server that handles a loooot of clients, how many units is the limit, how can I handle synchronization errors
* Some fun stuff, like cool abilities and a lot of projectiles

## Architecture

Right now, not much there. My idea is to see the game as a deterministic simulation that has to be synchronized on all clients while the server is the master. For this I will try to implement one game state that is always updated for all the players.

Each player action will be a command, which will be sent to the server, which in turn distributes it to each other player. The command will have a timestamp and the clients can then take into account at what time the command was issued and what happened in the meantime.

Thats the basic Idea, lets see what we can do.

## Following Tools and Libraries are required

* CMake
* Conan
* GTest
* Google Microbenchmark
* SFML

This should all be taken care of by Conan and CMake :)

**Note**: By default this uses C++20 because I want to play around with the newest things.

## Running this in WSL2

I tried running the application in WSL2, but could not get it to work because of this error:
*"Error: GLX: Failed to load GLX"*

Here is what I did:

* Install OpenGL dependencies to enable linking (some opengl libraries were missing e.g. xcursor, xrender,...)
* Install vcXsrv on windows to have something to display something (works with glxgears)
* Set Display: export DISPLAY=:0
* Run the application ...

This always resulted in the above error, I tried googling and adjusting paths and installing some packages but nothing worked. Maybe it is due to vcXsrv but I did no try anything else yet, will let it rest for now.

## Building

Requirements:

* CMake
* Conan
* Python (for Conan)

To build simply run those commands. ( If you want to use specific compiler settings I recommend setting up and using a conan profile, e.g. conan install .. -p gcc-9 ..)

```shell
mkdir build && cd build
conan install .. --build missing
conan build ..
```

Now you should see the library successfully compiling and running the tests.
