SET app=bnwpyramid

adb shell am start -n com.ru7w1k.%app%/com.ru7w1k.%app%.MainActivity
timeout 4
adb shell screencap /sdcard/screencap.png
adb pull /sdcard/screencap.png
