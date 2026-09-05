#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "org_bitcoin_NativeSecp256k1.h"
#include "include/secp256k1.h"
#include "include/secp256k1_ecdh.h"
#include "include/secp256k1_recovery.h"
#include "include/secp256k1_schnorrsig.h"
#include "include/secp256k1_extrakeys.h"
#include "include/secp256k1_ecdsa_adaptor.h"


SECP256K1_API jlong JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ctx_1clone
  (JNIEnv* env, jclass classObject, jlong ctx_l)
{
  const secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;

  jlong ctx_clone_l = (uintptr_t) secp256k1_context_clone(ctx);

  (void)classObject;(void)env;

  return ctx_clone_l;

}

SECP256K1_API jint JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1context_1randomize
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;

  const unsigned char* seed = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);

  (void)classObject;

  return secp256k1_context_randomize(ctx, seed);

}

SECP256K1_API void JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1destroy_1context
  (JNIEnv* env, jclass classObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;

  secp256k1_context_destroy(ctx);

  (void)classObject;(void)env;
}

SECP256K1_API jint JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ecdsa_1verify
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint siglen, jint publen)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;

  unsigned char* data = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);
  const unsigned char* sigdata = {  (unsigned char*) (data + 32) };
  const unsigned char* pubdata = { (unsigned char*) (data + siglen + 32) };

  secp256k1_ecdsa_signature sig;
  secp256k1_pubkey pubkey;

  int ret = secp256k1_ecdsa_signature_parse_der(ctx, &sig, sigdata, siglen);

  if( ret ) {
    ret = secp256k1_ec_pubkey_parse(ctx, &pubkey, pubdata, publen);

    if( ret ) {
      ret = secp256k1_ecdsa_verify(ctx, &sig, data, &pubkey);
    }
  }

  (void)classObject;

  return ret;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ecdsa_1sign
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* data = (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char* secKey = (unsigned char*) (data + 32);

  jobjectArray retArray;
  jbyteArray sigArray, intsByteArray;
  unsigned char intsarray[2];

  secp256k1_ecdsa_signature sig;

  int ret = secp256k1_ecdsa_sign(ctx, &sig, data, secKey, NULL, NULL);

  unsigned char outputSer[72];
  size_t outputLen = 72;

  if( ret ) {
    int ret2 = secp256k1_ecdsa_signature_serialize_der(ctx,outputSer, &outputLen, &sig ); (void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  sigArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, sigArray, 0, outputLen, (jbyte*)outputSer);
  (*env)->SetObjectArrayElement(env, retArray, 0, sigArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ecdsa_1sign_1with_1entropy
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* data = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char* secKey = (unsigned char*) (data + 32);
  unsigned char* entropy = (unsigned char*) (secKey + 32);

  jobjectArray retArray;
  jbyteArray sigArray, intsByteArray;
  unsigned char intsarray[2];

  secp256k1_ecdsa_signature sig;

  int ret = secp256k1_ecdsa_sign(ctx, &sig, data, secKey, NULL, entropy);

  unsigned char outputSer[72];
  size_t outputLen = 72;

  if( ret ) {
    int ret2 = secp256k1_ecdsa_signature_serialize_der(ctx,outputSer, &outputLen, &sig ); (void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  sigArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, sigArray, 0, outputLen, (jbyte*)outputSer);
  (*env)->SetObjectArrayElement(env, retArray, 0, sigArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jint JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ec_1seckey_1verify
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* secKey = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);

  (void)classObject;

  return secp256k1_ec_seckey_verify(ctx, secKey);
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ec_1pubkey_1create
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jboolean compressed)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  const unsigned char* secKey = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);

  secp256k1_pubkey pubkey;

  jobjectArray retArray;
  jbyteArray pubkeyArray, intsByteArray;
  unsigned char intsarray[2];

  int ret = secp256k1_ec_pubkey_create(ctx, &pubkey, secKey);

  unsigned char outputSer[65];
  size_t outputLen = 65;

  if( ret ) {
    int ret2 = secp256k1_ec_pubkey_serialize(ctx,outputSer, &outputLen, &pubkey, compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);(void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  pubkeyArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, pubkeyArray, 0, outputLen, (jbyte*)outputSer);
  (*env)->SetObjectArrayElement(env, retArray, 0, pubkeyArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;

}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1privkey_1tweak_1add
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* privkey = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);
  const unsigned char* tweak = (unsigned char*) (privkey + 32);

  jobjectArray retArray;
  jbyteArray privArray, intsByteArray;
  unsigned char intsarray[2];

  int privkeylen = 32;

  int ret = secp256k1_ec_privkey_tweak_add(ctx, privkey, tweak);

  intsarray[0] = privkeylen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  privArray = (*env)->NewByteArray(env, privkeylen);
  (*env)->SetByteArrayRegion(env, privArray, 0, privkeylen, (jbyte*)privkey);
  (*env)->SetObjectArrayElement(env, retArray, 0, privArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1privkey_1tweak_1mul
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* privkey = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);
  const unsigned char* tweak = (unsigned char*) (privkey + 32);

  jobjectArray retArray;
  jbyteArray privArray, intsByteArray;
  unsigned char intsarray[2];

  int privkeylen = 32;

  int ret = secp256k1_ec_privkey_tweak_mul(ctx, privkey, tweak);

  intsarray[0] = privkeylen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  privArray = (*env)->NewByteArray(env, privkeylen);
  (*env)->SetByteArrayRegion(env, privArray, 0, privkeylen, (jbyte*)privkey);
  (*env)->SetObjectArrayElement(env, retArray, 0, privArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1pubkey_1tweak_1add
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint publen, jboolean compressed)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* pkey = (*env)->GetDirectBufferAddress(env, byteBufferObject);
  const unsigned char* tweak = (unsigned char*) (pkey + publen);

  jobjectArray retArray;
  jbyteArray pubArray, intsByteArray;
  unsigned char intsarray[2];
  unsigned char outputSer[65];
  size_t outputLen = 65;

  secp256k1_pubkey pubkey;
  int ret = secp256k1_ec_pubkey_parse(ctx, &pubkey, pkey, publen);

  if( ret ) {
    ret = secp256k1_ec_pubkey_tweak_add(ctx, &pubkey, tweak);
  }

  if( ret ) {
    int ret2 = secp256k1_ec_pubkey_serialize(ctx,outputSer, &outputLen, &pubkey, compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);(void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  pubArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, pubArray, 0, outputLen, (jbyte*)outputSer);
  (*env)->SetObjectArrayElement(env, retArray, 0, pubArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1pubkey_1tweak_1mul
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint publen, jboolean compressed)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* pkey = (*env)->GetDirectBufferAddress(env, byteBufferObject);
  const unsigned char* tweak = (unsigned char*) (pkey + publen);

  jobjectArray retArray;
  jbyteArray pubArray, intsByteArray;
  unsigned char intsarray[2];
  unsigned char outputSer[65];
  size_t outputLen = 65;

  secp256k1_pubkey pubkey;
  int ret = secp256k1_ec_pubkey_parse(ctx, &pubkey, pkey, publen);

  if ( ret ) {
    ret = secp256k1_ec_pubkey_tweak_mul(ctx, &pubkey, tweak);
  }

  if( ret ) {
    int ret2 = secp256k1_ec_pubkey_serialize(ctx,outputSer, &outputLen, &pubkey, compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);(void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  pubArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, pubArray, 0, outputLen, (jbyte*)outputSer);
  (*env)->SetObjectArrayElement(env, retArray, 0, pubArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ec_1pubkey_1combine
  (JNIEnv * env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint publen, jint jnumkeys, jboolean compressed)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* pkeys = (*env)->GetDirectBufferAddress(env, byteBufferObject);

  const size_t numkeys = jnumkeys;
  secp256k1_pubkey sumkey;
  const secp256k1_pubkey *pubkeys[numkeys];
  secp256k1_pubkey pubkeysParsed[numkeys];
  int ret = 1;
  size_t i;

  jobjectArray retArray;
  jbyteArray pubArray, intsByteArray;
  unsigned char intsarray[2];
  unsigned char outputSer[65];
  size_t outputLen = 65;

  for (i = 0; i < numkeys; i++)
  {
    unsigned char* pkey = pkeys + (i*publen);
    
    ret = secp256k1_ec_pubkey_parse(ctx, &pubkeysParsed[i], pkey, publen);
    pubkeys[i] = &pubkeysParsed[i];

    if ( !ret ) {
      break;
    }
  }

  if ( ret ) {
    ret = secp256k1_ec_pubkey_combine(ctx, &sumkey, pubkeys, numkeys);
  }

  if ( ret ) {
    int ret2 = secp256k1_ec_pubkey_serialize(ctx, outputSer, &outputLen, &sumkey, compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);(void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  pubArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, pubArray, 0, outputLen, (jbyte*)outputSer);
  (*env)->SetObjectArrayElement(env, retArray, 0, pubArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ec_1pubkey_1decompress
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint publen)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  const unsigned char* pubdata = (*env)->GetDirectBufferAddress(env, byteBufferObject);

  secp256k1_pubkey pubkey;

  jobjectArray retArray;
  jbyteArray pubkeyArray, intsByteArray;
  unsigned char intsarray[2];

  int ret = secp256k1_ec_pubkey_parse(ctx, &pubkey, pubdata, publen);

  unsigned char outputSer[65];
  size_t outputLen = 65;

  if( ret ) {
    int ret2 = secp256k1_ec_pubkey_serialize(ctx,outputSer, &outputLen, &pubkey, SECP256K1_EC_UNCOMPRESSED);(void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  pubkeyArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, pubkeyArray, 0, outputLen, (jbyte*)outputSer);
  (*env)->SetObjectArrayElement(env, retArray, 0, pubkeyArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ecdh
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint publen)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  const unsigned char* secdata = (*env)->GetDirectBufferAddress(env, byteBufferObject);
  const unsigned char* pubdata = (const unsigned char*) (secdata + 32);

  jobjectArray retArray;
  jbyteArray outArray, intsByteArray;
  unsigned char intsarray[1];
  secp256k1_pubkey pubkey;
  unsigned char nonce_res[32];
  size_t outputLen = 32;

  int ret = secp256k1_ec_pubkey_parse(ctx, &pubkey, pubdata, publen);

  if (ret) {
    ret = secp256k1_ecdh(
      ctx,
      nonce_res,
      &pubkey,
      secdata,
      NULL,
      NULL
    );
  }

  intsarray[0] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  outArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, outArray, 0, 32, (jbyte*)nonce_res);
  (*env)->SetObjectArrayElement(env, retArray, 0, outArray);

  intsByteArray = (*env)->NewByteArray(env, 1);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 1, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1schnorrsig_1sign
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* msg32 = (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char* seckey = (unsigned char*)(msg32 + 32);
  unsigned char* rand32 = (unsigned char*)(seckey + 32);

  jobjectArray retArray;
  jbyteArray sigArray, intsByteArray;
  unsigned char intsarray[1];
  secp256k1_keypair keypair;
  unsigned char sig[64];

  int ret = secp256k1_keypair_create(ctx, &keypair, seckey);

  if (ret) {
    ret = secp256k1_schnorrsig_sign(ctx, &sig, msg32, &keypair, NULL, rand32);
  }

  intsarray[0] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  sigArray = (*env)->NewByteArray(env, 64);
  (*env)->SetByteArrayRegion(env, sigArray, 0, 64, (jbyte*)sig);
  (*env)->SetObjectArrayElement(env, retArray, 0, sigArray);

  intsByteArray = (*env)->NewByteArray(env, 1);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 1, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

int constant_nonce_function(unsigned char *nonce32, const unsigned char *msg32, const unsigned char *key32, const unsigned char *xonly_pk32, const unsigned char *algo16, void *data) {
  memcpy(nonce32, (const unsigned char*)data, 32);
  return 1;
}

const secp256k1_nonce_function_hardened constant_nonce = constant_nonce_function;

SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1schnorrsig_1sign_1with_1nonce
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* msg32 = (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char* seckey = (unsigned char*)(msg32 + 32);
  unsigned char* nonce = (unsigned char*)(seckey + 32);

  jobjectArray retArray;
  jbyteArray sigArray, intsByteArray;
  unsigned char intsarray[1];
  secp256k1_keypair keypair;
  unsigned char sig[64];

  int ret = secp256k1_keypair_create(ctx, &keypair, seckey);

  if (ret) {
    ret = secp256k1_schnorrsig_sign(ctx, &sig, msg32, &keypair, constant_nonce, nonce);
  }

  intsarray[0] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  sigArray = (*env)->NewByteArray(env, 64);
  (*env)->SetByteArrayRegion(env, sigArray, 0, 64, (jbyte*)sig);
  (*env)->SetObjectArrayElement(env, retArray, 0, sigArray);

  intsByteArray = (*env)->NewByteArray(env, 1);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 1, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}
/*
SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1schnorrsig_1compute_1sigpoint
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jboolean compressed)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* msg32 = (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char* nonce32 = (unsigned char*)(msg32 + 32);
  unsigned char* pubkey32 = (unsigned char*)(nonce32 + 32);

  secp256k1_xonly_pubkey pubkey;
  secp256k1_xonly_pubkey nonce;
  secp256k1_pubkey sigpoint;
  unsigned char intsarray[2];
  jobjectArray retArray;
  jbyteArray pointArray, intsByteArray;
  unsigned char outputSer[65];
  size_t outputLen = 65;

  int ret = secp256k1_xonly_pubkey_parse(ctx, &pubkey, pubkey32);

  if (ret) {
    ret = secp256k1_xonly_pubkey_parse(ctx, &nonce, nonce32);
  }

  if (ret) {
    ret = secp256k1_schnorrsig_compute_sigpoint(ctx, &sigpoint, msg32, &nonce, &pubkey);
  }

  if( ret ) {
    int ret2 = secp256k1_ec_pubkey_serialize(ctx, outputSer, &outputLen, &sigpoint, compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);(void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  pointArray = (*env)->NewByteArray(env, outputLen);
    (*env)->SetByteArrayRegion(env, pointArray, 0, outputLen, (jbyte*)outputSer);
    (*env)->SetObjectArrayElement(env, retArray, 0, pointArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
    (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
    (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}
*/
SECP256K1_API jint JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1schnorrsig_1verify
  (JNIEnv* env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* sig64 = (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char* msg32 = (unsigned char*)(sig64 + 64);
  unsigned char* pubx32 = (unsigned char*)(msg32 + 32);

  secp256k1_xonly_pubkey pubx;

  int ret = secp256k1_xonly_pubkey_parse(ctx, &pubx, pubx32);

  if (ret) {
    ret = secp256k1_schnorrsig_verify(ctx, sig64, msg32, &pubx);
  }

  (void)classObject;

  return ret;
}

/*
 * Class:     org_bitcoin_NativeSecp256k1
 * Method:    secp256k1_ecdsa_adaptor_sign
 * Signature: (Ljava/nio/ByteBuffer;J)[[B
 */
SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ecdsa_1adaptor_1sign
  (JNIEnv *env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint publen)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* seckey32 = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char* adaptor = (unsigned char*) (seckey32 + 32);
  unsigned char* msg32 = (unsigned char*) (adaptor + publen);
  unsigned char* rand32 = (unsigned char*) (msg32 + 32);

  jobjectArray retArray;
  jbyteArray sigArray, intsByteArray;

  unsigned char intsarray[1];
  unsigned char adaptor_sig162[162];

  secp256k1_pubkey adaptorPoint;
  int ret = secp256k1_ec_pubkey_parse(ctx, &adaptorPoint, adaptor, publen);

  if (ret) {
    ret = secp256k1_ecdsa_adaptor_encrypt(ctx, adaptor_sig162, seckey32, &adaptorPoint, msg32, NULL, rand32);
  }

  intsarray[0] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  sigArray = (*env)->NewByteArray(env, 162);
  (*env)->SetByteArrayRegion(env, sigArray, 0, 162, (jbyte*)adaptor_sig162);
  (*env)->SetObjectArrayElement(env, retArray, 0, sigArray);

  intsByteArray = (*env)->NewByteArray(env, 1);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 1, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

/*
 * Class:     org_bitcoin_NativeSecp256k1
 * Method:    secp256k1_ecdsa_adaptor_sig_verify
 * Signature: (Ljava/nio/ByteBuffer;JII)I
 */
SECP256K1_API jint JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ecdsa_1adaptor_1sig_1verify
  (JNIEnv *env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint publen)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char* adaptor_sig162 = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char* pkey = (unsigned char*) (adaptor_sig162 + 162);
  unsigned char* msg32 = (unsigned char*) (pkey + publen);
  unsigned char* adaptor = (unsigned char*) (msg32 + 32);

  secp256k1_pubkey pubKey;
  secp256k1_pubkey adaptorPoint;
  int ret = secp256k1_ec_pubkey_parse(ctx, &pubKey, pkey, publen);
  if (ret) {
    ret = secp256k1_ec_pubkey_parse(ctx, &adaptorPoint, adaptor, publen);
  }

  (void)classObject;

  if (ret) {
    ret = secp256k1_ecdsa_adaptor_verify(ctx, adaptor_sig162, &pubKey, msg32, &adaptorPoint);
  }

  return ret;
}

/*
 * Class:     org_bitcoin_NativeSecp256k1
 * Method:    secp256k1_ecdsa_adaptor_adapt
 * Signature: (Ljava/nio/ByteBuffer;J)[[B
 */
SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ecdsa_1adaptor_1adapt
  (JNIEnv *env, jclass classObject, jobject byteBufferObject, jlong ctx_l)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char *adaptor_secret32 = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char *adaptor_sig162 = (unsigned char*) (adaptor_secret32 + 32);

  jobjectArray retArray;
  jbyteArray sigArray, intsByteArray;

  unsigned char intsarray[2];
  secp256k1_ecdsa_signature sig;

  int ret = secp256k1_ecdsa_adaptor_decrypt(ctx, &sig, adaptor_secret32, adaptor_sig162);

  unsigned char outputSer[72];
  size_t outputLen = 72;

  if (ret) {
    int ret2 = secp256k1_ecdsa_signature_serialize_der(ctx, outputSer, &outputLen, &sig ); (void)ret2;
  }

  intsarray[0] = outputLen;
  intsarray[1] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  sigArray = (*env)->NewByteArray(env, outputLen);
  (*env)->SetByteArrayRegion(env, sigArray, 0, outputLen, (jbyte*)outputSer);
  (*env)->SetObjectArrayElement(env, retArray, 0, sigArray);

  intsByteArray = (*env)->NewByteArray(env, 2);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 2, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}

/*
 * Class:     org_bitcoin_NativeSecp256k1
 * Method:    secp256k1_ecdsa_adaptor_extract_secret
 * Signature: (Ljava/nio/ByteBuffer;J)[[B
 */
SECP256K1_API jobjectArray JNICALL Java_org_bitcoin_NativeSecp256k1_secp256k1_1ecdsa_1adaptor_1extract_1secret
  (JNIEnv *env, jclass classObject, jobject byteBufferObject, jlong ctx_l, jint siglen, jint publen)
{
  secp256k1_context *ctx = (secp256k1_context*)(uintptr_t)ctx_l;
  unsigned char *sig_bytes = (unsigned char*) (*env)->GetDirectBufferAddress(env, byteBufferObject);
  unsigned char *adaptor_sig162 = (unsigned char*) (sig_bytes + siglen);
  unsigned char *adaptor = (unsigned char*) (adaptor_sig162 + 162);

  jobjectArray retArray;
  jbyteArray secArray, intsByteArray;

  unsigned char intsarray[1];
  unsigned char adaptor_secret32[32];

  secp256k1_pubkey adaptorPoint;
  secp256k1_ecdsa_signature sig;
  int ret = secp256k1_ec_pubkey_parse(ctx, &adaptorPoint, adaptor, publen);
  if (ret) {
    ret = secp256k1_ecdsa_signature_parse_der(ctx, &sig, sig_bytes, siglen);
  }

  if (ret) {
    ret = secp256k1_ecdsa_adaptor_recover(ctx, adaptor_secret32, &sig, adaptor_sig162, &adaptorPoint);
  }

  intsarray[0] = ret;

  retArray = (*env)->NewObjectArray(env, 2,
    (*env)->FindClass(env, "[B"),
    (*env)->NewByteArray(env, 1));

  secArray = (*env)->NewByteArray(env, 32);
  (*env)->SetByteArrayRegion(env, secArray, 0, 32, (jbyte*)adaptor_secret32);
  (*env)->SetObjectArrayElement(env, retArray, 0, secArray);

  intsByteArray = (*env)->NewByteArray(env, 1);
  (*env)->SetByteArrayRegion(env, intsByteArray, 0, 1, (jbyte*)intsarray);
  (*env)->SetObjectArrayElement(env, retArray, 1, intsByteArray);

  (void)classObject;

  return retArray;
}