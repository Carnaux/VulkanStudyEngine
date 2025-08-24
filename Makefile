include .env

CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include -I/opt/homebrew/Cellar/glfw/3.4/include/ -I/opt/homebrew/Cellar/glm/1.0.1/include/
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
$(TARGET): *.cpp *.hpp 
	g++ $(CFLAGS) -o a.out *.cpp $(LDFLAGS)


%.spv: %
	${GLSLC_COMPILER_PATH} $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out
	rm -f *.spv