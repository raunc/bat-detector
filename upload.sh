#!/bin/bash

SKETCH_NAME=m0
SKETCH_FILE=$SKETCH_NAME/$SKETCH_NAME.ino

arduino --verbose --port /dev/ttyACM0 --board arduino:samd:arduino_zero_bl --upload $SKETCH_FILE
