# Reproducing Sparrow's `libsecp256k1.so`

This repository reproduces the Linux x86_64 library bundled at
[sparrow/drongo/src/main/resources/native/linux/x64/libsecp256k1.so](https://github.com/sparrowwallet/drongo/blob/master/src/main/resources/native/linux/x64/libsecp256k1.so)

Expected SHA-256:

```text
61ca7ca3090552417b946ddc537135c9b18b01cea6f4cae9883b0fb6f501146f
```

The result is byte-for-byte identical, including the GNU build ID.
Based on https://github.com/sparrowwallet/drongo/issues/52#issuecomment-5475925956

## Source

- Repository: `https://github.com/bitcoin-s/secp256k1-zkp`
- Branch: `2021-05-19-jni-mac-big-sur-m1`
- Commit: `6dd724b72bb2d47de514eb92e96c6ae6d26ae160`
- Local source: `secp256k1-zkp/`

This revision is BlockstreamResearch/secp256k1-zkp commit `f3708a1ecb445b1b05a0f8fcd1da6a88f83d89c4` plus five JNI commits. The JNI sources are included under `src/java/`.

## Reproduce

Install Podman. Build the self-contained builder image once while online:

```sh
podman build --tag localhost/sparrow-secp256k1-builder .
```

The image contains the source, build script, and complete toolchain. Reproduce the
library without network access:

```sh
podman run --rm --network=none \
  --volume "$PWD:/output:Z" \
  localhost/sparrow-secp256k1-builder
```

The script prints the SHA-256 and writes:

```text
./reproduced-libsecp256k1.so
```

### Preserve the builder for offline use

Export the fully provisioned image and record its checksum:

```sh
podman save --format oci-archive \
  --output secp256k1-builder.oci.tar \
  localhost/sparrow-secp256k1-builder
sha256sum secp256k1-builder.oci.tar \
  > secp256k1-builder.oci.tar.sha256
```

Run the build from the saved oci-archive:

```sh
sha256sum --check secp256k1-builder.oci.tar.sha256
podman load --input secp256k1-builder.oci.tar
podman run --rm --network=none \
  --volume "$PWD:/output:Z" \
  localhost/sparrow-secp256k1-builder
```

Get a shell inside:

```sh
podman run --rm -it --entrypoint /bin/bash localhost/sparrow-secp256k1-builder
```


## Toolchain

The reference binary was built on Ubuntu 20.04 x86_64 with:

| Package | Version |
| --- | --- |
| gcc-9 | `9.3.0-17ubuntu1~20.04` |
| binutils | `2.34-6ubuntu1.x` |
| libc6-dev | `2.31-0ubuntu9.x` |
| libtool | `2.4.6-14` |
| JDK | Any version providing `jni.h` |

The exact GCC build is required for identical code generation, symbol names, default hardening flags, and the final hash. The five `.deb` files in the repository were downloaded from the original [Canonical Launchpad build](https://launchpad.net/~ubuntu-toolchain-r/+archive/ubuntu/ppa/+build/19784872).

To download them again manually:

```sh
launchpad=https://launchpad.net/~ubuntu-toolchain-r/+archive/ubuntu/ppa
base=$launchpad/+build/19784872/+files
wget "$base/cpp-9_9.3.0-17ubuntu1~20.04_amd64.deb"
wget "$base/gcc-9-base_9.3.0-17ubuntu1~20.04_amd64.deb"
wget "$base/libasan5_9.3.0-17ubuntu1~20.04_amd64.deb"
wget "$base/libgcc-9-dev_9.3.0-17ubuntu1~20.04_amd64.deb"
wget "$base/gcc-9_9.3.0-17ubuntu1~20.04_amd64.deb"

changes=https://launchpadlibrarian.net/492520884
wget -qO- \
  "$changes/gcc-9_9.3.0-17ubuntu1~20.04_amd64.changes" |
  awk '
    $1 == "Checksums-Sha256:" { sha256 = 1; next }
    $1 == "Files:" { sha256 = 0 }
    sha256 &&
      $3 ~ /^(cpp-9|gcc-9-base|libasan5|libgcc-9-dev|gcc-9)_/ &&
      $3 ~ /_amd64\.deb$/ {
      print $1 "  " $3
    }
  ' > SHA256SUMS

sha256sum --check SHA256SUMS
```

### Required build settings

The configure options are:

```sh
./configure CC=gcc-9 \
  --enable-experimental \
  --enable-module-ecdh \
  --enable-module-schnorrsig \
  --enable-module-ecdsa-adaptor \
  --enable-jni
```

Compilation must use this command-line override:

```sh
make CFLAGS=-std=c99
```

Supplying `CFLAGS` to `make` is essential. It discards configure's optimization, visibility, warning, and debug flags, producing the unoptimized C99 binary without debug sections. Libtool adds `-fPIC -DPIC`, while the Ubuntu GCC package supplies `-fcf-protection=full` and `-fstack-protector-strong` by default.

The remaining settings are auto-detected as `bignum=no`, `asm=x86_64`, ecmult window size 15, and ecmult generator precision 4. The recovery module is disabled.
