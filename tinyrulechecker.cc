// -----------------------------------------------------------------------------
// Copyright (C) 2024 Pau Sanchez
// MIT License
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <string>
#include <cstring>
#include <stdint.h>
#include <charconv>

#include "tinyrulechecker.h"

// -----------------------------------------------------------------------------
// TinyRuleChecker constructor
// -----------------------------------------------------------------------------
TinyRuleChecker::TinyRuleChecker(bool defaultMethods) {
  clearVars();
  clearMethods();

  if (defaultMethods) {
    initMethods();
  }
}

// -----------------------------------------------------------------------------
// TinyRuleChecker destructor
// -----------------------------------------------------------------------------
TinyRuleChecker::~TinyRuleChecker() {
  // free variables and methods
  clearVars();
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
  _variables.set(name, v);
}

// -----------------------------------------------------------------------------
// setVarFloat
// -----------------------------------------------------------------------------
void TinyRuleChecker::setVarFloat(const char *name, float value) {
  VarValue v;
  v.type = V_TYPE_FLOAT;
  v.floatval = value;
  _variables.set(name, v);
}

// -----------------------------------------------------------------------------
// setVarString
// -----------------------------------------------------------------------------
void TinyRuleChecker::setVarString(const char *name, const char *value) {
  VarValue v;
  v.type = V_TYPE_STRING;
  v.strval = value;
  _variables.set(name, v);
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
  _methods.set(name, method);
}

