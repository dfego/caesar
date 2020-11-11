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
#include <unistd.h>

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
 * @param out   Output stream to write to
 */
static void crypt_str(int shift, const char *in, FILE *out)
{
    int len = strlen(in);
    for (int i = 0; i < len; i++) {
        fputc(crypt_char(shift, in[i]), out);
    }
}

/**
 * @brief Crypt an ASCII string stream, not changing non-alphabetic characters
 * @param shift Number of characters to logically right-shift
 * @param in    Input stream to read from
 * @param out   Output stream to write to
 */
static void crypt_stream(int shift, FILE *in, FILE *out)
{
    int in_char;
    while ((in_char = fgetc(in)) != EOF) {
        fputc(crypt_char(shift, in_char), out);
    }
}

/**
 * @brief Display program usage
 * @param prog  Name of program (typically passed in as argv[0])
 */
static void usage(char *prog)
{
    fprintf(stderr, "usage: %s [-h] (-d key | -e key) [msg]\n", prog);
    fprintf(stderr, "\n");
    fprintf(
        stderr,
        "Encrypt or decrypt the supplied message with a given key. The\n"
        "key should be a positive integer. This integer is used to either\n"
        "right-shift (encrypt) or left-shift (decrypt) the ASCII characters\n"
        "in the message.\n\n"
        "Any non-ASCII characters in the message are left unchanged. The\n"
        "encrypted or decrypted message is written to standard output.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "-h   Display program usage\n");
    fprintf(stderr, "-d   Decrypt message using the given key\n");
    fprintf(stderr, "-e   Encrypt message using the given key\n");
    fprintf(stderr, "msg  ASCII text to encrypt or decrypt. If omitted, read "
                    "from stdin.\n");
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
                fprintf(stderr, "%s: only -d or -e may be specified\n",
                        argv[0]);
                usage(argv[0]); // exits
            }
            mode = cm_decrypt;
            shift = parse_positive_long(optarg);
            break;
        case 'e':
            if (mode != cm_unset) {
                fprintf(stderr, "%s: only -d or -e may be specified\n",
                        argv[0]);
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
        fprintf(stderr, "%s: either -d or -e are required\n", argv[0]);
        usage(argv[0]); // exits
    }

    if (shift < 0) {
        fprintf(stderr, "%s: key must be a positive base 10 integer\n",
                argv[0]);
        usage(argv[0]); // exits
    }

    /** Encrypt or decrypt message **/

    char *message = argv[optind];

    // Simply pass in a negative offset for decryption
    shift = (mode == cm_encrypt) ? shift : -shift;

    if (message == NULL) {
        crypt_stream(shift, stdin, stdout);
    } else {
        crypt_str(shift, message, stdout);
    }

    // Add a newline if stdout is a terminal, for readability
    if (isatty(STDOUT_FILENO)) {
        fputc('\n', stdout);
    }

    return 0;
}