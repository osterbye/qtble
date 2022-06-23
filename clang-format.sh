#!/bin/sh

find . -type f \( -iname '*.h' -o -iname '*.cpp' \) -not -path "*/bluez/*" | xargs clang-format -style=file -i -verbose
