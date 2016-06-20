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

# BUILDPATH=build/android/cube
# cp -r SDLPATH/android-project/* BUILDPATH/
# mkdir BUILDPATH/jni/SDL
# cp -r SDLPATH/src               BUILDPATH/jni/SDL
# cp -r SDLPATH/include           BUILDPATH/jni/SDL
# cp -r SDLPATH/Android.mk        BUILDPATH/jni/SDL
# ACTIVITY="CubeActivity"
# cp -r src/cube BUILDPATH/jni/src
#
## Fill in a default Activity
# mkdir BUILDPATH/src/org/me/cube
# echo "package $APP;" >  "BUILDPATH/src/org/me/cube/$ACTIVITY.java"
# echo "import org.libsdl.app.SDLActivity;" >> "BUILDPATH/src/org/me/cube/$ACTIVITY.java"
# echo "public class $ACTIVITY extends SDLActivity {}" >> "BUILDPATH/src/org/me/cube/$ACTIVITY.java"
#
# sed -i "s|YourSourceHere.c|$MY_SOURCE_FILES|g" $BUILDPATH/jni/src/Android.mk
# sed -i "s|org\.libsdl\.app|$APP|g" $BUILDPATH/AndroidManifest.xml
# sed -i "s|SDL App|$MY_APP_NAME|g" $BUILDPATH/res/values/strings.xml
# sed -i "s|SDLActivity|$ACTIVITY|g" $BUILDPATH/AndroidManifest.xml
# sed -i "s|SDLActivity|$APP|g" $BUILDPATH/build.xml
# cd $BUILDPATH
# android update project --path $BUILDPATH
# ndk-build -j $NCPUS $NDKARGS
# ant debug
#
# cd $CURDIR
#
# APK="$BUILDPATH/bin/$APP-debug.apk"
#
# if [ -f "$APK" ]; then
#     echo "Your APK is ready at $APK"
#     echo "To install to your device: "
#     echo "cd  $BUILDPATH"
#     echo "ant debug install"
#     exit 0
# fi
#
