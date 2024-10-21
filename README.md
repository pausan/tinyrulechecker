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

Where `varname` is an id defined in the code and `value` is a string, int or float.
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

Compile with `g++ -o myexample myexample.cc tinyrulechecker.cc`


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

  checker.setMethod("isDoubleOf", [](const TinyRuleChecker::VarValue &v1, const TinyRuleChecker::VarValue &v2, bool &result) {
    if (v1.type == TinyRuleChecker::V_TYPE_INT) {
      result = v1.intval == 2*v2.intval;
    }
    else if (v1.type == TinyRuleChecker::V_TYPE_FLOAT) {
      result = v1.floatval == 2*v2.floatval;
    }
    else if (v1.type == TinyRuleChecker::V_TYPE_STRING) {
      result = v1.strval == (v2.strval + v2.strval);
    }
    else {
      std::cout << "unsupported operation 'eq' with type" << v1.type << std::endl;
      return false;
    }
    return true;
  });


  // Add a rule
  bool result;
  if (checker.eval("myint.isDoubleOf(5) || mystring.isDoubleOf(\"hello\")", result)) {
    std::cout << "Result is" << result << std::endl;
  } else {
    std::cout << "Error evaluating expression" << std::endl;
  }
  return 0;
}
```

## Grammar

The language grammar is very simple, as follows:

```txt
S -> expr

expr      -> '(' expr ')'
          -> statement
          -> statement boolop expr

statement -> id '.' id '(' value ')'
          -> '!' statement

value -> int | float | string

boolop -> '&&' | '||'
```

Strings can be enclosed in single or double quotes.

## Performance

Benchmark performed on an Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz (launched Q3
2018) and compiled with g++ 13.2.0 with -O3 on a linux machine on the best of 3 runs.

**Expression**

`myfloat.eq(1.9999999) || myint.eq(32)`

**Results**
- Less than 1 millisecond to parsed and evaluate 1000 times (0.96 ms)
- 1.24 million evaluations per second (fully parsing and evaluating each time, no precompilation step)

## License

MIT Licensed. See LICENSE file.

