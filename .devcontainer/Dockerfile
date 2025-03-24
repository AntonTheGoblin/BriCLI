FROM ubuntu:24.04

RUN apt-get update && apt-get install -y build-essential git cmake

# Install Catch2
RUN git clone https://github.com/catchorg/Catch2.git --branch v3.4.0 --single-branch && \
	cd Catch2 && \
	cmake -B build -H. -DBUILD_TESTING=OFF && \
	cmake --build build/ --target install && \
	cd .. && \
	rm -rf Catch2
