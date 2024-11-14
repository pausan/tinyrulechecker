// -----------------------------------------------------------------------------
// Copyright (C) 2024 Pau Sanchez
// MIT License
// -----------------------------------------------------------------------------
#ifndef __TinyRuleChecker_h__
#define __TinyRuleChecker_h__

#include <string>
#include <stdint.h>
#include <map>
#include <vector>

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

    typedef bool (*MethodOperator)(const VarValue &v1, const VarValue &v2, bool &result);

    TinyRuleChecker(bool defaultMethods = true);
    ~TinyRuleChecker();

    void clearVars();
    void setVarInt(const char *name, int value);
    void setVarFloat(const char *name, float value);
    void setVarString(const char *name, const char *value);

    void clearMethods();
    void initMethods();
    void setMethod(const char *name, MethodOperator method);

    bool eval(const char *expr, bool &result);
  private:
    typedef enum {
      TK_UNKNOWN = 'u',
      TK_ID = 'd',
      TK_INT = 'i',
      TK_FLOAT = 'f',
      TK_STRING = 's',
      TK_AND = '&',
      TK_OR = '|',
      TK_NOT = '!',
      TK_DOT = '.',
      TK_LPAR = '(',
      TK_RPAR = ')',
    } TokenType;

    typedef struct {
      int              offset; // instead of line, col, just the offset
      TokenType        type;
      std::string_view value;
    } Token;

    typedef std::vector<Token> TokenList;
    typedef TokenList::const_iterator TokenIt;

    std::map<std::string, VarValue, std::less<>> _variables;
    std::map<std::string, MethodOperator, std::less<>> _methods;
    TokenList tokenize(const char *expr);
    const char *_nextToken(const char *expr, Token &t);

    bool _parseStatement(TokenList &tokens, TokenIt &it, bool &result);
    bool _parseExpr(TokenList &tokens, TokenIt &it, bool &result);
    bool _parseValue(TokenList &tokens, TokenIt &it, VarValue &v);
    bool _evalStatement(const VarValue &v1, const std::string_view &method, const VarValue &v2, bool &result);
};


#endif