Compile with esp-idf SDK.

Before compiling change main/dummy_conf.inc

Needs an mqtt-broker accepting anonymous connections.
Build with command 'idf.py build'
Flash with command 'idf.py -p SERIAL_DEVICE flash', replace SERIAL_DEVICE.

Time intervalls are for testing right now and hardcoded.
There is no nice clean-up before the device restarts.

main/inc headers
main/src implementations

main

dummy_conf.inc dummy configuration the device bruns writes to its flash memory, if there is no configuration in flash.
