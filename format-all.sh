#!/bin/bash
find src/ -regex '.*\.\(c\|cpp\|h\|hpp\|tpp\)' -exec clang-format -i -style=Microsoft {} +

find src/ -regex '.*\.\(c\|cpp\|h\|hpp\|tpp\)' -exec clang-tidy {} --fix -p build/ \;
