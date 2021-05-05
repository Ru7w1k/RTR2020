@echo off
cls
for /D %%f in (.\*) do (
	echo -- building %%f ------------------
	cd %%f
	call gradlew.bat clean
	call gradlew.bat build
	adb install -r app/build/outputs/apk/debug/app-debug.apk
	call screencap.bat
	cd ..
	echo ----------------------------------
)

