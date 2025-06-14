# --- Configuration ---
CXX := g++
CXXFLAGS := -std=c++2b -Wall -Wextra -IuWebSockets/src -IuWebSockets/uSockets/src
LDFLAGS := -lpthread -lssl -lcrypto -lz
TARGET := server
UWS_DIR := uWebSockets
UWS_REPO := https://github.com/uNetworking/uWebSockets.git
.DEFAULT_GOAL := $(TARGET)

# --- Tools and system packages ---
REQUIRED_TOOLS := gcc g++ git make pkg-config
REQUIRED_PKGS := openssl zlib

# --- Dependency check ---
check-deps:
	@echo "Checking for required tools..."
	@for tool in $(REQUIRED_TOOLS); do \
		if ! command -v $$tool >/dev/null 2>&1; then \
			echo "Error: $$tool not found."; exit 1; \
		fi \
	done
	@echo "Checking for required packages..."
	@for pkg in $(REQUIRED_PKGS); do \
		if ! pkg-config --exists $$pkg; then \
			echo "Error: Development package for '$$pkg' not found."; exit 1; \
		fi \
	done
	@echo "All dependencies found."

# --- Clone uWebSockets and its submodules ---
$(UWS_DIR):
	git clone --recurse-submodules $(UWS_REPO)

# --- Build uSockets ---
$(UWS_DIR)/uSockets/uSockets.a: $(UWS_DIR)
	$(MAKE) -C $(UWS_DIR) WITH_OPENSSL=1 examples

# --- Lazy eval to avoid early expansion ---
OBJECTS = $(shell find $(UWS_DIR)/uSockets -type f -name '*.o' ! -path '*testdata*')

# --- Build target ---
$(TARGET): check-deps $(UWS_DIR)/uSockets/uSockets.a main.cpp
	@echo "Compiling main.cpp with uWebSockets..."
	$(CXX) $(CXXFLAGS) main.cpp $(OBJECTS) $(UWS_DIR)/uSockets/uSockets.a -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# --- Clean ---
clean:
	rm -f *.o
	rm -f $(TARGET)

# --- Phony rules ---
.PHONY: clean check-deps $(TARGET)
