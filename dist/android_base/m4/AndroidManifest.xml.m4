<?xml version="1.0" encoding="utf-8"?>
<!-- Replace org.libsdl.app with the identifier of your game below, e.g.
     com.gamemaker.game
-->
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
      package="GAME_APP_DOMAIN"
      android:versionCode="1"
      android:versionName="1.0"
      android:installLocation="auto">

    <!-- Android 2.3.3 -->
    <uses-sdk android:minSdkVersion="10" android:targetSdkVersion="12" />

    <!-- OpenGL ES 2.0 -->
    <uses-feature android:glEsVersion="0x00020000" android:required="true" />

    <!-- Allow writing to external storage -->
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />

    <!-- Create a Java class extending SDLActivity and place it in a
         directory under src matching the package, e.g.
         	src/com/gamemaker/game/MyGame.java

         then replace "SDLActivity" with the name of your class (e.g. "MyGame")
         in the XML below.

         An example Java class can be found in README-android.txt
    -->
    <application android:label="@string/app_name"
                 android:icon="@drawable/ic_launcher"
                 android:allowBackup="true"
                 android:theme="@android:style/Theme.NoTitleBar.Fullscreen"
                 android:hardwareAccelerated="true" >
        <activity android:name="GAME_ACTIVITY"
                  android:label="@string/app_name"
                  android:configChanges="keyboardHidden|orientation"
                  >
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
            <!-- Drop file event -->
            <!--
            <intent-filter>
                <action android:name="android.intent.action.VIEW" />
                <category android:name="android.intent.category.DEFAULT" />
                <data android:mimeType="*/*" />
            </intent-filter>
            -->
        </activity>
    </application>

</manifest> 
