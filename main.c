/**
 * @file main.c
 * Main file for Caesar cipher application.
 */
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int ALPHABET_SIZE = 26;

/**
 * @brief Crypt a single character with the given shift and alphabet base
 * @param shift Number of characters to logically right-shift
 * @param base  Base character of alphabet, i.e. 'a' or 'A'
 * @param c     Character to shift
 * @return Shifted character
 */
static char crypt_char_base(int shift, char base, char c)
{
    // Convert a negative offset so it works properly with %
    shift = ((shift % ALPHABET_SIZE) + ALPHABET_SIZE) % ALPHABET_SIZE;

    int cur_offset = c - base;
    int tgt_offset = (cur_offset + shift) % ALPHABET_SIZE;
    return base + tgt_offset;
}

/**
 * @brief Crypt a single ASCII character
 * @param shift Number of characters to logically right-shift
 * @param c     Character to shift
 * @return Crypted character, or the same character if not alphabetic
 */
static char crypt_char(int shift, char c)
{
    if (c >= 'A' && c <= 'Z') {
        return crypt_char_base(shift, 'A', c);
    }
    if (c >= 'a' && c <= 'z') {
        return crypt_char_base(shift, 'a', c);
    }

    return c;
}

/**
 * @brief Crypt an ASCII string, not changing non-alphabetic characters
 * @param shift Number of characters to logically right-shift
 * @param in    String to crypt
 * @param out   Output buffer to write to. Must be at least as long as @in
 * @return @out
 */
static char *crypt_str(int shift, const char *in, char *out)
{
    int len = strlen(in);
    for (int i = 0; i < len; i++) {
        out[i] = crypt_char(shift, in[i]);
    }
    out[len] = '\0';

    return out;
}

/**
 * @brief Display program usage
 * @param prog  Name of program (typically passed in as argv[0])
 */
static void usage(char *prog)
{
    printf("usage: %s [-h] (-d key | -e key) <msg>\n", prog);
    printf("\n");
    printf(
        "Encrypt or decrypt the supplied message with a given key. The\n"
        "key should be a positive integer. This integer is used to either\n"
        "right-shift (encrypt) or left-shift (decrypt) the ASCII characters\n"
        "in the message.\n\n"
        "Any non-ASCII characters in the message are left unchanged. The\n"
        "encrypted or decrypted message is written to standard output.\n");
    printf("\n");
    printf("-h   Display program usage\n");
    printf("-d   Decrypt message using the given key\n");
    printf("-e   Encrypt message using the given key\n");
    printf("msg  ASCII text to encrypt or decrypt\n");
    exit(1);
}

/**
 * @brief Parse a positive long integer from a string
 * @param arg   String to parse
 * @return The converted string as a long, or negative on failure
 */
static long parse_positive_long(const char *arg)
{
    char *endptr = NULL;
    errno = 0;
    long arg_long = strtol(arg, &endptr, 10);
    if (*endptr != '\0') {
        return -1;
    }

    if ((arg_long == LONG_MIN || arg_long == LONG_MAX) && errno == ERANGE) {
        return -2;
    }

    if (arg_long < 0) {
        return -3;
    }

    return arg_long;
}

int main(int argc, char *argv[])
{
    /** Parse arguments **/

    enum crypt_mode {
        cm_unset,
        cm_encrypt,
        cm_decrypt,
    };
    enum crypt_mode mode = cm_unset;

    long shift = 0;

    int c;
    while ((c = getopt(argc, argv, "d:e:h")) != -1) {
        switch (c) {
        case 'd':
            if (mode != cm_unset) {
                printf("%s: only -d or -e may be specified\n", argv[0]);
                usage(argv[0]); // exits
            }
            mode = cm_decrypt;
            shift = parse_positive_long(optarg);
            break;
        case 'e':
            if (mode != cm_unset) {
                printf("%s: only -d or -e may be specified\n", argv[0]);
                usage(argv[0]); // exits
            }
            mode = cm_encrypt;
            shift = parse_positive_long(optarg);
            break;
        case 'h':
        default:
            usage(argv[0]); // exits
        }
    }

    if (mode == cm_unset) {
        printf("%s: either -d or -e are required\n", argv[0]);
        usage(argv[0]); // exits
    }

    if (shift < 0) {
        printf("%s: key must be a positive base 10 integer\n", argv[0]);
        usage(argv[0]); // exits
    }

    if (argv[optind] == NULL) {
        printf("%s: missing required argument: msg\n", argv[0]);
        usage(argv[0]); // exits
    }

    /** Encrypt or decrypt message **/

    char *message = argv[optind];
    int message_len = strlen(message);
    char *crypted = calloc(1, message_len);
    if (crypted == NULL) {
        printf("%s: cannot allocate %d bytes for output message. aborting.\n",
               argv[0], message_len);
        return 2;
    }

    // Simply pass in a negative offset for decryption
    shift = (mode == cm_encrypt) ? shift : -shift;

    printf("%s\n", crypt_str(shift, message, crypted));
    free(crypted);

    return 0;
}