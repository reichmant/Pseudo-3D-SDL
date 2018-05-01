#OBJS specifies which files to compile as part of the project
OBJS = SDL_Render_Main.c

#CC specifies which compiler we're using
CC = gcc
CC_PI = ~/raspidev/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-gcc
#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w

#INCLUDE_FLAGS
INCLUDE_FLAGS = -I"/home/thomas/raspidev/SDL_cross/include/"

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lm -lSDL2_ttf
LINKER_FLAGS_PI = -lSDL2 -lm -std=c99

#LINKER_DIRECTORIES
LINKER_DIRECTORIES_PI = -L"/home/thomas/raspidev/SDL_cross/lib/"

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = output.o

#This is the target that compiles our executable
all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

test: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME) && ./$(OBJ_NAME)

pi: $(OBJS)
	$(CC_PI) $(OBJS) $(COMPILER_FLAGS) $(INCLUDE_FLAGS) $(LINKER_DIRECTORIES_PI) $(LINKER_FLAGS_PI) -o $(OBJ_NAME)
