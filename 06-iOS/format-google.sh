# -o is OR operator
# -i is for INPLACE convert
find . -iname *.h -o -iname *.m -o -iname *.mm | xargs clang-format -i -style=google
