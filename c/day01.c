#include "./macros/macros.h"

#define NUM(a, b)       (a - 48) * 10 + (b - 48)


char word_digits[10][6] = {
    "zero\0", "one\0", "two\0", "three\0", "four\0", "five\0", "six\0", "seven\0", "eight\0", "nine\0"
};


bool is_digit(char a) {
    return (a >= 48 && a <= 57);
}

int calibration_part_one(char *input, char *output) {
    int i, input_len;

    input_len = strlen(input);
    for (i = 0; i < input_len; i++) {
        if (!is_digit(input[i]))
            continue;
        if (output[0] == '\0')
            output[0] = output[1] = input[i];
        else
            output[1] = input[i];
    }

    return NUM(output[0], output[1]);
}

int calibration_part_two(char *input, char *output) {
    char *sub_str_occur;
    int i, input_len, j;
    int first_occ_idx, last_occ_idx, temp_occ_idx;
    int first_occ_num, last_occ_num, temp_occ_num;

    first_occ_idx = first_occ_num = 999;
    last_occ_idx = last_occ_num = -1;
    temp_occ_idx = temp_occ_num = 999;

    // DIGITS part
    input_len = strlen(input);
    for (i = 0; i < input_len; i++) {
        if (!is_digit(input[i]))
            continue;
        if (output[0] == '\0') {
            output[0] = output[1] = input[i];
            first_occ_num = last_occ_num = input[i] - 48;
            first_occ_idx = last_occ_idx = i;
        } else {
            output[1] = input[i];
            last_occ_num = input[i] - 48;
            last_occ_idx = i;
        }
    }

    // STRING numeric part
    for (i = 0; i < 10; i++)
        for (j = 0; j < input_len; j++) {
            sub_str_occur = strstr((input+j), word_digits[i]);
            if (sub_str_occur == NULL)
                continue;

            temp_occ_idx = sub_str_occur - input;
            temp_occ_num = i;

            if (temp_occ_idx < first_occ_idx) {
                first_occ_idx = temp_occ_idx;
                first_occ_num = temp_occ_num;
            } else if (temp_occ_idx > last_occ_idx) {
                last_occ_idx = temp_occ_idx;
                last_occ_num = temp_occ_num;
            }
        }

    output[0] = first_occ_num + 48;
    output[1] = last_occ_num + 48;

    return NUM(output[0], output[1]);
}

int main() {
    char *filename = "../input/day01.txt";
    char *input, *output;
    FILE *fp;
    long i, result;
    size_t line_len;
    
    output = (char *) calloc(2, sizeof(char));

    result = 0;
    fp = open_file(filename, "r");
    fseek(fp, 0, SEEK_SET);
    while (getline(&input, &line_len, fp) != -1) {
        memset(output, '\0', 2);
        result += calibration_part_one(input, output);
    }
    printf("Result part ONE : %ld\n", result);

    result = 0;
    fseek(fp, 0, SEEK_SET);
    while (getline(&input, &line_len, fp) != -1) {
        memset(output, '\0', 2);
        result += calibration_part_two(input, output);
        //printf("%s: %s\n", input, output);
        //printf("%ld. %s\n", ++i, output);
    }
    printf("Result part TWO : %ld\n", result);

    free(input);
    free(output);
    close_file(fp);
    return 0;
}