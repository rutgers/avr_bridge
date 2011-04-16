cmake_minimum_required(VERSION 2.6)


set(ARDUINO_CORE_DIR  "${avr_bridge_PACKAGE_PATH}/arduino_cmake/arduino")
include( "${avr_bridge_PACKAGE_PATH}/arduino_cmake/cmake_scripts/${ARDUINO_TYPE}.cmake" )

include_directories("${PROJECT_SOURCE_DIR}/src")

add_custom_target(gen_avr_ros ALL)
add_custom_command(TARGET gen_avr_ros PRE_BUILD
        COMMAND rosrun avr_bridge gen_avr.py ${PROJECT_SOURCE_DIR}/${AVR_BRIDGE_CONFIG} ${PROJECT_SOURCE_DIR}/src )
        

#to compile, use make
#to program arduino on /dev/ttyUSB0, do make flash
#to check the program size use make size
