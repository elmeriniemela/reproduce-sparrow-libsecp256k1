The source for the library is taken from the https://github.com/bitcoin-s/secp256k1-zkp/ repo, which as you have noted is a fork of BlockstreamResearch/secp256k1-zkp. The instructions for reproducing the Linux x86_64 binary are below.

The secp256k1 libraries currently bundled in drongo are due to be removed in the next six months by introducing [secp256k1-jdk](https://github.com/bitcoinj/secp256k1-jdk) as a dependency. Version v0.4.0 of that library will bundle the native libraries. I know the maintainer @msgilligan is a fan of reproducibility, so I am sure there will be documentation to reproduce the bundled native libs. This will conclude a long term plan of upgrading the libsecp256k1 dependency in Sparrow, which included the upgrade to JDK 25 earlier in the year for modern FFM support.

# Reproducing `src/main/resources/native/linux/x64/libsecp256k1.so`

SHA256: `61ca7ca3090552417b946ddc537135c9b18b01cea6f4cae9883b0fb6f501146f`

Verified byte-for-byte identical (including the GNU build-id) with the recipe below.

## Source

    https://github.com/bitcoin-s/secp256k1-zkp
    branch 2021-05-19-jni-mac-big-sur-m1
    commit 6dd724b72bb2d47de514eb92e96c6ae6d26ae160  ("Fixed minor nits", 2021-04-19)

That branch is exactly `BlockstreamResearch/secp256k1-zkp` commit `f3708a1ecb445b1b05a0f8fcd1da6a88f83d89c4` plus 5 JNI commits.  The JNI C sources are in-tree at `src/java/org_bitcoin_NativeSecp256k1.c` and `src/java/org_bitcoin_Secp256k1Context.c` — no external JNI source exists.

(The branch tip `4a7cf9e` only hardcodes a macOS java path in `build-aux/m4/ax_jni_include_dir.m4`; it produces the same Linux binary.)

```sh
git subtree add --prefix=secp256k1-zkp https://github.com/bitcoin-s/secp256k1-zkp.git 6dd724b72bb2d47de514eb92e96c6ae6d26ae160 --squash
```

## Toolchain (Ubuntu 20.04 focal)

    gcc-9      9.3.0-17ubuntu1~20.04   (focal-security, superseded 2022-03; on snapshot.ubuntu.com)
    binutils   2.34-6ubuntu1.x
    libc6-dev  2.31-0ubuntu9.x         (headers identical across 9 .. 9.18)
    libtool    2.4.6-14
    a JDK for jni.h (JDK version does not affect the output)

## Build

    export JAVA_HOME=/path/to/jdk
    ./autogen.sh
    ./configure --enable-experimental \
                --enable-module-ecdh \
                --enable-module-schnorrsig \
                --enable-module-ecdsa-adaptor \
                --enable-jni
    make CFLAGS=-std=c99

    sha256sum .libs/libsecp256k1.so.0.0.0

`CFLAGS` is overridden **on the make command line**, which is the key detail: it discards everything `configure` computed (`-O2 -fvisibility=hidden -std=c89 -pedantic -W... -g`).  The effective per-object compile is therefore:

    gcc -DHAVE_CONFIG_H -I. -I./src -DSECP256K1_BUILD -I./include -I./src \
        -c src/secp256k1.c -fPIC -DPIC -o src/.libs/libsecp256k1_la-secp256k1.o -std=c99

`-fPIC -DPIC` come from libtool; `-fcf-protection=full` and `-fstack-protector-strong` are Ubuntu gcc defaults.

Evidence that each of these is required, not guessed:
  * no `.debug_*` sections and `.comment` = gcc 9.3.0  -> no `-g`, exact gcc build
  * `constant_nonce` / `constant_nonce_function` are exported (they carry no
    `SECP256K1_API`)                                   -> no `-fvisibility=hidden`
  * unoptimized code                                   -> no `-O2`
  * gcc's static-variable UID suffixes in `.symtab`
    (`beta.3551`, `fe_1.3511`, `g1.3215`, ...) pin the language dialect exactly:
    c89->3234, iso9899:199409->3247, **c99->3551**, c11->3562, gnu89->4345, gnu11->4355

Everything else is defaults: `--with-bignum=no` and `--with-asm=x86_64` fall out of `auto` (no gmp headers present -> the binary needs only `libc.so.6`), ecmult static precomputation on, window 15, gen precision 4, recovery module off.
