void genie_mode(void);
void datapipe_mode(void);
void shell_mode(void);
void command_mode(void);
void remote_shell_mode(const char *host, int port);
void daemonize(void);
void init_logging(const char *log_path);
void derive_key_from_password(const char *password, unsigned char *key,
                              unsigned char *iv);

// Encryption functions from earlier
int encrypt_data(const unsigned char *plaintext, int plaintext_len,
                 const unsigned char *key, const unsigned char *iv,
                 unsigned char *ciphertext);
int decrypt_data(const unsigned char *ciphertext, int ciphertext_len,
                 const unsigned char *key, const unsigned char *iv,
                 unsigned char *plaintext);
