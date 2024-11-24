# Tiny Rule Checker: Simple Expression Condition Checker

## Intro

Born as a proof of concept to make a very simple condition expressions
evaluator.

Syntax is very basic and resembles high-level object-oriented expressions like
those of C++/Rust/JS/Python. It only supports very basic operations.

The idea was for this project to be used on a rule engine where it would check
conditions based on the given expression.

The expressions are straighforward, as easy as:

```txt
varname '.' method '(' value ')'
```

Where `varname` is an id defined in the code and `value` is a string, int, float
or an array of any of such items.

There is a set of predefined methods that you can totally override/extend/get rid of.

Operations return a boolean value and you can chain them using `and` (`&&`) and `or` (`||`) operators:

```txt
(myint.eq(10) && myfloat.gt(10.5)) || mystring.eq("hello")
```

## Design

It has an embedded lexer and parser. Used C++ because of convenience of high-level
containers, although it would not be hard to port to C (maybe tedious). Uses
standard C++ map, vector and strings, no other external dependency.

Language is pretty damn simple. Decided not to use comparison operators so that
the language and operations are simpler to define and extend.

Very lightweight and reasonably fast (see benchmarks).

## C++ Basic Example

```cpp
#include <iostream>
#include "tinyrulechecker.h"

int main () {
  // Create a rule checker
  TinyRuleChecker checker;

  // Add a variable
  checker.setVarInt("myint", 10);
  checker.setVarFloat("myfloat", 10.5);
  checker.setVarString("mystring", "hello");

  // Add a rule
  bool result;
  if (checker.eval("myint.eq(10) && myfloat.gt(10.5) || mystring.eq('hello')", result)) {
    std::cout << "Result is" << result << std::endl;
  } else {
    std::cout << "Error evaluating expression" << std::endl;
  }
  return 0;
}
```

Compile with `g++ -O3 -o myexample myexample.cc tinyrulechecker.cc`

## C++ Advanced Example

Here is an advanced example with your own rules

```cpp
#include <iostream>
#include "tinyrulechecker.h"

int main () {
  // Create a rule checker
  TinyRuleChecker checker(false); // <-- do not import default methods

  // Add a variable
  checker.setVarInt("myint", 10);
  checker.setVarFloat("myfloat", 10.5);
  checker.setVarString("mystring", "hellohello");

  checker.setMethod("isDoubleOf", [](
    const TinyRuleChecker::VarValue &v1,
    const TinyRuleChecker::VarValue &v2,
    TinyRuleChecker::EvalResult &eval
  ) {
    if (v1.type == TinyRuleChecker::V_TYPE_INT) {
      eval.result = v1.intval == 2*v2.intval;
    }
    else if (v1.type == TinyRuleChecker::V_TYPE_FLOAT) {
      eval.result = v1.floatval == 2*v2.floatval;
    }
    else if (v1.type == TinyRuleChecker::V_TYPE_STRING) {
      eval.result = v1.strval == (v2.strval + v2.strval);
    }
    else {
      eval.error = "unsupported operation 'isDoubleOf' with type '" + std::string(1, v1.type) + "'";
      return false;
    }
    return true;
  });


  // Add a rule
  auto eval = checker.eval("myint.isDoubleOf(5) || mystring.isDoubleOf(\"hello\")");
  if (eval.error.empty()) {
    std::cout << "Result is: " << (eval.result ? "true" : "false") << std::endl;
  } else {
    std::cout << "Error evaluating expression: " << eval.error << std::endl;
  }
  return 0;
}
```

## X-Ray Profiling

Profile with:
```sh
$ clang++ --stdlib=libc++ -fxray-instrument -fxray-instruction-threshold=1 -O3  -ggdb3 -o test test.cc tinyrulechecker.cc
$ XRAY_OPTIONS="patch_premain=true xray_mode=xray-basic verbosity=1" ./test
$ llvm-xray convert --symbolize --instr_map=test --output-format=trace_event xray-log.test.* | gzip> test-trace.txt.gz"
```

View trace in https://ui.perfetto.dev/ or https://speedscope.app/

## Grammar

The language grammar is very simple, as follows:

```txt
S -> expr

expr      -> '(' expr ')'
          -> statement
          -> statement boolop expr

statement -> id '.' id '(' value ')'
          -> '!' statement

value -> id | int | float | string | array

array -> '[' value (',' value)* ']'

boolop -> '&&' | '||'
```

Strings can be enclosed in single or double quotes.

## Performance

Benchmark performed on an Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz (launched Q3
2018) and compiled with g++ 13.2.0 with -O3 on a linux machine on the **best** of 5 runs.

**Expression**

`myfloat.eq(1.9999999) || myint.eq(32)`

**Results**

- **287.2MB/s of data processed per second**
- **7.02 million evaluations per second**
- **1000 evaluations in 0.142 ms**
- **1 evaluation in 142.44 ns**

Please note that the full string is parsed and evaluated fully every time,
no cache, no pre-compilation step.

## License

Copyright 2024 Pau Sanchez

MIT Licensed. See LICENSE file.