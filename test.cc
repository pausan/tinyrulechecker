#include <stdio.h>
#include <chrono>

#include "tinyrulechecker.h"

#define ASSERT_EXPR(expr, expected) { \
  TinyRuleChecker::EvalResult eres = e.eval(expr); \
  if (!eres.error.empty()) { \
    printf ("Error evaluating %s\n", expr); \
    printf ("Error: %s\n", eres.error.c_str()); \
    printf (">> %s:%d\n", __FILE__, __LINE__); \
    return false; \
  } \
  if (eres.result != expected) { \
    printf ("Error evaluating %s, expected value %d, got %d\n", expr, expected, eres.result); \
    printf (">> %s:%d\n", __FILE__, __LINE__); \
    return false; \
  } \
}

#define ASSERT_ERROR_EXPR(expr, expected_error) { \
  TinyRuleChecker::EvalResult eres = e.eval(expr); \
  if (eres.error.empty()) return false; \
  if (eres.error != expected_error) { \
    printf ("Error evaluating: %s\n - Expected Error: %s\n - Got Error     : %s\n", expr, expected_error, eres.error.c_str()); \
    printf (">> %s:%d\n", __FILE__, __LINE__); \
    return false; \
  } \
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

  // test spaces
  ASSERT_EXPR("a . eq (   100  )", true);
  ASSERT_EXPR("\na\t.\n\r  eq\t\t  \t\n\t(\t\n\n100\t\t  \t)\r     ", true);

  // test parenthesis
  ASSERT_EXPR("(a.gte(100))", true);

  ASSERT_EXPR("a.gte(100) && a.gt(99)", true);
  ASSERT_EXPR("(a.gte(100) && a.gt(99))", true);
  ASSERT_EXPR("(a.gte(100) && (a.gt(99) || a.gt(97)))", true);

  ASSERT_EXPR("b.eq(2.0)", true);
  ASSERT_EXPR("b.eq(1.9999999)", false);
  ASSERT_EXPR("c.eq(\"my string\")", true);
  ASSERT_EXPR("c.contains(\"string\")", true);
  ASSERT_EXPR("c.contains(\"stringo\")", false);
  ASSERT_EXPR("c.contains('string')", true);
  ASSERT_EXPR("c.contains('stringo')", false);
  ASSERT_EXPR("c.in(\"string\")", false);
  ASSERT_EXPR("c.in(\"this is my string example\")", true);
  ASSERT_EXPR("c.in(\"string\\\"\")", false);
  ASSERT_EXPR("c.in(\"\\\"my string\\\"\")", true);

  // test errors
  ASSERT_ERROR_EXPR("", "expecting expression");
  ASSERT_ERROR_EXPR(",", "expecting identifier");
  ASSERT_ERROR_EXPR("+", "expecting identifier");
  ASSERT_ERROR_EXPR("*", "expecting identifier");
  ASSERT_ERROR_EXPR("(", "expecting expression");
  ASSERT_ERROR_EXPR("a", "expecting '.'");
  ASSERT_ERROR_EXPR("a,", "expecting '.'");
  ASSERT_ERROR_EXPR("a.", "expecting identifier");
  ASSERT_ERROR_EXPR("a.a", "expecting '('");
  ASSERT_ERROR_EXPR("j.k", "expecting '('");
  ASSERT_ERROR_EXPR("j.k(", "expecting value");
  ASSERT_ERROR_EXPR("j.k()", "expecting value");
  ASSERT_ERROR_EXPR("a.k(.3)", "expecting value");
  ASSERT_ERROR_EXPR("j.k(2.)", "variable 'j' not found");
  ASSERT_ERROR_EXPR("Jey.k(2.)", "variable 'Jey' not found");
  ASSERT_ERROR_EXPR("j.k(2.7", "expecting ')'");
  ASSERT_ERROR_EXPR("a.eq(2.00)", "type mismatch: type i vs f");
  ASSERT_ERROR_EXPR("a.eq(2) &", "unexpected token '&'");
  ASSERT_ERROR_EXPR("a.eq(2) |", "unexpected token '|'");
  ASSERT_ERROR_EXPR("a.eq(2) &&", "expecting expression");
  ASSERT_ERROR_EXPR("a.eq(2) ||", "expecting expression");
  ASSERT_ERROR_EXPR("a.eq(2) && (", "expecting expression");
  ASSERT_ERROR_EXPR("a.eq('", "unterminated string");
  ASSERT_ERROR_EXPR("a.eq('something like this", "unterminated string");
  ASSERT_ERROR_EXPR("a.eq(\"", "unterminated string");
  ASSERT_ERROR_EXPR("a.eq(\" whatever ", "unterminated string");

  return true;
}

bool benchmark(int npasses, int niterations) {
  TinyRuleChecker e;
  e.setVarInt("myint", 1);
  e.setVarFloat("myfloat", 2.0);
  e.setVarString("mystr", "my string");

  for (int n = 0; n < npasses; n++) {
    // test performance
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    for (int i = 0; i < niterations; i++) {
      ASSERT_EXPR("myfloat.eq(1.9999999) || myint.eq(32)", false);
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    printf(
      "Pass %d: %.3f M ops/sec  (%d in %.3f seconds)\n",
      n+1,
      ((float)niterations / 1e6) / elapsed_seconds.count(),
      niterations,
      elapsed_seconds.count()
    );
  }

  // test performance of 1000 ops
  for (int n = 0; n < npasses; n++) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    int thousand = 1000;
    for (int i = 0; i < thousand; i++) {
      ASSERT_EXPR("myfloat.eq(1.9999999) || myint.eq(32)", false);
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    printf(
      "Pass %d: %d ops in %.3f ms\n",
      n,
      thousand,
      ((float)thousand)*elapsed_seconds.count()
    );
  }
  return true;
}

int main() {
  //TinyRuleChecker::__generateLookupTable(); return -1;

  bool testPassed = test_all();
  printf (testPassed ? "Tests PASS!\n" : "One or more tests FAILED!\n");

  int niterations = 10 * 1000 * 1000;

  // override iterations from env variable
  const char *env_iterations = getenv("BENCHMARK_ITERATIONS");
  if (env_iterations) {
    niterations = atoi(env_iterations);
  }

  printf ("Running benchmark (n=%d)...\n", niterations);
  benchmark(3, niterations);
  return testPassed ? 0 : -1;
}
