# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

BOARD_TAG    = protrinket5ftdi
ISP_PORT = /dev/ttyUSB0
MONITOR_PORT = /dev/ttyUSB0

#BOARD_TAG    = mega2560
#ISP_PORT = /dev/ttyACM0
#MONITOR_PORT = /dev/ttyACM0

ARDMK_DIR=/usr/share/arduino

BOARDS_TXT = boards.txt
USER_LIB_PATH = libraries

CXXFLAGS = -Wno-deprecated-declarations -g3

MONITOR_BAUDRATE = 115200

ARDUINO_LIBS = \
			   EEPROM \
			   Wire \
			   TimerOne \
			   SPI \
			   LiquidCrystal_I2C \
			   Adafruit_GFX_Library \
			   Adafruit_ST7735_Library

-include $(ARDMK_DIR)/Arduino.mk

EXTRA_FLAGS = -Wall -Werror
# !!! Important. You have to use make ispload to upload when using ISP programmer
