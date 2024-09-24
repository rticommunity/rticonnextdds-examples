ARG RTI_LICENSE_AGREEMENT_ACCEPTED=false
FROM ubuntu:20.04 as build-stage

ARG RTI_LICENSE_AGREEMENT_ACCEPTED

# Install the required packages
RUN export DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
    && apt-get install -y \
    build-essential \
    cmake \
    curl \
    git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists

# Install prerequisites
RUN curl -sSL -o /usr/share/keyrings/rti-official-archive.gpg \
    https://packages.rti.com/deb/official/repo.key
RUN printf -- "deb [arch=%s, signed-by=%s] %s %s main\n" \
    $(dpkg --print-architecture) \
    /usr/share/keyrings/rti-official-archive.gpg \
    https://packages.rti.com/deb/official \
    $(. /etc/os-release && echo ${VERSION_CODENAME}) | \
    tee /etc/apt/sources.list.d/rti-official.list >/dev/null

# Install the RTI Connext Debian Package
RUN export DEBIAN_FRONTEND=noninteractive \
    RTI_LICENSE_AGREEMENT_ACCEPTED=${RTI_LICENSE_AGREEMENT_ACCEPTED} \
    && apt-get update \
    && apt-get install -y rti-connext-dds-7.3.0 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists

# For armv8 uncomment the following line
# ENV CONNEXTDDS_ARCH=armv8Linux4gcc7.3.0
WORKDIR /app

# Copy the necessary files to the container
COPY rti_license.dat /opt/rti.com/rti_connext_dds-7.3.0/
COPY CMakeLists.txt .
COPY home_automation.idl .
COPY *.cxx ./
COPY *.xml ./
RUN git clone --recurse-submodule https://github.com/rticommunity/rticonnextdds-examples.git

# Build the applications
RUN mkdir build
WORKDIR /app/build
RUN cmake .. -DCONNEXTDDS_DIR=/opt/rti.com/rti_connext_dds-7.3.0/
RUN make -j4

# Copy the built applications to a new container
FROM ubuntu:20.04 as final-stage

WORKDIR /app
COPY --from=build-stage /opt/rti.com/rti_connext_dds-7.3.0/rti_license.dat ./
COPY --from=build-stage /app/build/ ./
COPY *.xml ./