#!/bin/bash
find src/ -regex '.*\.\(c\|cpp\|h\|hpp\|tpp\)' -exec clang-format -i -style=file {} +

find src/ -regex '.*\.\(c\|cpp\|h\|hpp\|tpp\)' -exec clang-tidy {} --fix -p build/ \;
