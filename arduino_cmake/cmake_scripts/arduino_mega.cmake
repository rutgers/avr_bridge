set(ARDUINO_PROTOCOL "arduino")
set(ARDUINO_BOARD "atmega328p")
set(ARDUINO_MCU "m1280")
set(ARDUINO_FCPU "16000000")
set(ARDUINO_UPLOAD_SPEED "57600")

if (NOT ARDUINO_PORT)
set(ARDUINO_PORT "/dev/ttyUSB0")
endif (NOT ARDUINO_PORT)

include("${avr_bridge_PACKAGE_PATH}/arduino_cmake/cmake_scripts/arduino.cmake")
