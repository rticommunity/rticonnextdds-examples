FROM ubuntu:20.04 as build-stage

ARG RTI_LICENSE_AGREEMENT_ACCEPTED=false

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

COPY rti_license.dat /opt/rti.com/rti_connext_dds-7.3.0/
RUN git clone --recurse-submodule https://github.com/rticommunity/rticonnextdds-examples.git