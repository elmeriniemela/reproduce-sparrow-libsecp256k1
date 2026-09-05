#!/usr/bin/env bash
set -euxo pipefail

apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y --allow-downgrades \
    autoconf automake libc6-dev libtool make openjdk-11-jdk-headless \
    /work/gcc-9-base_9.3.0-17ubuntu1~20.04_amd64.deb \
    /work/libasan5_9.3.0-17ubuntu1~20.04_amd64.deb \
    /work/cpp-9_9.3.0-17ubuntu1~20.04_amd64.deb \
    /work/libgcc-9-dev_9.3.0-17ubuntu1~20.04_amd64.deb \
    /work/gcc-9_9.3.0-17ubuntu1~20.04_amd64.deb

export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64

cp -a secp256k1-zkp /tmp/secp256k1-zkp
cd /tmp/secp256k1-zkp
./autogen.sh
./configure CC=gcc-9 \
    --enable-experimental \
    --enable-module-ecdh \
    --enable-module-schnorrsig \
    --enable-module-ecdsa-adaptor \
    --enable-jni
make CFLAGS=-std=c99
cp .libs/libsecp256k1.so.0.0.0 /work/reproduced-libsecp256k1.so
sha256sum /work/reproduced-libsecp256k1.so
