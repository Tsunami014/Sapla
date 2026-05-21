.PHONY: debug clean publish

all:
	cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
	cmake --build build --parallel --config "Release"

debug:
	cmake -B build -S . -DCMAKE_CXX_FLAGS="-g" -DCMAKE_BUILD_TYPE=Debug
	cmake --build build --parallel --config "Debug"

clean:
	rm -rf ./build
	rm -rf ./AppDir
	rm -rf ./out

publish:
	./publish.sh

