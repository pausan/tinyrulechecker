// -----------------------------------------------------------------------------
// Copyright (C) 2024 Pau Sanchez
// MIT License
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <map>
#include "tinyrulechecker.h"

// -----------------------------------------------------------------------------
// TinyRuleChecker constructor
// -----------------------------------------------------------------------------
TinyRuleChecker::TinyRuleChecker(bool defaultMethods) {
  if (defaultMethods)
    initMethods();
}

// -----------------------------------------------------------------------------
// TinyRuleChecker destructor
// -----------------------------------------------------------------------------
TinyRuleChecker::~TinyRuleChecker() {
  // do nothing
}

// -----------------------------------------------------------------------------
// Clear internal variables
// -----------------------------------------------------------------------------
void TinyRuleChecker::clearVars() {
  _variables.clear();
}

// -----------------------------------------------------------------------------
// setVarInt
// -----------------------------------------------------------------------------
void TinyRuleChecker::setVarInt(const char *name, int32_t value) {
  VarValue v;
  v.type = V_TYPE_INT;
  v.intval = value;
  _variables[name] = v;
}

// -----------------------------------------------------------------------------
// setVarFloat
// -----------------------------------------------------------------------------
void TinyRuleChecker::setVarFloat(const char *name, float value) {
  VarValue v;
  v.type = V_TYPE_FLOAT;
  v.floatval = value;
  _variables[name] = v;
}

// -----------------------------------------------------------------------------
// setVarString
// -----------------------------------------------------------------------------
void TinyRuleChecker::setVarString(const char *name, const char *value) {
  VarValue v;
  v.type = V_TYPE_STRING;
  v.strval = value;
  _variables[name] = v;
}

// -----------------------------------------------------------------------------
// Clear internal methods
// -----------------------------------------------------------------------------
void TinyRuleChecker::clearMethods() {
  _methods.clear();
}

// -----------------------------------------------------------------------------
// setMethod
// -----------------------------------------------------------------------------
void TinyRuleChecker::setMethod(const char *name, TinyRuleChecker::MethodOperator method) {
  _methods[name] = method;
}

// -----------------------------------------------------------------------------
// initMethods
//
// Initialize all standard methods
// -----------------------------------------------------------------------------
void TinyRuleChecker::initMethods() {
  _methods["eq"] = [](const VarValue &v1, const VarValue &v2, bool &result) {
    if (v1.type == V_TYPE_INT) {
      result = v1.intval == v2.intval;
    }
    else if (v1.type == V_TYPE_FLOAT) {
      result = v1.floatval == v2.floatval;
    }
    else if (v1.type == V_TYPE_STRING) {
      result = v1.strval == v2.strval;
    }
    else {
      printf("unsupported operation 'eq' with type '%c'\n", v1.type);
      return false;
    }
    return true;
  };

  _methods["neq"] = [](const VarValue &v1, const VarValue &v2, bool &result) {
    if (v1.type == V_TYPE_INT) {
      result = v1.intval != v2.intval;
    }
    else if (v1.type == V_TYPE_FLOAT) {
      result = v1.floatval != v2.floatval;
    }
    else if (v1.type == V_TYPE_STRING) {
      result = v1.strval != v2.strval;
    }
    else {
      printf("unsupported operation 'neq' with type '%c'\n", v1.type);
      return false;
    }
    return true;
  };

  _methods["gt"] = [](const VarValue &v1, const VarValue &v2, bool &result) {
    if (v1.type == V_TYPE_INT) {
      result = v1.intval > v2.intval;
    }
    else if (v1.type == V_TYPE_FLOAT) {
      result = v1.floatval > v2.floatval;
    }
    else {
      printf("unsupported operation 'gt' with type '%c'\n", v1.type);
      return false;
    }
    return true;
  };

  _methods["gte"] = [](const VarValue &v1, const VarValue &v2, bool &result) {
    if (v1.type == V_TYPE_INT) {
      result = v1.intval >= v2.intval;
    }
    else if (v1.type == V_TYPE_FLOAT) {
      result = v1.floatval >= v2.floatval;
    }
    else {
      printf("unsupported operation 'gte' with type '%c'\n", v1.type);
      return false;
    }
    return true;
  };

  _methods["lt"] = [](const VarValue &v1, const VarValue &v2, bool &result) {
    if (v1.type == V_TYPE_INT) {
      result = v1.intval < v2.intval;
    }
    else if (v1.type == V_TYPE_FLOAT) {
      result = v1.floatval < v2.floatval;
    }
    else {
      printf("unsupported operation 'lt' with type '%c'\n", v1.type);
      return false;
    }
    return true;
  };

  _methods["lte"] = [](const VarValue &v1, const VarValue &v2, bool &result) {
    if (v1.type == V_TYPE_INT) {
      result = v1.intval <= v2.intval;
    }
    else if (v1.type == V_TYPE_FLOAT) {
      result = v1.floatval <= v2.floatval;
    }
    else {
      printf("unsupported operation 'lte' with type '%c'\n", v1.type);
      return false;
    }
    return true;
  };

  _methods["contains"] = [](const VarValue &v1, const VarValue &v2, bool &result) {
    if (v1.type == V_TYPE_STRING) {
      result = v1.strval.find(v2.strval) != std::string::npos;
    }
    else {
      printf("unsupported operation 'contains' with type '%c'\n", v1.type);
      return false;
    }
    return true;
  };

  _methods["in"] = [](const VarValue &v1, const VarValue &v2, bool &result) {
    if (v2.type == V_TYPE_STRING) {
      result = v2.strval.find(v1.strval) != std::string::npos;
    }
    else {
      printf("unsupported operation 'in' with type '%c'\n", v2.type);
      return false;
    }
    return true;
  };
}

