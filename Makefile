CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS ?= -lglut -lGLU -lGL

SRC := src/main.cpp src/game.cpp src/game_state.cpp src/game_logic.cpp src/game_render.cpp
OUT := samsat-quest

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT) src/main.exe

.PHONY: all clean
