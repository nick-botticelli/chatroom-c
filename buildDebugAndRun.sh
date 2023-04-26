#!/bin/sh

make clean && CC=clang-16 make debug && ./build/chat_node
