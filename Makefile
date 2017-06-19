CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11
LDFLAGS =
CPPFLAGS =

ifeq ($(EXPATCC_DEBUG),1)
CXXFLAGS += -g -O0
endif

DEPS = expat

CXXFLAGS += $(shell pkg-config --cflags $(DEPS)) -fPIC
LIBS = -Wl,--as-needed $(shell pkg-config --libs $(DEPS))

TARGET = libexpatcc.so

SRC = $(wildcard *.cc)
OBJ = $(SRC:%.cc=%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) -shared $(LIBS)

clean:
	rm -rf $(OBJ) $(TARGET)

.PHONY: all clean

