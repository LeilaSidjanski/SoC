#include "cache.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define FLPT_SIGN_BIT 31    // Sign bit position
#define FLPT_MANTISSA_BITS 23
#define FLPT_EXPONENT_BIAS 127

typedef int32_t flpt;
void print_binary(flpt value);
flpt extract_sign_flpt(flpt value);
flpt extract_exponent_flpt(flpt value);
flpt extract_mantissa_flpt(flpt value);
flpt format_result_flpt(flpt sign, flpt exponent, flpt mantissa);
flpt convert_float_to_flpt(float input);
flpt multiplication_flpt(float x, float y);

int main() {

    float x = 1.5;
    float y = 2.5;

    print_binary(multiplication_flpt(x, y));
    printf("Expected: 01000000 01110000 00000000 00000000")
}   

void print_binary(flpt value) {
    // Iterate through each bit from the most significant bit to the least
    for (int i = 31; i >= 0; i--) {
        // Print the bit at position i
        printf("%d", (value >> i) & 1);

        // Add a space after every 8 bits for readability
        if (i % 8 == 0 && i != 0) {
            printf(" ");
        }
    }
    printf("\n");
}

// Reinterpret the float bits as a 32-bit integer (flpt)
flpt convert_float_to_flpt(float value) {
    return *((flpt*)&value);
}

// Extracting sign
flpt extract_sign_flpt(flpt value) {
    return (value >> FLPT_SIGN_BIT) & 0x1;
}

// Extracting exponent
flpt extract_exponent_flpt(flpt value) {
    return ((value >> FLPT_MANTISSA_BITS) & 0xFF) - FLPT_EXPONENT_BIAS; 
}

// Extracting mantissa
flpt extract_mantissa_flpt(flpt value) {
    flpt mantissa = value & 0x7FFFFF; // Extracting mantissa (wihtout implicit leading 1)
    // If the exponent is not zero (normalized number), we add the implicit leading 1
    if(((value >> FLPT_MANTISSA_BITS) & 0xFF) != 0) {
      mantissa |= 0x800000; // Adding implicit leading 1
    }
    return mantissa;
}

flpt format_result_flpt(flpt sign, flpt exponent, flpt mantissa) {
    return (sign << FLPT_SIGN_BIT) | ((exponent & 0xFF) << FLPT_MANTISSA_BITS) | (mantissa & 0x7FFFFF);
}

flpt multiplication_flpt(float x1, float y1) {
    flpt x = convert_float_to_flpt(x1);
    flpt sign_x = extract_sign_flpt(x);
    // Extracted exponent is biased
    flpt exponent_x = extract_exponent_flpt(x);
    flpt mantissa_x = extract_mantissa_flpt(x);

    printf("Sign_x:\n");
    print_binary(sign_x);
    printf("Exponent_x:\n");
    print_binary(exponent_x + FLPT_EXPONENT_BIAS);
    printf("Mantissa_x:\n");
    print_binary(mantissa_x);

    flpt y = convert_float_to_flpt(y1);
    flpt sign_y = extract_sign_flpt(y);
    // Extracted exponent is biased
    flpt exponent_y = extract_exponent_flpt(y);
    flpt mantissa_y = extract_mantissa_flpt(y);

    printf("Sign_y:\n");
    print_binary(sign_y);
    printf("Exponent_y:\n");
    print_binary(exponent_y + FLPT_EXPONENT_BIAS);
    printf("Mantissa_y:\n");
    print_binary(mantissa_y);

    flpt sign = sign_x ^ sign_y;  // Determining sign according to regular rules for multiplication
    int64_t mantissa = (int64_t) mantissa_x * (int64_t) mantissa_y; // Multiplying the mantissas
    flpt exponent = exponent_x + exponent_y + FLPT_EXPONENT_BIAS;

    // Shift the result right to normalize
    if (mantissa & (1LL << (FLPT_MANTISSA_BITS * 2))) { // Checking 47th bit for overflow
        mantissa >>= 24;
        exponent++;  // Adjust the exponent due to normalization
    } else {
        mantissa >>= 23;
    }

    // Check for overflow or underflow in the exponent
    if (exponent >= 255) {  // Exponent overflow, set to infinity
        exponent = 255;  // Set to infinity (max exponent)
        mantissa = 0;  // No mantissa
    } else if (exponent <= 0) {  // Exponent underflow, set to zero
        return 0;  // Return zero for underflow
    } else {
        mantissa &= 0x7FFFFF; // Keep only 23 bits of mantissa
    }

    printf("Sign:\n");
    print_binary(sign);
    printf("Exponent:\n");
    print_binary(exponent);
    printf("Mantissa:\n");
    print_binary(mantissa);

    return format_result_flpt(sign, exponent, mantissa);

}
