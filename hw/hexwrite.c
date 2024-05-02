#include <stdio.h>

void write_hex_to_binary_file(const char* hex_array, size_t array_size, FILE* output_file) {
    for (size_t i = 0; i < array_size; i += 2) {
        unsigned int value;
        sscanf(&hex_array[i], "%2x", &value);
        fwrite(&value, sizeof(unsigned char), 1, output_file);
    }
}

int main() {
    const char hex_array[] = "48656C6C6F20576F726C64"; // "Hello World" in hexadecimal
    const size_t array_size = sizeof(hex_array) / sizeof(hex_array[0]) - 1; // Exclude null terminator
    const char* output_filename = "output.bin";

    FILE* output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        perror("Error opening file");
        return 1;
    }

    write_hex_to_binary_file(hex_array, array_size, output_file);

    fclose(output_file);

    return 0;
}

