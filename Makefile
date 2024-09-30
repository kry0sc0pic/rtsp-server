all:
	@astyle --quiet --options=astylerc src/*.cpp,*.hpp
	@cmake -Bbuild -H.; cmake --build build -j$(nproc)
	@size build/rtsp-server

install:
	@bash install.sh

clean:
	@rm -rf build
	@echo "All build artifacts removed"

.PHONY: all install clean
