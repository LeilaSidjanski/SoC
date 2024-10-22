#include "cache.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

#define FLPT_SIGN_BIT 31    // Sign bit position
#define FLPT_MANTISSA_BITS 23
#define FLPT_EXPONENT_BIAS 127

#define FLPT_MAX_EXPONENT 255
#define FLPT_MIN_EXPONENT 0
#define MY_INFINIY 0x7F800000
#define MY_MINUS_INFINITY 0xFF800000

typedef int32_t flpt;
void print_binary(flpt value);
flpt extract_sign_flpt(flpt value);
flpt extract_exponent_flpt(flpt value);
flpt extract_mantissa_flpt(flpt value);
flpt format_result_flpt(flpt sign, flpt exponent, flpt mantissa);
flpt convert_float_to_flpt(float input);
flpt multiplication_flpt(float x, float y);
flpt addition_flpt(float x1, float y1);

int main() {

    /*float x = 1.5;
    float y = 2.5;
    print_binary(addition_flpt(x, y));
    print_binary(multiplication_flpt(x, y));*/

    // Testing multiplication overflow
    float a = 1e38;
    float b = 4.0;
    print_binary(multiplication_flpt(a, b));
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
    if((extract_exponent_flpt(value) + FLPT_EXPONENT_BIAS) != 0) {
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

    printf("x: \n");
    print_binary(x);
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

    printf("y:\n");
    print_binary(y);
    printf("Sign_y:\n");
    print_binary(sign_y);
    printf("Exponent_y:\n");
    print_binary(exponent_y + FLPT_EXPONENT_BIAS);
    printf("Mantissa_y:\n");
    print_binary(mantissa_y);

    flpt sign = sign_x ^ sign_y;  // Determining sign according to regular rules for multiplication
    
    // ARE LINES 112-120 FULLY NECESSARY? SHOULD THEY BE ALL THE WAY AT BEGINNING OF FUNCTION?
    // THE REST OF THE CHECKS SUFFICE BUT IS IT GOOD PRACTICE?
    //Pre-multiplication checks
/*     if(isinf(x1) || isinf(y1)) {
        return (sign == 0) ? MY_INFINIY : MY_MINUS_INFINITY; // Return max/min int for overflow
    }
    if((fabs(x1) * fabs(y1)) > FLT_MAX) {
        return (sign == 0) ? MY_INFINIY : MY_MINUS_INFINITY;
    } */
    
    int64_t mantissa = (int64_t) mantissa_x * (int64_t) mantissa_y; // Multiplying the mantissas
    flpt exponent = exponent_x + exponent_y + FLPT_EXPONENT_BIAS;

    if(mantissa & (1LL << ((2 * FLPT_MANTISSA_BITS) + 1))) {
        mantissa >>= 24;
        exponent++;
    } else {
        mantissa >>= 23;
    }

    // Check for overflow or underflow in the exponent
    if (exponent >= FLPT_MAX_EXPONENT) {  // Exponent overflow, set to
        return (sign == 0) ? MY_INFINIY : MY_MINUS_INFINITY; // Return max/min int for overflow
    } else if (exponent <= FLPT_MIN_EXPONENT) {  // Exponent underflow, set to zero
        return format_result_flpt(0, 0, 0);  // Return zero for underflow
    }

    printf("Sign:\n");
    print_binary(sign);
    printf("Exponent:\n");
    print_binary(exponent);
    printf("Mantissa:\n");
    print_binary(mantissa);
    
    return format_result_flpt(sign, exponent, mantissa);
}

flpt addition_flpt(float x1, float y1) {
    // AM I ALLOWED TO DO THESE PRE-CHECKS IN FLOAT?
    // SAME ISSUE AS MULTIPLICATION: IS IT NECESSARY/GOOD PRACTICE?
/*     if(x1 > 0 && y1 > 0 && (x1 + y1) > FLT_MAX) {
        return MY_INFINIY;
    }
    if(x1 < 0 && y1 < 0 && (x1 + y1) < -FLT_MAX) {
        return MY_MINUS_INFINITY;
    } */
    flpt x = convert_float_to_flpt(x1);
    flpt sign_x = extract_sign_flpt(x);
    flpt exponent_x = extract_exponent_flpt(x);
    flpt mantissa_x = extract_mantissa_flpt(x);

    printf("x:\n");
    print_binary(x);
    printf("Sign_x:\n");
    print_binary(sign_x);
    printf("Exponent_x:\n");
    print_binary(exponent_x + FLPT_EXPONENT_BIAS);
    printf("Mantissa_x:\n");
    print_binary(mantissa_x);

    flpt y = convert_float_to_flpt(y1);
    flpt sign_y = extract_sign_flpt(y);
    flpt exponent_y = extract_exponent_flpt(y);
    flpt mantissa_y = extract_mantissa_flpt(y);

    printf("y:\n");
    print_binary(y);
    printf("Sign_y:\n");
    print_binary(sign_y);
    printf("Exponent_y:\n");
    print_binary(exponent_y + FLPT_EXPONENT_BIAS);
    printf("Mantissa_y:\n");
    print_binary(mantissa_y);

    flpt exponent = FLPT_EXPONENT_BIAS; 

    // Aligning exponents by shifting the smaller mantissa
    if(exponent_x > exponent_y) {
        mantissa_y >>= (exponent_x - exponent_y);
        exponent += exponent_x;
    } else if(exponent_y > exponent_x) {
        mantissa_x >>= (exponent_y - exponent_x);
        exponent += exponent_y;
    } else {
        exponent += exponent_x;
    }

    flpt sign, mantissa;
    if(sign_x == sign_y) {
        mantissa = mantissa_x + mantissa_y;
        sign = sign_x;
    } else if(mantissa_x > mantissa_y) {
        mantissa = mantissa_x - mantissa_y;
        sign = sign_x; // Result will take the sign of the larger mantissa
    } else if(mantissa_y > mantissa_x) {
        mantissa = mantissa_y - mantissa_x;
        sign = sign_y;
    } else {
        return 0;
    }

    // Normalizing the result if mantissa exceeds maximum range
     while(mantissa > 0xFFFFFF) {
        mantissa >>= 1;
        exponent++;
    }

    // Overflow & underflow check
    if(exponent >= FLPT_MAX_EXPONENT) {
        return (sign == 0) ? MY_INFINIY : MY_MINUS_INFINITY;
    } else if (exponent <= FLPT_MIN_EXPONENT) { 
        return format_result_flpt(0, 0, 0);
    }
    
    
    printf("Sign:\n");
    print_binary(sign);
    printf("Exponent:\n");
    print_binary(exponent);
    printf("Mantissa:\n");
    print_binary(mantissa);

    return format_result_flpt(sign, exponent, mantissa);
}