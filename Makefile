.PHONY: build
build:
	cmake --build build --target cppquiz

docker:
	docker build -t rayburgemeestre/cppquiz:v1 .

format:
	cmake --build build --target clangformat
