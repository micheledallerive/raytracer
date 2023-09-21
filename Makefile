CXX = g++
CXXFLAGS = -g -std=c++11

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

all: main.o

run: all
	./main.o

clean:
	rm -f *.o