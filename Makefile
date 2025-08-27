GXX = g++
GCC = gcc
TARGET = Test
FLAGS = -o
SOURCE = main.cpp

$(TARGET)	:	$(SOURCE)
	$(GXX)	$(SOURCE)	$(FLAGS)	Test

class	:	class.cpp
	$(GXX)	class.cpp	$(FLAGS)	Test

c	:	main.c
	$(GCC)	main.c	$(FLAGS)	$(TARGET)

.PHONY	:	clean
	rm	$(TARGET)
