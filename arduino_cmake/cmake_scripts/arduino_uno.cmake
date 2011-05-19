set(ARDUINO_PROTOCOL "stk500v1")
set(ARDUINO_BOARD "atmega328p")
set(ARDUINO_MCU "m328p")
set(ARDUINO_FCPU "16000000")
set(ARDUINO_UPLOAD_SPEED "115200")

if (NOT ARDUINO_PORT)
	set(ARDUINO_PORT "/dev/ttyACM0")
endif

include("${avr_bridge_PACKAGE_PATH}/arduino_cmake/cmake_scripts/arduino.cmake")
