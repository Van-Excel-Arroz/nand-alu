#include <stdio.h>
#include <assert.h>
#define BIT_WIDTH 4
#define NUM_VALUES (1 << BIT_WIDTH)
#define MAX_VALUE (NUM_VALUES - 1)

typedef struct add_result {
    int sum;
    int carry;
} add_result;

typedef int (*gate_fn)(int, int);

typedef struct {
    int zero;
    int negative;
    int carry;
} alu_flags;

static int nand_count = 0;

enum { 
    ADD = 0, 
    SUB = 1, 
    AND = 2, 
    OR = 3
};

int nand(int a, int b) {
    nand_count++;
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
    int n1 = nand(a, b);
    int n2 = nand(a, n1);
    int n3 = nand(b, n1);
    return nand(n2, n3);
}

void print_truth_table(const char *name, gate_fn gate) {
    printf("\na b | %s\n", name);
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            printf("%d %d | %d\n", i, j, gate(i, j));
        }
    }
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

int add_sub(
    const int x_bits[BIT_WIDTH], 
    const int y_bits[BIT_WIDTH],
    int op,
    int result[BIT_WIDTH]) 
{
    int subtract = (op != 0);
    int carry = subtract;

    for (int i = 0; i < BIT_WIDTH; i++) {
        // XOR helps to flip the y bits for subtracting
        add_result r = full_adder(x_bits[i], xor_gate(y_bits[i], subtract), carry);
        result[i] = r.sum;
        carry = r.carry;
    }
    return carry;
}

alu_flags alu(
    const int x_bits[BIT_WIDTH], 
    const int y_bits[BIT_WIDTH],
    int op,
    int result[BIT_WIDTH]) 
{
    alu_flags f;
    f.carry = 0;
    
    switch (op) {
        case ADD:
        case SUB:
            f.carry = add_sub(x_bits, y_bits, op, result);
            break;
        case AND:
            for (int i = 0; i < BIT_WIDTH; i++) {
                result[i] = and_gate(x_bits[i], y_bits[i]);
            }
            break;
        case OR:
            for (int i = 0; i < BIT_WIDTH; i++) {
                result[i] = or_gate(x_bits[i], y_bits[i]);
            }
            break;
        default:
            assert(0 && "Unknown ALU op.\n");
    }

    // zero flag
    int any_one = 0;
    for (int i = 0; i < BIT_WIDTH; i++) {
        any_one = or_gate(result[i], any_one);
    }
    f.zero = not_gate(any_one);
    
    // negative flag
    f.negative = result[BIT_WIDTH - 1];

    return f;
}

void print_bits(const int bits[BIT_WIDTH]) {
    for (int i = BIT_WIDTH - 1; i >= 0; i--) {
        printf("%d", bits[i]);
    }
}

void count_gate(const char* name, gate_fn gate) {
    nand_count = 0;
    gate(0, 0);
    printf("%s gate uses %d NANDs\n", name, nand_count);
}

int main(void) {
    printf("Truth Tables\n");
    print_truth_table("NAND", nand);
    print_truth_table("AND", and_gate);
    print_truth_table("OR", or_gate);
    print_truth_table("XOR", xor_gate);

    printf("\na | NOT\n");
    for (int i = 0; i <= 1; i++) {
        printf("%d | %d\n", i, not_gate(i));
    }

    printf("\na b | carry sum (half adder)\n");
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            add_result r = half_adder(i, j);
            printf("%d %d | %d %d\n", i, j, r.carry, r.sum);
        }
    }

    printf("\na b cin | carry sum (full adder)\n");
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            for (int k = 0; k <= 1; k++) {
                add_result r = full_adder(i, j, k);
                printf("%d %d %d   | %d %d  \n", i, j, k, r.carry, r.sum);
            }
        }
    }

    int a = 3;
    int b = 7;

    nand_count = 0;
    not_gate(0);
    printf("\nNOT gate uses %d NANDs\n", nand_count);
    count_gate("AND", and_gate);
    count_gate("OR", or_gate);
    count_gate("XOR", xor_gate);

    int x_bits[BIT_WIDTH];
    int y_bits[BIT_WIDTH];
    to_bits(a, x_bits);
    to_bits(b, y_bits);
    int result[BIT_WIDTH];

    nand_count = 0;
    half_adder(0, 0);
    printf("half_adder gates uses %d NANDs\n", nand_count);
    nand_count = 0;
    full_adder(0, 0, 0);
    printf("full_adder gates uses %d NANDs\n", nand_count);
    nand_count = 0;
    int final_carry = add_sub(x_bits, y_bits, ADD, result);
    printf("add_sub gates uses %d NANDs\n", nand_count);

    printf("\nFinal Add Result (Binary): ");
    print_bits(result);
    printf("\nFinal Add Result (Decimal): %d", from_bits(result));
    printf("\nFinal carry: %d\n", final_carry);

    to_bits(a, x_bits);
    to_bits(b, y_bits);
    final_carry = add_sub(x_bits, y_bits, SUB, result);
    
    printf("\nFinal Subtract Result (Binary): ");
    print_bits(result);
    printf("\nFinal Subtract Result (Decimal): %d", from_bits(result));
    printf("\nFinal carry: %d\n", final_carry);

    int add_errors = 0;
    for (int x = 0; x < NUM_VALUES; x++) {
        for (int y = 0; y < NUM_VALUES; y++) {
            to_bits(x, x_bits);
            to_bits(y, y_bits);
            final_carry = add_sub(x_bits, y_bits, ADD, result);
            int expected = x + y;

            for (int z = 0; z < BIT_WIDTH; z++) {
                int expected_bit = (expected >> z) & 1;
                if (result[z] != expected_bit) {
                    add_errors++;
                }
            }

            if (final_carry != ((expected >> BIT_WIDTH) & 1)) {
                add_errors++;
            }
        }
    }   
    printf("\naddition errors: %d\n", add_errors);

    int sub_errors = 0;
    for (int x = 0; x < NUM_VALUES; x++) {
        for (int y = 0; y < NUM_VALUES; y++) {
            to_bits(x, x_bits);
            to_bits(y, y_bits);
            add_sub(x_bits, y_bits, SUB, result);      
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

    int alu_errors = 0;
    for (int op = ADD; op <= OR; op++) {
        for (int x = 0; x < NUM_VALUES; x++) {
            for (int y = 0; y < NUM_VALUES; y++) {
                to_bits(x, x_bits);
                to_bits(y, y_bits);
                alu_flags f = alu(x_bits, y_bits, op, result);
                int expected = 0;

                switch (op) {
                    case ADD:  expected = (x + y) & MAX_VALUE; break;
                    case SUB:  expected = (x - y) & MAX_VALUE; break;
                    case AND:  expected = (x & y) & MAX_VALUE; break;
                    case OR:   expected = (x | y) & MAX_VALUE; break;
                }
                
                if (from_bits(result) != expected) {
                    alu_errors++;
                }

                if (f.zero != (expected == 0)) {
                    alu_errors++;
                }
            }
        }
    }
    printf("ALU errors: %d\n", alu_errors);

    return 0;
}