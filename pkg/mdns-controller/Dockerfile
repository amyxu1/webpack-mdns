FROM ubuntu:20.04 as baseline

RUN apt-get update && apt-get install -y libprotobuf-dev net-tools g++ cmake tcpdump iproute2

COPY webpacks/ /root/webpack-mdns/webpacks
COPY run_scripts /root/webpack-mdns/run_scripts

FROM baseline as builder

# Move project files into container
COPY . /root/webpack-mdns

# Build the project
WORKDIR /root/webpack-mdns
RUN rm -rf build
RUN mkdir build
WORKDIR /root/webpack-mdns/build
RUN cmake ../
RUN make -j

# Copy built binaries to an output image
FROM baseline
COPY --from=builder /root/webpack-mdns/build/src/mdns-controller /root/webpack-mdns/run_scripts
