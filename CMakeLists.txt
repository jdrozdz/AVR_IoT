CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)
SET(CMAKE_SYSTEM_NAME Generic)

#============================================================================================
PROJECT(AVR)

SET(FILES main.c lib/conf/config.h lib/conf/macros.h lib/conf/relays_mod.h lib/network/network.c lib/network/enc28j60.c lib/uart/uart.c lib/dht22/dht22.c)

SET(DEVICE atmega644pa)
SET(FREQ 8000000)

SET(AVRBIN /usr/bin)
SET(AVROBJCOPY avr-objcopy)
SET(AVRSIZE avr-size)
SET(AVRDUDE avrdude)

SET(PROGRAMMER usbasp)
SET(PORT usb)
# SET(BAUD 57600)

SET(EEPROM NO)
SET(FLASH NO)

#============================================================================================

SET(CMAKE_C_COMPILER ${AVRBIN}/avr-gcc)
SET(CMAKE_CXX_COMPILER ${AVRBIN}/avr-g++)

SET(CMAKE_C_FLAGS  "-Os -mmcu=${DEVICE} -DF_CPU=${FREQ}UL -std=gnu99 -Wl,--gc-sections -Wno-deprecated-declarations -D__PROG_TYPES_COMPAT__")
SET(CMAKE_CXX_FLAGS "-Os -mmcu=${DEVICE} -DF_CPU=${FREQ}UL -Wl,--gc-sections")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_PREFIX_PATH /usr/lib/avr)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.hex"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.eep"
)

include_directories(/usr/lib/avr/include)

SET(SOURCE_FILES ${FILES})

#============================================================================================

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${FILES})

ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${AVRBIN}/${AVROBJCOPY} -O ihex -R .eeprom ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.elf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.hex)

if(EEPROM)
    ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${AVRBIN}/${AVROBJCOPY} -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.elf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.eep)
endif(EEPROM)

ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${AVRBIN}/${AVRSIZE} --target=ihex ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.hex)

if(FLASH)
    ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${AVRBIN}/${AVRDUDE} -p${DEVICE} -c${PROGRAMMER} -P${PORT} -b${BAUD} -U flash:w:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.hex)
endif(FLASH)