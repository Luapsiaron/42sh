#include "test.h"

int main(void)
{
  ASSERT_TRUE(1 < 2);
  ASSERT_INT_EQ(10, 2);
  return 0;
}
