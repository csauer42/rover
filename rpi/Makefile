INC = -I include 
EXTCXX = cpp
EXTC = c
SRC = src
CXX = g++
CC = gcc
BUILD = build
BIN = bin
TEST = tests
LIBDIR = lib
CXXFLAGS = -Wall $(INC) -std=c++11 -Os 
CFLAGS = -Wall $(INC) -std=c99 -Os 
STRIP = strip

ifeq ($(debug), true)
CXXFLAGS += -g
CFLAGS += -g
endif

all: Intermediary SerialTest strip


test: $(TESTS)

# object build
$(BUILD)/%.o: $(SRC)/%.$(EXTCXX)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD)/%.obj: $(SRC)/%.$(EXTC)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/%.lo: $(SRC)/%.cpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -fpic -c -o $@ $^

# main targets

Intermediary: $(BUILD)/Intermediary.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $< -lzmq

SerialTest: $(BUILD)/SerialTest.o $(BUILD)/serial_setup.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

# utility
strip:
	$(STRIP) $(BIN)/* 

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILD) $(BIN)

.PHONY: clean strip all
