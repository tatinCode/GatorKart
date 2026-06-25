# Run Instructions
This folder contains the main application entry point for the project.
## Build Targets

From the project root, use these commands:
```
make rgb_sensor
make ir_sensor
make lidar
make motor
make car
```


## Run Targets
After building, run the matching binary from bin/:
```
make run-rgb
make run-ir
make run-lidar
make run-motor
make run-car
```


# Build Instructions
This folder contains the main application entry point for the project.


## Clean Build Files
To remove generated build output:
```
make clean
```


## Notes
- Run all make commands from the project root, not from inside src/.
- src/main.c is the main program used by the build targets.
- Module-specific targets only build the code needed for that subsystem.
- make car builds the full integrated program.


## Current Build Macros
The build system enables modules with compile-time flags such as:
- ENABLE_RGB_SENSOR
- ENABLE_IR_SENSOR
- ENABLE_LIDAR
- ENABLE_MOTORS


These are passed in through the Makefile.
Hardware / Library Requirements
If you are building on the Raspberry Pi and using the real hardware drivers, 
you may need system libraries such as bcm2835. If linking fails, make sure the 
required development libraries are installed and that the Makefile includes the 
needed linker flags.
