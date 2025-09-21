## Welcome to ZeroPilot3.5
ZeroPilot is WARG's custom flight controller! Before beginning work, please take a look at the desgin documentation and KT videos here: https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/2668101634/ZeroPilot3.5+Design

## Building the Project
TODO:
- hardware build
- testing build

## Debugging GoogleTest in VS Code
To debug the testing build in VS Code, add the following `launch.json` file to the `.vscode` folder:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "GTest Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/zeropilot3.5/tests/build/gtestzeropilot3.5",
            "args": [
                "--gtest_filter=*"
            ],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": <path to gdb>,
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

Notes:

- make sure to update `<path to gdb>` to reflect the path to your gdb executable
- use `--gtest_filter` to run a specific or subset of tests: https://google.github.io/googletest/advanced.html#running-a-subset-of-the-tests


- Run `g++ -I/usr/include -I./zeropilot3.5/include -I./zeropilot3.5/include/driver_ifaces -I./zeropilot3.5/tests/driver_mocks -std=gnu++17 ./zeropilot3.5/tests/system_manager/21004_Watchdog.cpp -lgtest -lgmock -lgtest_main -pthread -o 21004_Watchdog && ./21004_Watchdog` to test `21004_Watchdog.cpp`