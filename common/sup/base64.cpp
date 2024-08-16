#include "base64.h"

int Base64::base64_char_index(char c)
{
    if ('A' <= c && c <= 'Z')
        return c - 'A';
    if ('a' <= c && c <= 'z')
        return c - 'a' + 26;
    if ('0' <= c && c <= '9')
        return c - '0' + 52;
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;
    return -1;
}

size_t Base64::encodedSize(size_t input_len)
{
    return 4 * ((input_len + 2) / 3) + 1; // +1 for the null terminator
}

size_t Base64::decodedSize(const char *input)
{
    size_t input_len = strlen(input);
    size_t padding = 0;

    if (input_len >= 2 && input[input_len - 1] == '=')
        padding++;
    if (input_len >= 2 && input[input_len - 2] == '=')
        padding++;

    return (input_len / 4) * 3 - padding;
}

void Base64::encode(const unsigned char *input, size_t len, char *output)
{
    size_t i, j;
    size_t enc_len = 4 * ((len + 2) / 3);

    for (i = 0, j = 0; i < len;)
    {
        uint32_t octet_a = i < len ? input[i++] : 0;
        uint32_t octet_b = i < len ? input[i++] : 0;
        uint32_t octet_c = i < len ? input[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output[j++] = base64_table[(triple >> 3 * 6) & 0x3F];
        output[j++] = base64_table[(triple >> 2 * 6) & 0x3F];
        output[j++] = base64_table[(triple >> 1 * 6) & 0x3F];
        output[j++] = base64_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < (3 - len % 3) % 3; i++)
        output[enc_len - 1 - i] = '=';

    output[enc_len] = '\0'; // Null-terminate the string
}

bool Base64::decode(const char *input, unsigned char *output, size_t *out_len)
{
    size_t input_len = strlen(input);

    if (input_len % 4 != 0)
        return false;

    *out_len = input_len / 4 * 3;
    if (input[input_len - 1] == '=')
        (*out_len)--;
    if (input[input_len - 2] == '=')
        (*out_len)--;

    size_t i, j;
    uint32_t triple;
    for (i = 0, j = 0; i < input_len;)
    {
        int sextet_a = input[i] == '=' ? 0 & i++ : base64_char_index(input[i++]);
        int sextet_b = input[i] == '=' ? 0 & i++ : base64_char_index(input[i++]);
        int sextet_c = input[i] == '=' ? 0 & i++ : base64_char_index(input[i++]);
        int sextet_d = input[i] == '=' ? 0 & i++ : base64_char_index(input[i++]);

        if (sextet_a == -1 || sextet_b == -1 || sextet_c == -1 || sextet_d == -1)
            return false;

        triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < *out_len)
            output[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *out_len)
            output[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *out_len)
            output[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return true;
}

const char Base64::base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
