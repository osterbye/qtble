#!/bin/sh

find . -type f \( -iname '*.h' -o -iname '*.cpp' \) -not -path "*/bluez-5.41/*" | xargs clang-format -style=file -i -verbose
