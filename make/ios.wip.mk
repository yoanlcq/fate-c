

# See https://developer.apple.com/library/mac/documentation/CoreFoundation/Conceptual/CFBundles/BundleTypes/BundleTypes.html
# To test the app : 
# Run Simulator
#     open -a Simulator
# Once Simulator is running :
#     xcrun simctl install booted hey.app
# The app can now be launched from the device's menu.
# Also, here's a way to do it from the command line :
#     xcrun simctl launch booted com.yoanlecoq.hey
#
# For completeness, it's possible to run a specific device with :
#     xcrun instruments -w <device_name>
# Where <device_name> can be one of the items given by :
#     xcrun instruments -s
# The above command also gives a list of known templates.
#
# Knowing this, it's easy to run the latest iPhone device with :
#     xcrun instruments -t Blank -w $(xcrun instruments -s | grep iPhone | tail -n 2 | cut -d [ -f 2 | cut -c 1-36)
# But the regular "open Simulator" is simpler and guaranteed to work.

all: hey

SDK=$(shell xcrun --sdk iphonesimulator --show-sdk-path)
FRAMEWORKS=$(SDK)/System/Library/Frameworks/
INCLUDES=$(SDK)/usr/include

FLAGS= -arch i386 -mios-simulator-version-min=6.1 -fobjc-abi-version=2 \
        -isysroot $(SDK) 

hey.o: hey.c
	clang $(FLAGS) -c hey.c -o hey.o

hey: hey.o
	clang $(FLAGS) -o hey hey.o -framework Foundation -framework UIKit

