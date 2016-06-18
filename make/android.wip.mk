APP_ABI=(
	x86
	x86_64
	arm-linux-androideabi
	mipsel-linux-androideabi
	aarch64-linux-androideabi
	mips64el-linux-androideabi
)
GCC_VER=4.9
SYSROOT=$(NDK)/platforms/android-21/arch-arm
CC=$(NDK)/toolchains/$(APP_ABI)-$(GCC_VER)/prebuilt/linux-x86_64/bin/$(APP_ABI)-gcc-$(GCC_VER) --sysroot=$(NDK)/platforms/android-21/arch-arm

#if arm
# flags += -mthumb
#
# if ab == armeabi-v7a
# CFLAGS= -march=armv7-a -mfloat-abi=softfp -mfpu=neon
# LDFLAGS= -march=armv7-a -Wl,--fix-cortex-a8

