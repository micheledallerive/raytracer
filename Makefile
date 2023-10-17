CXX = g++
CXXFLAGS = -g -std=c++17 -O3
HEADERS = $(wildcard **/*.h)

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

profile: all
	rm -f callgrind.out.*
	valgrind --tool=callgrind ./main.o
	# graphically display the call graph
	kcachegrind callgrind.out.*

memcheck: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main.o 2> memcheck.log