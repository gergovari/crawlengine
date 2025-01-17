#!/bin/bash
find src/ -regex '.*\.\(c\|cpp\|h\|hpp\|tpp\)' -exec clang-format -i -style=Microsoft {} +
