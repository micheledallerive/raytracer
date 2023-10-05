CXX = g++
CXXFLAGS = -g -std=c++17
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

%.png: %.ppm
	convert $< $@

all: main.o

run: all
	./main.o
	make result.png

clean:
	rm -f *.o