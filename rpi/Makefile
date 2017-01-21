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

all: Intermediary SerialTest SerialReadTest SerialAutoTest SerialReqRep interop strip

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

Intermediary: $(BUILD)/Intermediary.o $(BUILD)/Rover.o $(BUILD)/serial_setup.o \
              $(BUILD)/crc.o $(BUILD)/serial_comms.o $(BUILD)/conversion.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^ -lzmq

conversiontest: $(BUILD)/conversiontest.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

interop: $(BUILD)/interop.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^ -lzmq

SerialTest: $(BUILD)/SerialTest.o $(BUILD)/serial_setup.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

SerialReadTest: $(BUILD)/SerialReadTest.o $(BUILD)/serial_setup.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

SerialAutoTest: $(BUILD)/SerialAutoTest.o $(BUILD)/serial_setup.o $(BUILD)/crc.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

SerialReqRep: $(BUILD)/SerialReqRep.o $(BUILD)/serial_setup.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

# utility
strip:
	$(STRIP) $(BIN)/* 

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILD) $(BIN)

.PHONY: clean strip all
