FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY . .

RUN mkdir build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)


FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
        libstdc++6 \
        && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /src/build/call-control/vse-control /app/vse-control
COPY --from=builder /src/build/call-network/vse-network /app/vse-network
COPY --from=builder /src/build/billing/vse-billing /app/vse-billing
COPY --from=builder /src/build/simulator/call-simulator /app/call-simulator

CMD ["/app/vse-control"]
