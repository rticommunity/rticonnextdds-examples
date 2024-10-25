# For armv8 uncomment the following line
# ENV CONNEXTDDS_ARCH=armv8Linux4gcc7.3.0
FROM connext-sdk:latest as build-stage
WORKDIR /app

# Copy the necessary files to the container
COPY --from=connext-sdk /opt/rti.com/rti_connext_dds-7.3.0 ./opt/rti.com/rti_connext_dds-7.3.0
COPY --from=connext-sdk rticonnextdds-examples ./rticonnextdds-examples
COPY CMakeLists.txt .
COPY home_automation.idl .
COPY *.cxx ./
COPY *.xml ./

# Build the applications
RUN apt-get install -y cmake
RUN mkdir build
WORKDIR /app/build
RUN cmake .. -DCONNEXTDDS_DIR=/opt/rti.com/rti_connext_dds-7.3.0/
RUN make -j4

# Copy the built applications to a new container
FROM connext-sdk:latest as final-stage

WORKDIR /app
COPY --from=connext-sdk /opt/rti.com/rti_connext_dds-7.3.0/rti_license.dat ./
COPY --from=build-stage /app/build/ ./
COPY *.xml ./