#OBJS specifies which files to compile as part of the project
OBJS = src/*.cpp

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -ggdb -w -std=c++17 -Wall -Wpedantic -Wfatal-errors

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_ttf -lSDL2_gfx -lcurl -lcurlpp -lmpdclient

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = $(shell basename "${PWD}")

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
