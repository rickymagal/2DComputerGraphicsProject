# Compiler
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Directories
SRC_DIR := src
INC_DIR := include
TP_DIR  := third_party

# Executable
TARGET := trabalhocg

# Include paths
INCLUDES := -I$(INC_DIR) -I$(TP_DIR)/tinywml2

# Libraries (Linux + freeglut)
LIBS := -lglut -lGL -lGLU -lm

# Source files
SRCS := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/game/Game.cpp \
	$(SRC_DIR)/game/InputState.cpp \
	$(SRC_DIR)/game/Renderer.cpp \
	$(SRC_DIR)/world/Arena.cpp \
	$(SRC_DIR)/world/Obstacle.cpp \
	$(SRC_DIR)/entity/Player.cpp \
	$(SRC_DIR)/entity/Bullet.cpp \
	$(SRC_DIR)/math/Vec2.cpp \
	$(SRC_DIR)/math/Collision.cpp \
	$(SRC_DIR)/io/SvgLoader.cpp \
	$(TP_DIR)/tinywml2/tinyxml2.cpp

# Object files
OBJS := $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
