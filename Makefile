.PHONY: build
build:
	cmake --build build --target cppquiz

format:
	cmake --build build --target clangformat
