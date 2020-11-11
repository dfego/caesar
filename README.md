# Caesar cipher README

This is a simple application that takes in a message and applies a Caesar
cipher encryption or decryption on the ASCII bytes within and writes the
resulting message to standard output.

Note that the key only accepts positive integers, and will wrap automatically
if the key is greater than 25 (i.e. 26 is the same as 0).

## Building

To build, GCC and GNU Make are required. Simply run:

    make

To build. This application was written for and tested on Linux.

## Usage

The program's usage statement is as shown:

    usage: ./caesar [-h] (-d key | -e key) [msg]

    Encrypt or decrypt the supplied message with a given key. The
    key should be a positive integer. This integer is used to either
    right-shift (encrypt) or left-shift (decrypt) the ASCII characters
    in the message.

    Any non-ASCII characters in the message are left unchanged. The
    encrypted or decrypted message is written to standard output.

    -h   Display program usage
    -d   Decrypt message using the given key
    -e   Encrypt message using the given key
    msg  ASCII text to encrypt or decrypt. If omitted, read from stdin.

For example, to encrypt "This is a message!" by right-shifting by 6:

    ./caesar -e 6 "This is a message!"
    Znoy oy g skyygmk!

To decrypt:

    ./caesar -d 6 "Znoy oy g skyygmk!"
    This is a message!

Messages can also be read from stdin:

    ./caesar -e 23 "This is a message that I have typed into the terminal!" | ./caesar -d 23
    This is a message that I have typed into the terminal!
