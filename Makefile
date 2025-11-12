.PHONY: all build test clean
all: build

build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

test:
	ctest --test-dir build --output-on-failure

clean:
	rm -rf build
