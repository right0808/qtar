CXXFLAGS=-W -Wall -g -O2

OBJECTS=qtar.o linux_getdents.o firstblock.o
BIN=qtar

all: $(BIN)

clean:
	rm -f $(BIN) $(OBJECTS)

$(BIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS)  $^ -o $@ $(LDFLAGS)
