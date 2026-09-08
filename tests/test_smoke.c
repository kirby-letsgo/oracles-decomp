#include "unit.h"
static void build_works(void) { ASSERT_EQ(1 + 1, 2); }
int main(void) { RUN(build_works); return 0; }
