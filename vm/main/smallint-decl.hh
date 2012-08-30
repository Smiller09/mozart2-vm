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

#ifndef __SMALLINT_DECL_H
#define __SMALLINT_DECL_H

#include "mozartcore-decl.hh"

namespace mozart {

class SmallInt;

#ifndef MOZART_GENERATOR
#include "SmallInt-implem-decl.hh"
#endif

class SmallInt: public DataType<SmallInt>, Copyable, StoredAs<nativeint>,
  WithValueBehavior {
public:
  typedef SelfType<SmallInt>::Self Self;
public:
  static constexpr UUID uuid = "{00000000-0000-4f00-0000-000000000001}";

  static atom_t getTypeAtom(VM vm) {
    return vm->getAtom(MOZART_STR("int"));
  }

  SmallInt(nativeint value) : _value(value) {}

  static void create(nativeint& self, VM vm, nativeint value) {
    self = value;
  }

  inline
  static void create(nativeint& self, VM vm, GR gr, Self from);

public:
  nativeint value() const { return _value; }

  inline
  bool equals(VM vm, Self right);

  inline
  int compareFeatures(VM vm, Self right);

public:
  // Comparable interface

  inline
  void compare(Self self, VM vm, RichNode right, int& result);

public:
  // IntegerValue inteface

  void intValue(Self self, VM vm, nativeint& result) {
    result = value();
  }

  inline
  void equalsInteger(Self self, VM vm, nativeint right, bool& result);

public:
  // Numeric inteface

  void isNumber(Self self, VM vm, bool& result) {
    result = true;
  }

  void isInt(Self self, VM vm, bool& result) {
    result = true;
  }

  void isFloat(Self self, VM vm, bool& result) {
    result = false;
  }

  inline
  void opposite(Self self, VM vm, UnstableNode& result);

  inline
  void add(Self self, VM vm, RichNode right, UnstableNode& result);

  inline
  void addValue(Self self, VM vm, nativeint b, UnstableNode& result);

  inline
  void subtract(Self self, VM vm, RichNode right, UnstableNode& result);

  inline
  void subtractValue(Self self, VM vm, nativeint b, UnstableNode& result);

  inline
  void multiply(Self self, VM vm, RichNode right, UnstableNode& result);

  inline
  void multiplyValue(Self self, VM vm, nativeint b, UnstableNode& result);

  inline
  void divide(Self self, VM vm, RichNode right, UnstableNode& result);

  inline
  void div(Self self, VM vm, RichNode right, UnstableNode& result);

  inline
  void divValue(Self self, VM vm, nativeint b, UnstableNode& result);

  inline
  void mod(Self self, VM vm, RichNode right, UnstableNode& result);

  inline
  void modValue(Self self, VM vm, nativeint b, UnstableNode& result);

public:
  // VirtualString inteface

  void isVirtualString(Self self, VM vm, bool& result) {
    result = true;
  }

  inline
  void toString(Self self, VM vm, std::basic_ostream<nchar>& sink);

  inline
  void vsLength(Self self, VM vm, nativeint& result);

public:
  // Miscellaneous

  void printReprToStream(Self self, VM vm, std::ostream& out, int depth) {
    out << value();
  }

private:
  inline
  bool testMultiplyOverflow(nativeint a, nativeint b);

  const nativeint _value;
};

#ifndef MOZART_GENERATOR
#include "SmallInt-implem-decl-after.hh"
#endif

}

#endif // __SMALLINT_DECL_H