// -----------------------------------------------------------------------------
// initMethods
//
// Initialize all standard methods
// -----------------------------------------------------------------------------
void TinyRuleChecker::initMethods() {
#define ENSURE_SAME_TYPE(v1, v2) \
    if (v1.type != v2.type) { \
      eval.error = "type mismatch: type " + std::string(1, v1.type) + " vs " + std::string(1, v2.type); \
      return false; \
    } \

  setMethod("eq", [](const VarValue &v1, const VarValue &v2, EvalResult &eval) {
    ENSURE_SAME_TYPE(v1, v2);

    switch (v1.type) {
      case V_TYPE_INT:
        eval.result = v1.intval == v2.intval;
        break;
      case V_TYPE_FLOAT:
        eval.result = v1.floatval == v2.floatval;
        break;
      case V_TYPE_STRING:
        eval.result = v1.strval == v2.strval;
        break;
      default:
        eval.error = "unsupported operation 'eq' with type '" + std::string(1, v1.type) + "'";
        return false;
    }
    return true;
  });

  setMethod("neq", [](const VarValue &v1, const VarValue &v2, EvalResult &eval) {
    ENSURE_SAME_TYPE(v1, v2);

    switch (v1.type) {
      case V_TYPE_INT:
        eval.result = v1.intval != v2.intval;
        break;
      case V_TYPE_FLOAT:
        eval.result = v1.floatval != v2.floatval;
        break;
      case V_TYPE_STRING:
        eval.result = v1.strval != v2.strval;
        break;
      default:
        eval.error = "unsupported operation 'neq' with type '" + std::string(1, v1.type) + "'";
        return false;
    }
    return true;
  });

  setMethod("gt", [](const VarValue &v1, const VarValue &v2, EvalResult &eval) {
    ENSURE_SAME_TYPE(v1, v2);

    switch (v1.type) {
      case V_TYPE_INT:
        eval.result = v1.intval > v2.intval;
        break;
      case V_TYPE_FLOAT:
        eval.result = v1.floatval > v2.floatval;
        break;
      case V_TYPE_STRING:
        eval.result = v1.strval > v2.strval;
        break;
      default:
        eval.error = "unsupported operation 'gt' with type '" + std::string(1, v1.type) + "'";
        return false;
    }
    return true;
  });

  setMethod("gte", [](const VarValue &v1, const VarValue &v2, EvalResult &eval) {
    ENSURE_SAME_TYPE(v1, v2);

    switch(v1.type) {
      case V_TYPE_INT:
        eval.result = v1.intval >= v2.intval;
        break;
      case V_TYPE_FLOAT:
        eval.result = v1.floatval >= v2.floatval;
        break;
      case V_TYPE_STRING:
        eval.result = v1.strval >= v2.strval;
        break;
      default:
        eval.error = "unsupported operation 'gte' with type '" + std::string(1, v1.type) + "'";
        return false;
    }
    return true;
  });

  setMethod("lt", [](const VarValue &v1, const VarValue &v2, EvalResult &eval) {
    ENSURE_SAME_TYPE(v1, v2);

    switch (v1.type) {
      case V_TYPE_INT:
        eval.result = v1.intval < v2.intval;
        break;
      case V_TYPE_FLOAT:
        eval.result = v1.floatval < v2.floatval;
        break;
      case  V_TYPE_STRING:
        eval.result = v1.strval < v2.strval;
        break;
      default:
        eval.error = "unsupported operation 'lt' with type '" + std::string(1, v1.type) + "'";
        return false;
    }
    return true;
  });

  setMethod("lte", [](const VarValue &v1, const VarValue &v2, EvalResult &eval) {
    ENSURE_SAME_TYPE(v1, v2);

    switch (v1.type) {
      case V_TYPE_INT:
        eval.result = v1.intval <= v2.intval;
        break;
      case V_TYPE_FLOAT:
        eval.result = v1.floatval <= v2.floatval;
        break;
      case  V_TYPE_STRING:
        eval.result = v1.strval <= v2.strval;
        break;
      default:
        eval.error = "unsupported operation 'lte' with type '" + std::string(1, v1.type) + "'";
        return false;
    }
    return true;
  });

  setMethod("contains", [](const VarValue &v1, const VarValue &v2, EvalResult &eval) {
    if (v1.type == V_TYPE_STRING) {
      eval.result = v1.strval.find(v2.strval) != std::string::npos;
    }
    else {
      eval.error = "unsupported operation 'contains' with type '" + std::string(1, v1.type) + "'";
      return false;
    }
    return true;
  });

  setMethod("in", [](const VarValue &v1, const VarValue &v2, EvalResult &eval) {
    if (v2.type == V_TYPE_STRING) {
      eval.result = v2.strval.find(v1.strval) != std::string::npos;
    }
    else if (v2.type == V_TYPE_ARRAY) {
      for (const VarValue &v : v2.array) {
        if (v1.type == v.type) {
          switch (v1.type) {
            case V_TYPE_INT:
              if (v1.intval == v.intval) {
                eval.result = true;
                return true;
              }
              break;
            case V_TYPE_FLOAT:
              if (v1.floatval == v.floatval) {
                eval.result = true;
                return true;
              }
              break;
            case V_TYPE_STRING:
              if (v1.strval == v.strval) {
                eval.result = true;
                return true;
              }
              break;
            default:
              break;
          }
        }
      }
      eval.result = false;
    }
    else {
      eval.error = "unsupported operation 'in' with type '" + std::string(1, v2.type) + "'";
      return false;
    }
    return true;
  });
}

