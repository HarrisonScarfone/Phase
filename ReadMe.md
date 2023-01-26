# Phase Chess Engine

My next attempt at a chess engine. Hopefully this one will be much stronger.

## Setting Up Locally

### Creating a Local Build

Create a folder called `build` at the root of the project.

```shell
mkdir build
```

Using [CMake](https://cmake.org/), run the following commands *inside* the build folder. This command will build the project with debugging enabled. I'll add another command when its in a runnable state.

```shell
cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" 
```
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

### Running Tests

From inside the `build` folder:

```shell
./test/Phase_test
```