// -----------------------------------------------------------------------------
// eval
//
// Returns TRUE if the expression is valid and the result is stored in the
// 'result' parameter. Returns FALSE otherwise.
// -----------------------------------------------------------------------------
bool TinyRuleChecker::eval(const char *expr, bool &result) {
  TokenList tokens = tokenize(expr);
  TokenIt it = tokens.begin();
  bool ok = _parseExpr(tokens, it, result);
  if (it != tokens.end()) {
    printf("unexpected token at offset %d\n", it->offset);
    return false;
  }
  return ok;
}

// -----------------------------------------------------------------------------
// _parseExpr
//
// parse an expression and returns the result in the 'result' parameter.
//
// expr      -> '(' expr ')'
//           -> statement
//           -> statement boolop expr
// -----------------------------------------------------------------------------
bool TinyRuleChecker::_parseExpr(TokenList &tokens, TokenIt &it, bool &result) {
  if (it == tokens.end()) {
    return false;
  }

  if (it->type == TK_LPAR) {
    it++; // lpar

    if (!_parseExpr(tokens, it, result)) {
      return false;
    }

    if (it->type == TK_RPAR) {
      it++;
      return true;
    }
    else {
      printf ("expecting ')'\n");
      return false;
    }
  }

  if (!_parseStatement(tokens, it, result)) {
    return false;
  }

  if (it == tokens.end()) {
    return true;
  }

  if (it->type == TK_AND) {
    it++;

    bool exprResult = false;
    if (!_parseExpr(tokens, it, exprResult)) {
      return false;
    }
    result &= exprResult;
    return true;
  }
  else if (it->type == TK_OR) {
    it++;

    bool exprResult = false;
    if (!_parseExpr(tokens, it, exprResult)) {
      return false;
    }
    result |= exprResult;
    return true;
  }

  // continue parsing, the expresion might continue at a higher level
  return true;
}

// -----------------------------------------------------------------------------
// _parseStatement
//
// parse a statement and returns the result in the 'result' parameter.
//
// statement -> id '.' id '(' value ')'
//           -> 'not' statement
// -----------------------------------------------------------------------------
bool TinyRuleChecker::_parseStatement(TokenList &tokens, TokenIt &it, bool &result) {
  if (it == tokens.end()) {
    return false;
  }

  // optional 'not' operator
  if (it->type == TK_NOT) {
    it++;
    if (!_parseStatement(tokens, it, result)) {
      return false;
    }
    result = !result;
    return true;
  }

  // expecting an identifier
  if (it->type != TK_ID) {
    printf("expecting identifier\n");
    return false;
  }

  std::string id = it->value;
  it++;

  // then expecting a dot
  if (it->type != TK_DOT) {
    printf("expecting '.'\n");
    return false;
  }
  it++;

  // then another identifier
  if (it->type != TK_ID) {
    printf("expecting identifier\n");
    return false;
  }

  std::string method = it->value;
  it++;

  // then a '('
  if (it->type != TK_LPAR) {
    printf("expecting '('\n");
    return false;
  }
  it++;

  // then should parse a value
  VarValue value;
  if (!_parseValue(tokens, it, value)) {
    return false;
  }

  // then a ')'
  if (it->type != TK_RPAR) {
    printf("expecting ')'\n");
    return false;
  }
  it++;

  // now we can evaluate the statement
  if (_variables.find(id) == _variables.end()) {
    printf("variable '%s' not found\n", id.c_str());
    return false;
  }

  VarValue &var = _variables[id];
  return _evalStatement(var, method, value, result);
}

