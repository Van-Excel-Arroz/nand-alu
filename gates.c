#include <stdio.h>

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

    int a[4] = {1, 0 , 1, 0}; // 5
    int b[4] = {1, 1, 0, 0}; // 3
    int result[4];
    int carry = 0;

    printf("Final Add Result: ");
    for (int i = 0; i < 4; i++) {
        add_result r = full_adder(a[i], b[i], carry);
        result[i] = r.sum;
        carry = r.carry;
    }
    
    for (int i = 3; i >= 0; i--) {
        printf("%d, ", result[i]);
    }

    printf("\nFinal carry: %d\n", carry);
 
    int errors = 0;
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            int a[4], b[4], result[4];
            int carry = 0;        
            for (int z = 0; z < 4; z++) {
                a[z] = (x >> z) & 1;
                b[z] = (y >> z) & 1;
            }

           for (int z = 0; z < 4; z++) {
                add_result r = full_adder(a[z], b[z], carry);
                result[z] = r.sum;
                carry = r.carry;
           }

            int expected = x + y;

            for (int z = 0; z < 4; z++) {
                int expected_bit = (expected >> z) & 1;
                if (result[z] != expected_bit) {
                    errors++;
                }
            }

            if (carry != ((expected >> 4) & 1)) {
                errors++;
            }
        }
    }   
    printf("errors: %d\n", errors);

       
    int c[4] = {1, 1 , 1, 0}; // 7
    int d[4] = {1, 1, 0, 0}; // 3
    int result1[4];
    int carry1 = 1;

    printf("\nFinal Subtract Result: ");
    for (int i = 0; i < 4; i++) {
        add_result r = full_adder(c[i], not_gate(d[i]), carry1);
        result1[i] = r.sum;
        carry1 = r.carry;
    }

    for (int i = 3; i >= 0; i--) {
        printf("%d, ", result1[i]); 
    }
    printf("\nFinal carry: %d\n", carry1);

    int errors1 = 0;
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            int a[4], b[4], result[4];
            int carry = 1;        
            for (int z = 0; z < 4; z++) {
                a[z] = (x >> z) & 1;
                b[z] = (y >> z) & 1;
            }

           for (int z = 0; z < 4; z++) {
                add_result r = full_adder(a[z], not_gate(b[z]), carry);
                result[z] = r.sum;
                carry = r.carry;
           }

            int expected = (x - y) & 15;

            for (int z = 0; z < 4; z++) {
                int expected_bit = (expected >> z) & 1;
                if (result[z] != expected_bit) {
                    errors1++;
                }
            }
        }
    }   
    printf("errors: %d\n", errors1);

    return 0;
}