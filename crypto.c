#include <openssl/evp.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <string.h>

#define NUM_ITERS 65536
#define KEY_SIZE_BYTES 16
#define IV_SIZE_BYTES 16
#define SALT "l1pWK8SAkDR4UtE1uk9pPIc1qEMlxq7pwIJaoV1W4dspCYlnbs"


void crypto_init(){
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
}

void crypto_deinit(){
    EVP_cleanup();
    ERR_free_strings();
}

int generateKey(const char *pass, const char *salt, char *out){
    return PKCS5_PBKDF2_HMAC_SHA1(pass, -1,
                (unsigned char *) salt, strlen(salt) + 1, NUM_ITERS, 
                KEY_SIZE_BYTES, (unsigned char *) out);
}

char *getEncryptedBody(char *message){
    return message + IV_SIZE_BYTES;
}

int decrypt(char *message, char *password, char *plaintext){
  EVP_CIPHER_CTX *ctx;
  int len;
  int plaintext_len;
  char key[KEY_SIZE_BYTES];
  int status = generateKey(password, SALT, key);
  if(!status){
    return -1;
  }
  const char *ciphertext = getEncryptedBody(message);


  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())){
    return -1;
  } 
  

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (unsigned char *) key, (unsigned char *) message)){
    return -1;
  }

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, (unsigned char *) plaintext, &len, (unsigned char *) ciphertext, strlen(ciphertext))){
    return -1;
  }

  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, (unsigned char *) plaintext + len, &len)){
    return -1;
  } 
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}
