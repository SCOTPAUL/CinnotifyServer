#pragma once

#include <openssl/evp.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <string.h>
#include <stdint.h>

void crypto_init(void);
void crypto_deinit(void); 

/**
 * Decrypts a message which was encrypted using AES/CBC/PKCS5Padding and where the key was
 * generated with PBKDF2WithHmacSHA1.
 *
 * \param message the ciphertext message prefixed with the IV
 * \param message_size the size of the full message, including IV
 * \param password the password used for the encryption
 * \param plaintext where the resultant plaintext is placed on success.
 *                  Must be at least message_size + 1 chars long.
 *
 * \return the size of plaintext message on success, or -1 otherwise
 */
int decrypt(char *message, uint32_t message_size, char *password, char *plaintext);
