GXX = g++
TARGET = Test
FLAGS = -o
SOURCE = main.cpp

$(TARGET)	:	$(SOURCE)
	$(GXX)	$(SOURCE)	$(FLAGS)	Test

class	:	class.cpp
	$(GXX)	class.cpp	$(FLAGS)	Test

.PHONY	:	clean
	rm	$(TARGET)
