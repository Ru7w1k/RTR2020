for d in */ ; do
    echo "$d"
    cd $d
    ./build.sh
    cd ..
done