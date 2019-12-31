/*Реализуйте функцию с прототипом:

typedef enum {
    PlusZero      = 0x00,
    MinusZero     = 0x01,
    PlusInf       = 0xF0,
    MinusInf      = 0xF1,
    PlusRegular   = 0x10,
    MinusRegular  = 0x11,
    PlusDenormal  = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN  = 0x30,
    QuietNaN      = 0x31
} float_class_t;

extern float_class_t
classify(double *value_ptr);
      

которая классифицирует специальные значения вещественного числа,
на которое указывает value_ptr, в соответствии со стандартом IEEE 754.

При решении допускается использовать только побитовые операции, включая сдвиги.*/

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>



typedef enum float_class_t {
    PlusZero      = 0x00,
    MinusZero     = 0x01,
    PlusInf       = 0xF0,
    MinusInf      = 0xF1,
    PlusRegular   = 0x10,
    MinusRegular  = 0x11,
    PlusDenormal  = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN  = 0x30,
    QuietNaN      = 0x31
} float_class_t;

const int32_t
    mantissa_pos = 51,
    exponent_pos = 62,
    sign_pos = 63;

bool get_nth_bit(uint64_t pattern, uint32_t pos) {
    return 1 & (pattern >> pos);
}

bool is_mantissa_filled_with_one_digit(uint64_t pattern, bool digit) {
    bool result = false;

    for (int i = 0; i <= mantissa_pos; ++i) {
        if (get_nth_bit(pattern, i) == (1 & digit))
            result = true;
    }

    return result;
}

bool is_exponent_filled_with_one_digit(uint64_t pattern, bool digit) {
    bool result = false;

    for (int i = mantissa_pos + 1; i <= exponent_pos; ++i) {
        if (get_nth_bit(pattern, i) == (1 & digit))
            result = true;
    }

    return result;
}

extern float_class_t classify(double *value_ptr) {
    enum float_class_t classification;
    uint64_t pattern = *(uint64_t*)(value_ptr);

    // Check mantissa.
    bool null_bit_frac = is_mantissa_filled_with_one_digit(pattern, false);
    bool not_null_bit_frac = is_mantissa_filled_with_one_digit(pattern, true);


    // Check exponent.
    bool null_bit_exp = is_exponent_filled_with_one_digit(pattern, false);
    bool not_null_bit_exp = is_exponent_filled_with_one_digit(pattern, true);

    // Check sign.
    bool negative = 0;
    if (get_nth_bit(pattern, sign_pos)) {
        negative = 1;
    }

    // Classification.
    if (!negative && !not_null_bit_exp && !not_null_bit_frac)
        classification = PlusZero;
    if (!negative && !not_null_bit_exp && not_null_bit_frac && null_bit_frac)
        classification = PlusDenormal;
    if (!negative && not_null_bit_exp && null_bit_exp)
        classification = PlusRegular;
    if (!negative && !null_bit_exp && !not_null_bit_frac)
        classification = PlusInf;

    if (negative && !not_null_bit_exp && !not_null_bit_frac)
        classification = MinusZero;
    if (negative && !not_null_bit_exp && not_null_bit_frac && null_bit_frac)
        classification = MinusDenormal;
    if (negative && not_null_bit_exp && null_bit_exp)
        classification = MinusRegular;
    if (negative && !null_bit_exp && !not_null_bit_frac)
        classification = MinusInf;

    if (!null_bit_exp && null_bit_frac && not_null_bit_frac && get_nth_bit(pattern, mantissa_pos))
        classification = QuietNaN;
    if (!null_bit_exp &&  null_bit_frac && not_null_bit_frac && !get_nth_bit(pattern, mantissa_pos))
        classification = SignalingNaN;

    return classification;
}

int main(int argc, char **argv) {
    uint64_t y = 0x7FF0000000000000ULL;
    double x = *(double*)(&y);
    printf("%lud\n", y);

    // scanf("%lf", &x);

    enum float_class_t classification;
    classification = classify(&x);

    if (classification == PlusZero) // +
        printf("%d", 1);
    if (classification == PlusDenormal) // +
        printf("%d", 2);
    if (classification == PlusRegular) // +
        printf("%d", 3);
    if (classification == PlusInf) // +
        printf("%d", 4);
    if (classification == MinusZero) // +
        printf("%d", 5);
    if (classification == MinusDenormal) // +
        printf("%d", 6);
    if (classification == MinusRegular) // +
        printf("%d", 7);
    if (classification == MinusInf) // +
        printf("%d", 8);
    if (classification == SignalingNaN) // +
        printf("%d", 9);
    if (classification == QuietNaN) // +
        printf("%d", 10);

    return 0;
}
