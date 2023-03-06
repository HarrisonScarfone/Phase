# Phase Chess Engine

My next attempt at a chess engine. Engine is signifigantly stronger than my first attempts. Move generation is also fast enough to test perft at more meaningful depths resulting drastically increased confidence in legal move generation.

I'll post better instructions as the engine progresses. I can't beat it anymore, but I'm aiming *signifigantly* higher on the strength ladder than that XD.

## Setting Up Locally

### Creating a Local Build

Create a folder called `build` at the root of the project.

```shell
mkdir build
```

Using [CMake](https://cmake.org/), run the following commands *inside* the build folder. Select the command containing `Release` to play against the engine, use debug for debugging.

```shell
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" 
```

OR

```shell
cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" 
```

THEN

```shell
make all
```

### Setting up Your Environment for Debugging

I use VSCode, if youd like to use my settings files feel free. In a `.vscode` folder at project root, add these two files to be able to run with debugging.


1. `launch.json`

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "(gdb) Launch",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/build/src/Phase_run",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}",
      "environment": [],
      "externalConsole": false,
      "MIMode": "gdb",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ],
      "preLaunchTask": "build-debug"
    }
  ]
}
```
2. `tasks.json`

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "build-debug",
      "type": "shell",
      "command": "cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -G \"Unix Makefiles\" && make all"
    },
  ]
}
```

## Running Files

### Regular Runs

From inside the `build` folder:

```shell
./src/Phase_run
```

HOWEVER, I recommend not running it in this fashion and instead running the engine inside of a UCI compatible GUI such as [Arena](http://www.playwitharena.de/).

### Running Tests

From inside the `build` folder:

```shell
./test/Phase_test
```

