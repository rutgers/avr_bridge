set(ARDUINO_PROTOCOL "arduino")
set(ARDUINO_BOARD "atmega328p")
set(ARDUINO_MCU "m328p")
set(ARDUINO_FCPU "8000000")
set(ARDUINO_UPLOAD_SPEED "57600")
set(ARDUINO_PORT "/dev/ttyUSB0")

include("${avr_bridge_PACKAGE_PATH}/arduino_cmake/cmake_scripts/arduino.cmake")
