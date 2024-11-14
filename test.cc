#include <stdio.h>
#include <chrono>

#include "tinyrulechecker.h"


#define ASSERT_EXPR(expr, expected) { \
  bool result; \
  if (!e.eval(expr, result)) { \
    printf ("Error evaluating %s\n", expr); \
    return false; \
  } \
  if (result != expected) { \
    printf ("Error evaluating %s, expected %d, got %d\n", expr, expected, result); \
    return false; \
  } \
}

#define ASSERT_ERROR_EXPR(expr) { \
  bool result; \
  if (e.eval(expr, result)) return false; \
}

bool test_all () {
  TinyRuleChecker e;
  e.setVarInt("a", 1);
  e.setVarFloat("b", 2.0);
  e.setVarString("c", "my string");

  ASSERT_EXPR("a.eq(1)", true);
  ASSERT_EXPR("a.eq(2)", false);
  ASSERT_EXPR("a.eq(1234551234)", false);

  e.setVarInt("a", 100);
  ASSERT_EXPR("a.eq(100)", true);
  ASSERT_EXPR("a.eq(101)", false);
  ASSERT_EXPR("a.eq(99)", false);
  ASSERT_EXPR("a.neq(100)", false);
  ASSERT_EXPR("a.neq(101)", true);
  ASSERT_EXPR("a.neq(99)", true);
  ASSERT_EXPR("a.lt(100)", false);
  ASSERT_EXPR("a.lt(101)", true);
  ASSERT_EXPR("a.lt(99)", false);
  ASSERT_EXPR("a.lte(100)", true);
  ASSERT_EXPR("a.lte(101)", true);
  ASSERT_EXPR("a.lte(99)", false);
  ASSERT_EXPR("a.gt(100)", false);
  ASSERT_EXPR("a.gt(101)", false);
  ASSERT_EXPR("a.gt(99)", true);
  ASSERT_EXPR("a.gte(100)", true);
  ASSERT_EXPR("a.gte(101)", false);
  ASSERT_EXPR("a.gte(99)", true);
  ASSERT_EXPR("!a.gte(99)", false);

  ASSERT_EXPR("(a.gte(100))", true);
  ASSERT_EXPR("(a.gte(100) && a.gt(99))", true);
  ASSERT_EXPR("a.gte(100) && a.gt(99)", true);

  ASSERT_EXPR("b.eq(2.0)", true);
  ASSERT_EXPR("b.eq(1.9999999)", false);
  ASSERT_EXPR("c.eq(\"my string\")", true);
  ASSERT_EXPR("c.contains(\"string\")", true);
  ASSERT_EXPR("c.contains(\"stringo\")", false);
  ASSERT_EXPR("c.contains('string')", true);
  ASSERT_EXPR("c.contains('stringo')", false);
  ASSERT_EXPR("c.in(\"string\")", false);
  ASSERT_EXPR("c.in(\"this is my string example\")", true);

//   ASSERT_ERROR_EXPR("a.eq(2.00)"); // type mismatch
  return true;
}

bool benchmark() {
  TinyRuleChecker e;
  e.setVarInt("myint", 1);
  e.setVarFloat("myfloat", 2.0);
  e.setVarString("mystr", "my string");

  // test performance
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  uint32_t iterations = 10*1000*1000;
  for (uint32_t i = 0; i < iterations; i++) {
    ASSERT_EXPR("myfloat.eq(1.9999999) || myint.eq(32)", false);
  }
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  printf("%.3f M ops/sec  (%d in %.3f seconds)\n", ((float)iterations / 1e6) / elapsed_seconds.count(), iterations, elapsed_seconds.count());

  // test performance of 1000 ops
  start = std::chrono::system_clock::now();
  iterations = 1000;
  for (uint32_t i = 0; i < iterations; i++) {
    ASSERT_EXPR("myfloat.eq(1.9999999) || myint.eq(32)", false);
  }
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  printf("%d ops in %.3f ms\n", iterations, 1000.0*elapsed_seconds.count());
  return true;
}


int main() {
  bool testPassed = test_all();
  if (testPassed) {
    printf ("Tests PASS!\n");
  }
  else {
    printf ("One or more tests FAILED!\n");
  }

  printf ("Running benchmark...\n");
  benchmark();
  return testPassed;
}