// -----------------------------------------------------------------------------
// eval
//
// Returns TRUE if the expression is valid and the result is stored in the
// 'result' parameter. Returns FALSE otherwise.
// -----------------------------------------------------------------------------
TinyRuleChecker::EvalResult
TinyRuleChecker::eval(const char *expr) {
  ParseState ps { expr };
  EvalResult er;

  // no matter if error or not, we are assigning both vars
  _parseExpr(ps);

  // we should have consumed everything, otherwise there's an error
  if (ps.error.empty() && _peekToken(ps.next, ps.token)) {
    er.error = "unexpected token \'" + std::string(ps.token.value) + "\'";
    return er;
  }

  er.result = ps.result;
  er.error = ps.error;

  return er;
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
bool TinyRuleChecker::_parseExpr(ParseState &ps) {
  if (!_peekToken(ps.next, ps.token)) {
    ps.error = "expecting expression";
    return false;
  }

  if (ps.token.type == TK_LPAR) {
    // consume LPAR
    ps.next = _nextToken(ps.next, ps.token);

    // then expression
    if (!_parseExpr(ps)) {
      // preserve error by parseExpr
      return false;
    }

    // then RPAR
    ps.next = _nextToken(ps.next, ps.token);
    if (ps.token.type != TK_RPAR) {
      ps.error = "expecting ')'";
      return false;
    }
  }
  else if (!_parseStatement(ps)) {
    // preserve error by parseStatement
    return false;
  }

  // let's see what's ahead (we might need to process some extra things or
  // we might just want to return to higher level, e.g still ")..." to be
  // processed)
  _peekToken(ps.next, ps.token);
  switch(ps.token.type) {
    case TK_AND:
      {
        // consume AND
        ps.next = _nextToken(ps.next, ps.token);

        bool result = ps.result;
        if (!_parseExpr(ps))
          return false;

        ps.result &= result;
        return true;
      }
      break;

    case TK_OR:
      {
        // consume OR
        ps.next = _nextToken(ps.next, ps.token);

        bool result = ps.result;
        if (!_parseExpr(ps))
          return false;

        ps.result |= result;
      }
      return true;
    default:
      // nothing else to manage on purpose
      break;
  }

  // let's allow high-level function to continue processing if needed
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
bool
TinyRuleChecker::_parseStatement(ParseState &ps) {
  ps.next = _nextToken(ps.next, ps.token);
  if (ps.next == NULL) {
    ps.error = "expecting statement";
    return false;
  }

  // optional 'not' operator
  if (ps.token.type == TK_NOT) {
    if (!_parseStatement(ps))
      return false;

    ps.result = !ps.result;
    return true;
  }

  // expecting an identifier
  if (ps.token.type != TK_ID) {
    ps.error = "expecting identifier";
    return false;
  }

  std::string_view id = ps.token.value;

  // then expecting a dot
  ps.next = _nextToken(ps.next, ps.token);
  if (ps.token.type != TK_DOT) {
    ps.error = "expecting '.'";
    return false;
  }

  // then another identifier
  ps.next = _nextToken(ps.next, ps.token);
  if (ps.token.type != TK_ID) {
    ps.error = "expecting identifier";
    return false;
  }

  std::string_view method = ps.token.value;

  // then a '('
  ps.next = _nextToken(ps.next, ps.token);
  if (ps.token.type != TK_LPAR) {
    ps.error = "expecting '('";
    return false;
  }

  // then should parse a value
  VarValue value;
  if (!_parseValue(ps, value)) {
    // preserve error by parseValue
    return false;
  }

  // then a ')'
  ps.next = _nextToken(ps.next, ps.token);
  if (ps.token.type != TK_RPAR) {
    ps.error = "expecting ')'";
    return false;
  }

  // evaluate the statement inline
  const VarValue *pVar = _variables.get(id);
  if (pVar == NULL) {
    ps.error = "variable '" + std::string(id) + "' not found";
    return false;
  }
  return _evalStatement(ps, *pVar, method, value);
}

// -----------------------------------------------------------------------------
// _parseValue
// -----------------------------------------------------------------------------
bool
TinyRuleChecker::_parseValue(ParseState &ps, VarValue &v) {
  ps.next = _nextToken(ps.next, ps.token);

  switch (ps.token.type) {
    case TK_INT:
      // we know is a valid int value
      v.type = V_TYPE_INT;
      v.intval = ps.token.intval;
      return true;

    case TK_FLOAT:
      v.type = V_TYPE_FLOAT;
      v.floatval = ps.token.floatval;
      return true;

    case TK_RAW_STRING:
    case TK_RAW_STRING_NO_ESCAPE:
      {
        v.type = V_TYPE_STRING;
        v.strval = ps.token.value;

        // no escape sequences, thus, we are done!
        if (ps.token.type == TK_RAW_STRING_NO_ESCAPE)
          return true;

        // TK_RAW_STRING requires unescaping the string
        size_t pos = 0;
        for (size_t i = 0; i < v.strval.size(); i++) {
          if (v.strval[i] == '\\') {
            if (i + 1 < v.strval.size()) {
              switch (v.strval[i+1]) {
                case 'n': v.strval[i] = '\n'; break;
                case 'r': v.strval[i] = '\r'; break;
                case 't': v.strval[i] = '\t'; break;
                case '0': v.strval[i] = '\0'; break;
                case '\\': v.strval[i] = '\\'; break;
                case '\'': v.strval[i] = '\''; break;
                case '"': v.strval[i] = '"'; break;
                default: break;
              }
              v.strval[pos++] = v.strval[i];
            }
          }
          else {
            v.strval[pos++] = v.strval[i];
          }
        }
        v.strval.resize(pos);
      }
      return true;

    case TK_UNTERMINATED_STRING:
      ps.error = "unterminated string";
      return false;

    case TK_ID:
      {
        const VarValue *pVar = _variables.get(ps.token.value);
        if (pVar == NULL) {
          ps.error = "variable '" + std::string(ps.token.value) + "' not found";
          return false;
        }

        v = *pVar;
      }
      return true;

    case TK_LBRACE:
      {
        v.type = V_TYPE_ARRAY;
        v.array.clear();

        _peekToken(ps.next, ps.token);
        while (ps.token.type != TK_RBRACE) {
          VarValue vtmp;
          if (!_parseValue(ps, vtmp)) {
            // preserve error by parseValue
            return false;
          }

          v.array.push_back(vtmp);

          // then a ',' or end of array
          ps.next = _nextToken(ps.next, ps.token);
          if (ps.token.type == TK_RBRACE) {
            break;
          }
          else if (ps.token.type != TK_COMMA) {
            ps.error = "expecting ','";
            return false;
          }
        }

        // end of array found, we are done!
        if (ps.token.type != TK_RBRACE) {
          ps.error = "expecting ']'";
          return false;
        }
      }
      return true;

    default:
      ps.error = "expecting value, got " + _stringifyToken(ps.token);
  }

  return false;
}

// -----------------------------------------------------------------------------
// _evalStatement
// -----------------------------------------------------------------------------
bool TinyRuleChecker::_evalStatement(
  ParseState &ps,
  const VarValue &v1,
  const std::string_view &method,
  const VarValue &v2
) {
  // NOTE: type compatibility is left to the method implementation
  // if (v1.type != v2.type) {
  //   ps.error = "type mismatch: type " + std::string(1, v1.type) + " vs " + std::string(1, v2.type);
  //   return false;
  // }

  const MethodOperator *pMethod = _methods.get(method);
  if (pMethod == NULL) {
    ps.error = "unknown method '" + std::string(method) + "'";
    return false;
  }

  EvalResult evalResult;
  if (!(*pMethod)(v1, v2, evalResult)) {
    ps.error = evalResult.error;
    return false;
  }

  ps.result = evalResult.result;
  return true;
}

// -----------------------------------------------------------------------------
// _peekToken
//
// peek next token without consuming it, returns TRUE if there is a token and
// FALSE otherwise
// -----------------------------------------------------------------------------
bool TinyRuleChecker::_peekToken(const char *expr, Token &t) {
  return _nextToken(expr, t) != NULL;
}

// -----------------------------------------------------------------------------
// __generateLookupTable
//
// generate a lookup table for token types where the index is the character
// (only considering that the character is the first one of a token)
// -----------------------------------------------------------------------------
void TinyRuleChecker::__generateLookupTable() {
  printf ("static const char _TOKEN_LOOKUP_TABLE[256] = {\n  ");
  for (int i = 0; i < 256; i++) {
    if (i == 0) printf("'%c'", TK_EOF);
    else if (isspace(i)) printf("'%c'", TK_SPACE);
    else if (isalpha(i) || i == '_') printf("'%c'", TK_ID);
    else if (i == '"' || i == '\'') printf("'%c'", TK_RAW_STRING);
    else if (isdigit(i) || i == '-'  || i == '+') printf("'%c'", TK_INT);
    else if (isspace(i)) printf("'%c'", TK_UNKNOWN);
    else if (i == '(') printf("'%c'", TK_LPAR);
    else if (i == ')') printf("'%c'", TK_RPAR);
    else if (i == '[') printf("'%c'", TK_LBRACE);
    else if (i == ']') printf("'%c'", TK_RBRACE);
    else if (i == '&') printf("'%c'", TK_AND);
    else if (i == '|') printf("'%c'", TK_OR);
    else if (i == '!') printf("'%c'", TK_NOT);
    else if (i == '.') printf("'%c'", TK_DOT);
    else if (i == ',') printf("'%c'", TK_COMMA);
    else printf("'%c'", TK_UNKNOWN);

    if (i < 255) printf(",");
    if (i % 16 == 15) printf("\n  ");
  }
  printf ("};\n");

  // alphanumeric and '_' accepted as second character onwards
  // for an identifier
  printf ("static const char _TOKEN_LOOKUP_ID[256] = {\n  ");
  for (int i = 0; i < 256; i++) {
    if (i == 0) printf("'%c'", TK_EOF);
    else if (isalnum(i) || i == '_') printf("'%c'", TK_ID);
    else if (isspace(i)) printf("'%c'", ' ');
    else printf("'%c'", TK_UNKNOWN);

    if (i < 255) printf(",");
    if (i % 16 == 15) printf("\n  ");
  }
  printf ("};\n");
}

// -----------------------------------------------------------------------------
// lookup table for token types where the index is the character
// NOTE: generated with __generateLookupTable()
// -----------------------------------------------------------------------------
static const char _TOKEN_LOOKUP_TABLE[256] = {
  'e','u','u','u','u','u','u','u','u',' ',' ',' ',' ',' ','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  ' ','!','s','u','u','u','&','s','(',')','u','n',',','n','.','u',
  'n','n','n','n','n','n','n','n','n','n','u','u','u','u','u','u',
  'u','i','i','i','i','i','i','i','i','i','i','i','i','i','i','i',
  'i','i','i','i','i','i','i','i','i','i','i','[','u',']','u','i',
  'u','i','i','i','i','i','i','i','i','i','i','i','i','i','i','i',
  'i','i','i','i','i','i','i','i','i','i','i','u','|','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u'
};

// -----------------------------------------------------------------------------
// lookup table with characters accepted as identifier after first character
// (alphanum + '_')
// NOTE: generated with __generateLookupTable()
// -----------------------------------------------------------------------------
static const char _TOKEN_LOOKUP_ID[256] = {
  'e','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'i','i','i','i','i','i','i','i','i','i','u','u','u','u','u','u',
  'u','i','i','i','i','i','i','i','i','i','i','i','i','i','i','i',
  'i','i','i','i','i','i','i','i','i','i','i','u','u','u','u','i',
  'u','i','i','i','i','i','i','i','i','i','i','i','i','i','i','i',
  'i','i','i','i','i','i','i','i','i','i','i','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u',
  'u','u','u','u','u','u','u','u','u','u','u','u','u','u','u','u'
};

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

  if (expr == NULL || *expr == '\0') {
    t.type = TK_EOF;
    t.value = std::string_view{};
    return NULL;
  }

  // while (isspace(*expr)) {
  // while (*expr == ' ' || *expr == '\n' || *expr == '\r' || *expr == '\t') {
  while (_TOKEN_LOOKUP_TABLE[*expr] == TK_SPACE) {
    expr++;
  }

  const char *start_expr = expr;
  t.type = (TokenType)_TOKEN_LOOKUP_TABLE[*expr];
  switch(t.type) {
    case TK_ID:
      expr++; // letter or underscore
      // while (isalnum(*expr) || *expr == '_') {
      while(_TOKEN_LOOKUP_ID[*expr] == TK_ID) {
        expr++;
      }
      t.value = std::string_view(start_expr, expr - start_expr);
      return expr;

    case TK_RAW_STRING:
      {
        bool escapeCharFound = false;
        char quoteChar = *expr;
        expr++;

        start_expr = expr;
        while (*expr && *expr != quoteChar) {
          // skip escaped characters for now, will be processed later
          escapeCharFound |= (*expr == '\\');
          expr += (*expr == '\\');

          expr++;
        }
        t.value = std::string_view(start_expr, expr - start_expr);

        if (*expr == quoteChar) {
          t.type = escapeCharFound ? TK_RAW_STRING : TK_RAW_STRING_NO_ESCAPE;
          expr++;
        }
        else {
          // EOF reached without closing quote
          t.type = TK_UNTERMINATED_STRING;
        }
      }
      break;

    case TK_INT:
      {
        bool negative = *expr == '-';
        t.intval = (*expr >= '0' && *expr <= '9') ?  *expr - '0' : 0;

        // skip first digit or sign
        expr++;

        //while(isdigit(*expr)) {
        while (*expr >= '0' && *expr <= '9') {
          t.intval = t.intval * 10 + (*expr - '0');
          expr++;
        }

        if (negative) {
          t.intval = -t.intval;
        }

        if (*expr == '.') {
          float fraction = 0;
          float divisor = 1.0;
          t.type = TK_FLOAT;
          expr++;
          //while (isdigit(*expr)) {
          while (*expr >= '0' && *expr <= '9') {
            fraction = fraction * 10 + (*expr - '0');
            divisor *= 10;
            expr++;
          }

          t.floatval  = (float)t.intval;
          t.floatval += fraction / divisor;
        }
        t.value = std::string_view(start_expr, expr - start_expr);
      }
      break;

    case TK_LPAR:
      t.value = std::string_view{expr, 1};
      expr++;
      break;

    case TK_RPAR:
      t.value = std::string_view{expr, 1};
      expr++;
      break;

    case TK_LBRACE:
      t.value = std::string_view{expr, 1};
      expr++;
      break;

    case TK_RBRACE:
      t.value = std::string_view{expr, 1};
      expr++;
      break;

    case TK_AND:
      if (*(expr+1) == '&') {
        t.value = std::string_view{expr, 2};
        expr+=2;
      }
      else {
        t.type = TK_UNKNOWN;
        t.value = std::string_view{expr, 1};
        return expr;
      }
      break;

    case TK_OR:
      if (*(expr+1) == '|') {
        t.value = std::string_view{expr, 2};
        expr+=2;
      }
      else {
        t.type = TK_UNKNOWN;
        t.value = std::string_view{expr, 1};
        return expr;
      }
      break;

    case TK_NOT:
      t.value = std::string_view{expr, 1};
      expr++;
      break;

    case TK_DOT:
      t.value = std::string_view{expr, 1};
      expr++;
      break;

    case TK_COMMA:
      t.value = std::string_view{expr, 1};
      expr++;
      break;

    case TK_EOF:
      // can happen if expression ends with spaces
      t.value = std::string_view{};
      return NULL;

    case TK_SPACE:
      // spaces should not appear here
    default:
      t.type = TK_UNKNOWN;
      t.value = std::string_view{expr, 1};
      expr++;
      break;
  }

  return expr;
}

// -----------------------------------------------------------------------------
// _stringifyToken
// -----------------------------------------------------------------------------
std::string TinyRuleChecker::_stringifyToken(const Token &t) {
  switch (t.type) {
    case TK_INT: return std::to_string(t.intval);
    case TK_FLOAT: return std::to_string(t.floatval);
    case TK_RAW_STRING:
    case TK_RAW_STRING_NO_ESCAPE:
      return "string '" + std::string(t.value) + "'";
    case TK_UNTERMINATED_STRING:
      return "unterminated string (" + std::string(t.value) + ")";
    case TK_EOF: return "EOF";
    default:
      return "'" + std::string(1, t.type) + "'";
  }
}
