# Uses Arduino.mk https://github.com/sudar/Arduino-Makefile
BOARD_TAG    = atmega328
ARDUINO_LIBS = SerialCommand RFControl Keypad DHTlib Ethernet SPI
USER_LIB_PATH := $(realpath ./libraries)
MONITOR_PORT  = /dev/ttyUSB0
CPPFLAGS    = -DNETWORK_ENABLED -DMAX_RECORDINGS=300

ARDMK_DIR := $(realpath ./arduino-mk)
include $(ARDMK_DIR)/Arduino.mk
