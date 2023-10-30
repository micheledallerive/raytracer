CXX = g++
CXXFLAGS = -g -std=c++17 -O3
# Create headers variable with all .h files in any subdirectory but in include/glm/
HEADERS = $(shell find . -name '*.h' -not -path "./include/glm/*")

%.o: %.cpp $(HEADERS)
	echo $(HEADERS)
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