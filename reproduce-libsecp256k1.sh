#!/usr/bin/env bash
set -euxo pipefail

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
cp .libs/libsecp256k1.so.0.0.0 /output/reproduced-libsecp256k1.so
sha256sum /output/reproduced-libsecp256k1.so
