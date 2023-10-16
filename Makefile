CXX = g++
CXXFLAGS = -g -std=c++17 -Ofast
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

# watch continuously the file main.cpp, when there is a change call make run
dev:
	echo "main.cpp" | entr -n make run