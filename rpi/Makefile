INC = -I include 
EXTCXX = cpp
SRC = src
CXX = g++
BUILD = build
BIN = bin
TEST = tests
CXXFLAGS = -Wall $(INC) -std=c++11 -Os 
STRIP = strip

ifeq ($(debug), true)
CXXFLAGS += -g
endif

all: Intermediary strip

test: conversiontest interop SerialTest SerialReadTest SerialAutoTest SerialReqRep

# object build
$(BUILD)/%.o: $(SRC)/%.$(EXTCXX)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# main targets

Intermediary: $(BUILD)/Intermediary.o $(BUILD)/Rover.o $(BUILD)/serial_setup.o \
              $(BUILD)/crc.o $(BUILD)/serial_comms.o $(BUILD)/conversion.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^ -lzmq

# tests

conversiontest: $(BUILD)/conversiontest.o
	@mkdir -p $(TEST)
	$(CXX) $(CXXFLAGS) -o $(TEST)/$@ $^

interop: $(BUILD)/interop.o
	@mkdir -p $(TEST)
	$(CXX) $(CXXFLAGS) -o $(TEST)/$@ $^ -lzmq

SerialTest: $(BUILD)/SerialTest.o $(BUILD)/serial_setup.o
	@mkdir -p $(TEST)
	$(CXX) $(CXXFLAGS) -o $(TEST)/$@ $^

SerialReadTest: $(BUILD)/SerialReadTest.o $(BUILD)/serial_setup.o
	@mkdir -p $(TEST)
	$(CXX) $(CXXFLAGS) -o $(TEST)/$@ $^

SerialAutoTest: $(BUILD)/SerialAutoTest.o $(BUILD)/serial_setup.o $(BUILD)/crc.o
	@mkdir -p $(TEST)
	$(CXX) $(CXXFLAGS) -o $(TEST)/$@ $^

SerialReqRep: $(BUILD)/SerialReqRep.o $(BUILD)/serial_setup.o
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $^

# utility
strip:
	$(STRIP) $(BIN)/* 

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILD) $(BIN) $(TEST)

.PHONY: clean strip all test
