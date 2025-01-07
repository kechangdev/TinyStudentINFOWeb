# Stage 1: Build Stage
FROM ubuntu:22.04 AS builder

# Set environment variables to non-interactive to avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies and tools required to install CMake manually
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    git \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# Install CMake 3.30 or higher
ENV CMAKE_VERSION=3.30.0

RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz \
    && tar -zxvf cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz \
    && mv cmake-${CMAKE_VERSION}-linux-x86_64 /opt/cmake \
    && ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake \
    && rm cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz

# Verify CMake installation
RUN cmake --version

# Set the working directory
WORKDIR /app

# Clone the GitHub repository
RUN git clone https://github.com/kechangdev/TinyStudentINFOWeb.git .

# (Optional) Checkout a specific branch or tag
# RUN git checkout <branch-or-tag>

# Create and move into the build directory
RUN mkdir build
WORKDIR /app/build

# Configure the project using CMake
RUN cmake ..

# Build the project using all available CPU cores
RUN make -j$(nproc)

# Stage 2: Runtime Stage
FROM ubuntu:22.04

# Set environment variables to non-interactive
ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user for running the application
RUN useradd -m appuser

# Set the working directory
WORKDIR /app

# Copy the built executable from the builder stage
COPY --from=builder /app/build/TinyStudentINFOWeb .

# Copy configuration files and necessary directories
COPY --from=builder /app/conf.h .
COPY --from=builder /app/db/ db/
COPY --from=builder /app/www/ www/

# Change ownership to the non-root user
RUN chown -R appuser:appuser /app

# Switch to the non-root user
USER appuser

# Expose the port your application listens on (replace 8080 with your actual port)
EXPOSE 10086

# Define the entry point to run your application
CMD ["./TinyStudentINFOWeb"]
