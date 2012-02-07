// Copyright © 2011, Université catholique de Louvain
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef __CALLABLES_H
#define __CALLABLES_H

#include "type.hh"
#include "smallint.hh"
#include "codearea.hh"

#include <iostream>

//////////////////////
// BuiltinProcedure //
//////////////////////

/**
 * Type of a builtin function
 */
typedef BuiltinResult (*OzBuiltin)(VM vm, UnstableNode* args[]);

class BuiltinProcedure;

template <>
class Implementation<BuiltinProcedure> {
public:
  Implementation<BuiltinProcedure>(int arity, OzBuiltin builtin) :
    _arity(arity), _builtin(builtin) {}

  /**
   * Arity of this builtin
   */
  int getArity() const { return _arity; }

  /**
   * Call the builtin
   * @param vm     Contextual VM
   * @param argc   Actual number of parameters
   * @param args   Actual parameters
   */
  BuiltinResult callBuiltin(Node* self, VM vm, int argc, UnstableNode* args[]) {
    if (argc == _arity)
      return _builtin(vm, args);
    else
      return raiseIllegalArity(argc);
  }

  /**
   * Get the arity of the builtin in a node
   */
  BuiltinResult arity(Node* self, VM vm, UnstableNode* result) {
    result->make<SmallInt>(vm, (nativeint) _arity);
    return BuiltinResultContinue;
  }
private:
  BuiltinResult raiseIllegalArity(int argc);

  const int _arity;
  const OzBuiltin _builtin;
};

/**
 * Type of a builtin procedure
 */
class BuiltinProcedure {
public:
  typedef Node* Self;

  static const Type* const type;
private:
  static const Type rawType;
};

/////////////////
// Abstraction //
/////////////////

class Abstraction;

/**
 * Abstraction value, i.e., user-defined procedure
 */
template <>
class Implementation<Abstraction> {
public:
  Implementation<Abstraction>(VM vm, int arity, UnstableNode* body,
    int Gc, UnstableNode* Gs[]);

  int getArity() { return _arity; }

  /**
   * Get the arity of the abstraction in a node
   */
  BuiltinResult arity(Node* self, VM vm, UnstableNode* result) {
    result->make<SmallInt>(vm, (nativeint) _arity);
    return BuiltinResultContinue;
  }

  /**
   * Get the information needed to call this abstraction
   * @param vm       Contextual VM
   * @param arity    Output: arity of this abstraction
   * @param body     Output: code area which is the body
   * @param start    Output: start of the code area
   * @param Xcount   Output: number of X registers used by the code area
   * @param Gs       Output: G registers
   * @param Ks       Output: K registers
   */
  inline
  BuiltinResult getCallInfo(Node* self, VM vm, int* arity, StableNode** body,
                            ProgramCounter* start, int* Xcount,
                            StaticArray<StableNode>** Gs,
                            StaticArray<StableNode>** Ks);
private:
  int _arity;
  StableNode _body;
  StaticArray<StableNode> _Gs;

  // cache for information of the code area
  bool _codeAreaCacheValid;
  ProgramCounter _start;
  int _Xcount;
  StaticArray<StableNode>* _Ks;
};

/**
 * Type of an abstraction
 */
class Abstraction {
public:
  typedef Node* Self;

  static const Type* const type;
private:
  static const Type rawType;
};

////////////////////////
// Inline Abstraction //
////////////////////////

#include "coreinterfaces.hh"

BuiltinResult Implementation<Abstraction>::getCallInfo(
  Node* self, VM vm, int* arity, StableNode** body, ProgramCounter* start,
  int* Xcount, StaticArray<StableNode>** Gs, StaticArray<StableNode>** Ks) {

  if (!_codeAreaCacheValid) {
    CodeAreaProvider provider = _body.node;
    BuiltinResult result = provider.getCodeAreaInfo(
      vm,&_start, &_Xcount, &_Ks);

    if (result != BuiltinResultContinue)
      return result;

    _codeAreaCacheValid = true;
  }

  *arity = _arity;
  *body = &_body;
  *start = _start;
  *Xcount = _Xcount;
  *Gs = &_Gs;
  *Ks = _Ks;

  return BuiltinResultContinue;
}

#endif // __CALLABLES_H
