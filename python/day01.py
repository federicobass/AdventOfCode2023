import re


digits_list = [ 0,1,2,3,4,5,6,7,8,9 ]
digits_string_list = [ "zero","one","two","three","four","five","six","seven","eight","nine" ]


def calibration_part_one(input):
    global digits_list

    first_occ_idx = 999
    first_occ_num = -1
    last_occ_idx = -1
    last_occ_num = -1

    for char, i in zip(input, range(len(input))):
        if not char.isdigit():
            continue
        
        if first_occ_num == -1:
            first_occ_idx = i
            first_occ_num = int(char)

        last_occ_idx = i
        last_occ_num = int(char)

    return (first_occ_num * 10 + last_occ_num)

def calibration_part_two(input):
    global digits_list, digits_string_list

    first_occ_idx = 999
    first_occ_num = -1
    last_occ_idx = -1
    last_occ_num = -1

    # DIGITS part
    for char, i in zip(input, range(len(input))):
        if not char.isdigit():
            continue
        
        if first_occ_num == -1:
            first_occ_idx = i
            first_occ_num = int(char)

        last_occ_idx = i
        last_occ_num = int(char)

    # STRING numeric part
    for num, i in zip(digits_string_list, range(len(digits_string_list))):
        num_occurrences = [elem.start() for elem in re.finditer(num, input)]
        if num_occurrences == []:
            continue

        if min(num_occurrences) < first_occ_idx:
            first_occ_idx = min(num_occurrences)
            first_occ_num = i
        if max(num_occurrences) > last_occ_idx:
            last_occ_idx = max(num_occurrences)
            last_occ_num = i

    return (first_occ_num * 10 + last_occ_num)

def main():
    result = 0

    with open("../input/day01.txt") as fp:
        while str_line := fp.readline():
            str_line = str_line.strip()
            result += calibration_part_one(str_line)
        print(f"Result part ONE: {result}")

        fp.seek(0)
        result = 0
        while str_line := fp.readline():
            str_line = str_line.strip()
            result += calibration_part_two(str_line)
        print(f"Result part TWO: {result}")

    exit(0)


if __name__ == "__main__":
    #print(calibration_part_one("nue2i8932dmio2ur839"))
    main()