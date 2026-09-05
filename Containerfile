FROM docker.io/library/ubuntu:20.04

COPY SHA256SUMS *.deb /tmp/
RUN cd /tmp && \
    sha256sum --check SHA256SUMS && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        --no-install-recommends --allow-downgrades \
        autoconf automake libc6-dev libtool make openjdk-11-jdk-headless \
        /tmp/*.deb && \
    rm -rf /var/lib/apt/lists/* /tmp/*.deb /tmp/SHA256SUMS

ENV JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64
WORKDIR /builder
COPY reproduce-libsecp256k1.sh ./
COPY secp256k1-zkp ./secp256k1-zkp

VOLUME ["/output"]
ENTRYPOINT ["./reproduce-libsecp256k1.sh"]
