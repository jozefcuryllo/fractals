BUILD_DIR := build

.PHONY: all build run clean release

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make -j$$(nproc)

release:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$$(nproc)

run: build
	./$(BUILD_DIR)/Fractal
	
clean:
	rm -rf $(BUILD_DIR)
