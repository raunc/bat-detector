#!/bin/bash

SKETCH_NAME=BatDetector
SKETCH_FILE=$SKETCH_NAME/$SKETCH_NAME.ino

arduino --verbose --port /dev/ttyACM0 --board arduino:samd:arduino_zero_bl --upload $SKETCH_FILE
