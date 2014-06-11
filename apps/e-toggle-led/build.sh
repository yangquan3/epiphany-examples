#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin/

CROSS_PREFIX=
case $(uname -p) in
    arm*)
        # Use native arm compiler (no cross prefix)
        CROSS_PREFIX=
        ;;
       *)
        # Use cross compiler
        CROSS_PREFIX="arm-linux-gnueabihf-"
        ;;
esac

# Build HOST side application
${CROSS_PREFIX}gcc src/e-toggle-led.c -o bin/e-toggle-led.elf -I ${EINCS} -L ${ELIBS} -le-hal #-le-loader

# Build DEVICE side program
e-gcc -T ${ELDF} src/device-e-toggle-led.c -o bin/device-e-toggle-led.elf -le-lib

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/device-e-toggle-led.elf bin/device-e-toggle-led.srec


