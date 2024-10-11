# Copy the built applications to a new container
FROM connext-sdk:latest as final-stage

WORKDIR /app
COPY --from=connext-sdk /opt/rti.com/rti_connext_dds-7.3.0/rti_license.dat ./
COPY --from=connext-sdk /app/build/ ./
COPY *.xml ./
