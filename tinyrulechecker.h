// -----------------------------------------------------------------------------
// Copyright (C) 2024 Pau Sanchez
// MIT License
// -----------------------------------------------------------------------------
#ifndef __TinyRuleChecker_h__
#define __TinyRuleChecker_h__

#include <string>
#include <cstring>
#include <stdint.h>
#include <map>
#include <vector>

// -----------------------------------------------------------------------------
// FastStringLookup
//
// The main idea is to have a fast lookup table for strings, where we can just
// look for a hash lookup, if we don't find we know is not there, if we find
// and the index is less than the size of the lookup table, we can compare with
// the original string, if it is the same, then we have the index to the value;
// in any other case we'll need to use the lookup map.
//
// Note that this is not a normal hash table, because here in case of collissions
// we don't try to insert the new element anywhere, we just mark it as a collission
// which will get resolved by using the lookup map.
// -----------------------------------------------------------------------------
template<typename T>
class FastStringLookup {
  public:
    FastStringLookup() {
      // up to X fast elements (still works with more, but slower)
      _lookup.resize(1021, 0);
      _lookupNames.resize(_lookup.size());
    }
    ~FastStringLookup() {
      clear();
    }

    void clear();
    void set(const std::string &key, T value);
    const T *get(const std::string &key) const;
    const T *get(const std::string_view &key) const;

  private:
    static uint32_t _fnvHash32v(const uint8_t *data, size_t n);
    std::map<std::string, uint32_t, std::less<>> _lookupMap;
    std::vector<uint32_t>    _lookup;
    std::vector<std::string> _lookupNames;
    std::vector<T>           _values;
};

// -----------------------------------------------------------------------------
// TinyRuleChecker
// -----------------------------------------------------------------------------
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

    typedef bool (*MethodOperator)(
      const VarValue &v1,
      const VarValue &v2,
      EvalResult &result
    );

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
      TK_SPACE = ' ',
      TK_EOF = 'e'
    } TokenType;

    public: static void __generateLookupTable();

    typedef struct {
      TokenType        type;
      std::string_view value;
      int              intval;
      float            floatval;
    } Token;

    typedef struct {
      const char *next;
      Token       token;
      bool        result;
      std::string error;
    } ParseState;

    FastStringLookup<VarValue> _variables;
    FastStringLookup<MethodOperator> _methods;

    const char *_nextToken(const char *expr, Token &t);
    bool _peekToken(const char *expr, Token &t);

    bool _parseExpr(ParseState &ps);
    bool _parseStatement(ParseState &ps);
    bool _parseValue(ParseState &ps, VarValue &v);
    bool _evalStatement(ParseState &ps, const VarValue &v1, const std::string_view &method, const VarValue &v2);
};

// -----------------------------------------------------------------------------
// FastStringLookup<T>::_fnvHash32v
//
// FNV-1a hash function over a string/binary data
// -----------------------------------------------------------------------------
template<typename T>
uint32_t FastStringLookup<T>::_fnvHash32v(const uint8_t *data, size_t n) {
  const uint32_t PRIME = 16777619;
  uint32_t result = 0;

  // continue iterating through the rest of the string
  for(size_t i = 0; i < n; i++) {
    result ^= data[i];
    result *= PRIME;
  }

  return result;
}

// -----------------------------------------------------------------------------
// FastStringLookup<T>::clear
// -----------------------------------------------------------------------------
template<typename T>
void FastStringLookup<T>::clear() {
  _lookupMap.clear();
  _values.clear();
  std::fill(_lookup.begin(), _lookup.end(), 0);
}

// -----------------------------------------------------------------------------
// FastStringLookup<T>::set
// -----------------------------------------------------------------------------
template<typename T>
void FastStringLookup<T>::set(const std::string &key, T value) {
  uint32_t index = _values.size();
  uint32_t qkey = _fnvHash32v((const uint8_t*)key.c_str(), key.size()) % _lookup.size();

  _values.push_back(value);

  _lookupMap[key] = index;
  _lookup[qkey] = (_lookup[qkey] == 0 ? index + 1 : _lookup.size());
  _lookupNames[qkey] = key;
}

// -----------------------------------------------------------------------------
// FastStringLookup<T>::get
// -----------------------------------------------------------------------------
template<typename T>
const T *FastStringLookup<T>::get(const std::string_view &key) const {
  uint32_t qkey = _fnvHash32v((const uint8_t*)key.data(), key.size()) % _lookup.size();
  uint32_t index = _lookup[qkey];
  if (index == 0) {
    return NULL;
  }
  else if (index < _lookup.size()) {
    // validate we are talking about same name
    if (_lookupNames[qkey].size() != key.size()) {
      return NULL;
    }

    return (memcmp(_lookupNames[qkey].data(), key.data(), key.size()) == 0)
      ? &_values[index - 1] : NULL;
  }

  // index = _lookup.size() means duplicates, thus, we need to use lookup map
  auto it = _lookupMap.find(key);
  if (it == _lookupMap.end()) {
    return NULL;
  }

  return &_values[it->second];
}

// -----------------------------------------------------------------------------
// FastStringLookup<T>::get
// -----------------------------------------------------------------------------
template<typename T>
const T *FastStringLookup<T>::get(const std::string &key) const {
  return get(std::string_view(key));
}

#endif