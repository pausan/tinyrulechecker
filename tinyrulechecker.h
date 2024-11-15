// -----------------------------------------------------------------------------
// Copyright (C) 2024 Pau Sanchez
// MIT License
// -----------------------------------------------------------------------------
#ifndef __TinyRuleChecker_h__
#define __TinyRuleChecker_h__

#include <string>
#include <stdint.h>
#include <map>

class TinyRuleChecker {
  public:

    typedef enum {
      V_TYPE_INT = 'i',
      V_TYPE_FLOAT = 'f',
      V_TYPE_STRING = 's'
    } VarType;

    typedef struct {
      VarType     type;
      int32_t     intval;
      float       floatval;
      std::string strval;
    } VarValue;

    typedef struct {
      bool        result;
      std::string error;
    } EvalResult;

    typedef bool (*MethodOperator)(const VarValue &v1, const VarValue &v2, EvalResult &result);

    TinyRuleChecker(bool defaultMethods = true);
    ~TinyRuleChecker();

    void clearVars();
    void setVarInt(const char *name, int value);
    void setVarFloat(const char *name, float value);
    void setVarString(const char *name, const char *value);

    void clearMethods();
    void initMethods();
    void setMethod(const char *name, MethodOperator method);

    EvalResult eval(const char *expr);

  private:
    typedef enum {
      TK_UNKNOWN = 'u',
      TK_ID = 'i',
      TK_INT = 'n',
      TK_FLOAT = 'f',
      TK_RAW_STRING = 's',
      TK_RAW_STRING_NO_ESCAPE = 'S', // no escape chars inside
      TK_UNTERMINATED_STRING = 't',
      TK_AND = '&',
      TK_OR = '|',
      TK_NOT = '!',
      TK_DOT = '.',
      TK_LPAR = '(',
      TK_RPAR = ')',
      TK_EOF = 'e'
    } TokenType;

    static void __generateLookupTable();

    typedef struct {
      TokenType        type;
      std::string_view value;
    } Token;

    typedef struct {
      const char *next;
      Token       token;
      bool        result;
      std::string error;
    } ParseState;

    std::map<std::string, VarValue, std::less<>> _variables;
    std::map<std::string, MethodOperator, std::less<>> _methods;
    const char *_nextToken(const char *expr, Token &t);
    bool _peekToken(const char *expr, Token &t);

    bool _parseExpr(ParseState &ps);
    bool _parseStatement(ParseState &ps);
    bool _parseValue(ParseState &ps, VarValue &v);
    bool _evalStatement(ParseState &ps, const VarValue &v1, const std::string_view &method, const VarValue &v2);
};


#endif