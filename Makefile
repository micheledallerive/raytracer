CXX = g++
CXXFLAGS = -g -std=c++17 -Ofast
# Create headers variable with all .h files in any subdirectory but in include/glm/
HEADERS = $(shell find . -name '*.h' -not -path "./include/glm/*")
DEBUG := 0
ANIMATE := 0

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -DANIMATE=$(ANIMATE) -DDEBUG=$(DEBUG) $< -o $@

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
	find . -type f \( -iname \*.h -o -iname \*.cpp \) | entr -n make run

profile: all
	rm -f callgrind.out.*
	valgrind --tool=callgrind ./main.o
	# graphically display the call graph
	kcachegrind callgrind.out.*

memcheck: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main.o 2> memcheck.log


# ----------------- Animation -----------------
TOTAL_FRAMES = 200
FRAME_RATE = 40

frames_dir:
	mkdir -p frames

frames/%.ppm: ./main.o
	./main.o ./frames/$*.ppm $* $(TOTAL_FRAMES)

frames: frames_dir $(addprefix ./frames/, $(addsuffix .png, $(shell seq -w 0 $(TOTAL_FRAMES))))

result.mp4: ANIMATE := 1
result.mp4: frames
	ffmpeg -r $(FRAME_RATE) -i frames/%03d.png -y result.mp4

