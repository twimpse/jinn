#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>

// Encrypt function
int encrypt_data(const unsigned char *plaintext, int plaintext_len,
                 const unsigned char *key, const unsigned char *iv,
                 unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;
  int len, ciphertext_len;

  ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);
  EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
  ciphertext_len = len;
  EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
  ciphertext_len += len;
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

// Decrypt function
int decrypt_data(const unsigned char *ciphertext, int ciphertext_len,
                 const unsigned char *key, const unsigned char *iv,
                 unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;
  int len, plaintext_len;

  ctx = EVP_CIPHER_CTX_new();
  EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);
  EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
  plaintext_len = len;
  EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
  plaintext_len += len;
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}