// -----------------------------------------------------------------------------
// _parseValue
// -----------------------------------------------------------------------------
bool TinyRuleChecker::_parseValue(TokenList &tokens, TokenIt &it, VarValue &v) {
  if (it == tokens.end()) {
    return false;
  }

  if (it->type == TK_INT) {
    v.type = V_TYPE_INT;
    v.intval = atoi(it->value.c_str());
    it++;
    return true;
  }
  else if (it->type == TK_FLOAT) {
    v.type = V_TYPE_FLOAT;
    v.floatval = atof(it->value.c_str());
    it++;
    return true;
  }
  else if (it->type == TK_STRING) {
    v.type = V_TYPE_STRING;
    v.strval = it->value; // FIXME! remove quotes & escape sequences
    it++;
    return true;
  }

  printf("expecting value\n");
  return false;
}

// -----------------------------------------------------------------------------
// _evalStatement
// -----------------------------------------------------------------------------
bool TinyRuleChecker::_evalStatement(
  const VarValue &v1,
  const std::string &method,
  const VarValue &v2,
  bool &result
) {
  if (v1.type != v2.type) {
    printf("type mismatch: type %c vs %c\n", v1.type, v2.type);
    return false;
  }

  std::map<std::string, MethodOperator>::iterator mit = _methods.find(method);
  if (mit == _methods.end()) {
    printf("unknown method '%s'\n", method.c_str());
    return false;
  }

  return mit->second(v1, v2, result);
}

// -----------------------------------------------------------------------------
// tokenize
//
// get a list of all tokens in given expression
// -----------------------------------------------------------------------------
TinyRuleChecker::TokenList TinyRuleChecker::tokenize(const char *expr) {
  TokenList tokens;
  Token t;

  const char *myexpr = expr;
  while ((myexpr = _nextToken(myexpr, t)) != NULL) {
    t.offset = myexpr - expr;
    tokens.push_back(t);
  }

  return tokens;
}

// -----------------------------------------------------------------------------
// _nextToken
//
// get next token evaluating given expression and returning a pointer to the
// next character after the token (or NULL if no more tokens)
//
// this function will eat all spaces first
// -----------------------------------------------------------------------------
const char *TinyRuleChecker::_nextToken(const char *expr, Token &t) {
  t.type = TK_UNKNOWN;
  t.value = "";
  t.offset = 0;

  if (expr == NULL || *expr == '\0') {
    return NULL;
  }

  while (isspace(*expr)) {
    expr++;
  }

  if (isalpha(*expr) || *expr == '_') {
    t.type = TK_ID;
    while (isalnum(*expr) || *expr == '_') {
      t.value += *expr;
      expr++;
    }
  }
  else if (*expr == '"') {
    t.type = TK_STRING;
    expr++;
    while (*expr && *expr != '"') {
      t.value += *expr;
      expr++;
    }
    if (*expr == '"') {
      expr++;
    }
  }
  else if (*expr == '\'') {
    t.type = TK_STRING;
    expr++;
    while (*expr && *expr != '\'') {
      t.value += *expr;
      expr++;
    }
    if (*expr == '\'') {
      expr++;
    }
  }
  else if (isdigit(*expr)) {
    t.type = TK_INT;
    while (isdigit(*expr)) {
      t.value += *expr;
      expr++;
    }

    if (*expr == '.') {
      t.type = TK_FLOAT;
      t.value += *expr;
      expr++;
      while (isdigit(*expr)) {
        t.value += *expr;
        expr++;
      }
    }
  }
  else if (*expr == '(') {
    t.type = TK_LPAR;
    t.value = "(";
    expr++;
  }
  else if (*expr == ')') {
    t.type = TK_RPAR;
    t.value = ")";
    expr++;
  }
  else if (*expr == '&' && *(expr+1) == '&') {
    t.type = TK_AND;
    t.value = "&&";
    expr+=2;
  }
  else if (*expr == '|' && *(expr+1) == '|') {
    t.type = TK_OR;
    t.value = "||";
    expr+=2;
  }
  else if (*expr == '!') {
    t.type = TK_NOT;
    t.value = "!";
    expr++;
  }
  else if (*expr == '.') {
    t.type = TK_DOT;
    t.value = ".";
    expr++;
  }
  else {
    t.type = TK_UNKNOWN;
    t.value = *expr;
    expr++;
  }

  if (*expr == '\0' && t.type == TK_UNKNOWN) {
    return NULL;
  }

  return expr;
}