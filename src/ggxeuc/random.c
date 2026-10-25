#include "generated/random.h"

void RandomInit(int num) {
    RandomSeed = num;
}

int Random(void) {
    RandomSeed = RandomSeed * 0x1579 + 0x7FA9;
    return RandomSeed & 0xFFFF;
}
