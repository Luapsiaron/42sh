#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

#define ASSERT_TRUE(cond) do { \
  if (!(cond)) { \
    fprintf(stderr, "ASSERT failed: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
    return 1; \
  } \
} while (0)

#define ASSERT_INT_EQ(a,b) do { \
  int _a=(a), _b=(b); \
  if (_a != _b) { \
    fprintf(stderr, "ASSERT failed: %d != %d (%s:%d)\n", \
            _a, _b, __FILE__, __LINE__); \
    return 1; \
  } \
} while (0)

#endif
