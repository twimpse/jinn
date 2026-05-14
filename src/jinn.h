
// sha512(md5sum(md5sum("zero"))
#DEFINE CODE_KEY "xAA\xBB\xCC\xDD\xEE\xFF"
#DEFINE CODE_BUFFER_BOOKEND "\x00000000,\x00000000,\x00000000\x00000000,\x00000000,\x00000000,\x00000000\x00000000"
#DEFINE VERSION 0.0.1-pre-beta-dev
#DEFINE LISTEN_PORT 17283
#DEFINE MAGIC_8_BALL_MODE "genie"
#DEFINE TRANSPARENT_PROXY "dp"
#DEFINE ADMIN_USERNAME "controller"
#DEFINE ADMIN_PASSWORD "encryptedstring" 
// how large do we make our custom dsta section in the .rodata and .data parts. keep about 1024 bits as low entropy. 
#DEFINE SECTION_BUFFER_SIZE 4096
