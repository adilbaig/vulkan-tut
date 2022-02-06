CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

HelloTriangle: main.cpp
	g++ $(CFLAGS) -o HelloTriangle main.cpp $(LDFLAGS)

.PHONY: test clean

test: HelloTriangle
	./HelloTriangle
	rm -f HelloTriangle

clean:
	rm -f HelloTriangle