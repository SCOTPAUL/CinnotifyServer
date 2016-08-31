#pragma once

void crypto_init(void);
void crypto_deinit(void); 

int decrypt(unsigned char *message, unsigned char *password, unsigned char *plaintext);
