FROM rticom/connext-sdk:7.3.0-EAR as build-stage

# For armv8 uncomment the following line
# ENV CONNEXTDDS_ARCH=armv8Linux4gcc7.3.0

WORKDIR /app

# Copy the necessary files to the container
COPY rti_license.dat ${NDDSHOME}
COPY CMakeLists.txt .
COPY home_automation.idl .
COPY *.cxx .
COPY *.xml .

# Build the applications
RUN mkdir build
WORKDIR /app/build
RUN cmake ..
RUN make -j4

# Copy the built applications to a new container
FROM rticom/connext-runtime:7.3.0-EAR as final-stage

WORKDIR /app
COPY --chown=rtiuser:rtigroup --from=build-stage ${NDDSHOME}/rti_license.dat ${NDDSHOME}
COPY --chown=rtiuser:rtigroup --from=build-stage /app/build/ /app
COPY *.xml /app
