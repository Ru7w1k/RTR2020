for f in *; do
    if [ -d "$f" ]; then
        echo "-- building  $f ------------------"
	cd $f
	./gradlew clean
	./gradlew build
	adb install -r app/build/outputs/apk/debug/app-debug.apk
	./screencap
	cd ..	
	echo "----------------------------------"
    fi
done

