CPP=g++
CPPFLAGS=-std=c++17 -Wall -Wextra
LIBS=
EXE=run_orbis

OBJ_DIR=obj
SRC_DIR=src

_DEPS=options.hpp utility.hpp wavefront_obj.hpp window.hpp camera.hpp texture.hpp light.hpp instance.hpp ant_attack.hpp world.hpp blockinstance.hpp
DEPS=$(patsubst %,$(SRC_DIR)/%,$(_DEPS))

_OBJ=main.o options.o utility.o wavefront_obj.o window.o camera.o texture.o light.o instance.o world.o blockinstance.o
OBJ=$(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

OS := $(shell uname)

ifeq ($(OS),Darwin)
# Mac OS
	LIBS+=-framework OpenGL -lGLEW -lglfw -lpng
endif

default: debug

debug: CPPFLAGS += -g
debug: build

release: CPPFLAGS += -O2
release: build

build: setup_build $(EXE)
	@echo "Build finished"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(EXE): $(OBJ)
	$(CPP) $(CPPFLAGS) $(LIBS) $^ -o $@

setup_build:
	@mkdir -p $(OBJ_DIR)

.PHONY: clean

clean:
	@echo "Cleaning"
	@rm -f $(OBJ_DIR)/*.o *~ $(SRC_DIR)/*~
