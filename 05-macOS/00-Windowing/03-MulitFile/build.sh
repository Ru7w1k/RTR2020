clear
mkdir -p Window.app/Contents/MacOS
clang -o Window.app/Contents/MacOS/Window Window.m AppDelegate.m MyView.m -framework Cocoa
