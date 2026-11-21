#include <stdio.h>
#define BIT_WIDTH 4
#define NUM_VALUES (1 << BIT_WIDTH)
#define MAX_VALUE (NUM_VALUES - 1)

typedef struct add_result {
    int sum;
    int carry;
} add_result;

int nand(int a, int b) {
    return !(a && b);
}

int not_gate(int a) {
    return nand(a, a);
}

int and_gate(int a, int b) {
    return not_gate(nand(a, b));
}

int or_gate(int a, int b) {
    return nand(not_gate(a), not_gate(b));
}

int xor_gate(int a, int b) {
    int x = and_gate(not_gate(a), b);
    int y = and_gate(a, not_gate(b));
    return or_gate(x, y);
}

add_result half_adder(int a, int b) {
    add_result r;
    r.sum = xor_gate(a, b);
    r.carry = and_gate(a, b);
    return r;
}

add_result full_adder(int a, int b, int cin) {
    add_result h1 = half_adder(a, b); 
    add_result h2 = half_adder(h1.sum, cin); 
    add_result r;
    r.carry = or_gate(h1.carry, h2.carry); 
    r.sum = h2.sum;
    return r;
}

void to_bits(int value, int bits[BIT_WIDTH]) {
    for (int i = 0; i < BIT_WIDTH; i++) {
        bits[i] = (value >> i) & 1;
    }
}

int from_bits(const int bits[BIT_WIDTH]) {
    int value = 0;
    int current_bit_value = 1;

    for (int i = 0; i < BIT_WIDTH; i++) {
        if (bits[i] == 1) {
            value += current_bit_value;
        }
        current_bit_value *= 2;
    }
    return value;
}

int main(void) {
    printf("a b | nand\n");
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            printf("%d %d | %d\n", i, j, nand(i, j));
        }
    }
    
    printf("a | NOT using nand\n");
    for (int i = 0; i <= 1; i++) {
        printf("%d | %d\n", i, not_gate(i));
    }

    printf("a b | AND using nand\n");
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            printf("%d %d | %d\n", i, j, and_gate(i, j));
        }
    }
    
    printf("a b | OR using nand\n");
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            printf("%d %d | %d\n", i, j, or_gate(i, j));
        }
    }

    printf("a b | XOR using nand\n");
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            printf("%d %d | %d\n", i, j, xor_gate(i, j));
        }
    }

    printf("a b | carry sum\n");
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            add_result r = half_adder(i, j);
            printf("%d %d | %d %d\n", i, j, r.carry, r.sum);
        }
    }

    printf("a b cin | carry sum\n");
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            for (int k = 0; k <= 1; k++) {
                add_result r = full_adder(i, j, k);
                printf("%d %d %d   | %d %d  \n", i, j, k, r.carry, r.sum);
            }
        }
    }

    int a[BIT_WIDTH];
    int b[BIT_WIDTH];
    to_bits(5, a);
    to_bits(3, b);
    int result[BIT_WIDTH];
    int carry = 0;

    for (int i = 0; i < BIT_WIDTH; i++) {
        add_result r = full_adder(a[i], b[i], carry);
        result[i] = r.sum;
        carry = r.carry;
    }
    
    printf("\nFinal Add Result (Binary): ");
    for (int i = BIT_WIDTH - 1; i >= 0; i--) {
        printf("%d, ", result[i]);
    }
    printf("\nFinal Add Result (Decimal): %d", from_bits(result));
    printf("\nFinal carry: %d\n", carry);
       
    int c[BIT_WIDTH];
    int d[BIT_WIDTH];
    to_bits(7, c);
    to_bits(3, d);
    int result1[BIT_WIDTH];
    int carry1 = 1;

    for (int i = 0; i < BIT_WIDTH; i++) {
        add_result r = full_adder(c[i], not_gate(d[i]), carry1);
        result1[i] = r.sum;
        carry1 = r.carry;
    }
    
    printf("\nFinal Subtract Result (Binary): ");
    for (int i = BIT_WIDTH - 1; i >= 0; i--) {
        printf("%d, ", result1[i]); 
    }
    printf("\nFinal Subtract Result (Decimal): %d", from_bits(result1));
    printf("\nFinal carry: %d\n", carry1);

    int add_errors = 0;
    for (int x = 0; x < NUM_VALUES; x++) {
        for (int y = 0; y < NUM_VALUES; y++) {
            int a[BIT_WIDTH], b[BIT_WIDTH], result[BIT_WIDTH];
            int carry = 0;        
            to_bits(x, a);
            to_bits(y, b);

           for (int z = 0; z < BIT_WIDTH; z++) {
                add_result r = full_adder(a[z], b[z], carry);
                result[z] = r.sum;
                carry = r.carry;
           }

            int expected = x + y;

            for (int z = 0; z < BIT_WIDTH; z++) {
                int expected_bit = (expected >> z) & 1;
                if (result[z] != expected_bit) {
                    add_errors++;
                }
            }

            if (carry != ((expected >> BIT_WIDTH) & 1)) {
                add_errors++;
            }
        }
    }   
    printf("\naddition errors: %d\n", add_errors);

    int sub_errors = 0;
    for (int x = 0; x < NUM_VALUES; x++) {
        for (int y = 0; y < NUM_VALUES; y++) {
            int a[BIT_WIDTH], b[BIT_WIDTH], result[BIT_WIDTH];
            int carry = 1;        
            to_bits(x, a);
            to_bits(y, b);

           for (int z = 0; z < BIT_WIDTH; z++) {
                add_result r = full_adder(a[z], not_gate(b[z]), carry);
                result[z] = r.sum;
                carry = r.carry;
           }

            int expected = (x - y) & MAX_VALUE;

            for (int z = 0; z < BIT_WIDTH; z++) {
                int expected_bit = (expected >> z) & 1;
                if (result[z] != expected_bit) {
                    sub_errors++;
                }
            }
        }
    }   
    printf("subtraction errors: %d\n", sub_errors);

    return 0;
}