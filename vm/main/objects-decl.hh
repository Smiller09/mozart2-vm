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

#ifndef __OBJECTS_DECL_H
#define __OBJECTS_DECL_H

#include "mozartcore-decl.hh"

#include "opcodes.hh"
#include "datatypeshelpers-decl.hh"

namespace mozart {

////////////
// Object //
////////////

class Object;

#ifndef MOZART_GENERATOR
#include "Object-implem-decl.hh"
#endif

/**
 * Object
 */
class Object: public DataType<Object>, public WithHome,
  StoredWithArrayOf<UnstableNode> {
public:
  typedef SelfType<Object>::Self Self;
private:
  // defined in coredatatypes.cc
  static const ByteCode dispatchByteCode[9];
public:
  static atom_t getTypeAtom(VM vm) {
    return vm->getAtom(MOZART_STR("object"));
  }

  inline
  Object(VM vm, size_t attrCount, StaticArray<UnstableNode> _attributes,
         RichNode clazz, RichNode attrModel, RichNode featModel);

  inline
  Object(VM vm, size_t attrCount, StaticArray<UnstableNode> _attributes,
         GR gr, Self from);

private:
  inline
  bool isFreeFlag(VM vm, RichNode value);

public:
  size_t getArraySize() {
    return _attrCount;
  }

public:
  // Dottable interface

  inline
  void lookupFeature(Self self, VM vm, RichNode feature,
                     bool& found, nullable<UnstableNode&> value);

  inline
  void lookupFeature(Self self, VM vm, nativeint feature,
                     bool& found, nullable<UnstableNode&> value);

public:
  // ChunkLike interface

  void isChunk(Self self, VM vm, bool& result) {
    result = true;
  }

public:
  // ObjectLike interface

  void isObject(Self self, VM vm, bool& result) {
    result = true;
  }

  inline
  void getClass(Self self, VM vm, UnstableNode& result);

  inline
  void attrGet(Self self, VM vm, RichNode attribute, UnstableNode& result);

  inline
  void attrPut(Self self, VM vm, RichNode attribute, RichNode value);

  inline
  void attrExchange(Self self, VM vm, RichNode attribute,
                    RichNode newValue, UnstableNode& oldValue);

private:
  inline
  void getAttrOffset(Self self, VM vm, RichNode attribute, size_t& offset);

public:
  // Callable interface

  void isCallable(Self self, VM vm, bool& result) {
    result = true;
  }

  void isProcedure(Self self, VM vm, bool& result) {
    result = false;
  }

  inline
  void procedureArity(Self self, VM vm, size_t& result);

  inline
  void getCallInfo(Self self, VM vm, size_t& arity,
                   ProgramCounter& start, size_t& Xcount,
                   StaticArray<StableNode>& Gs,
                   StaticArray<StableNode>& Ks);

  inline
  void getDebugInfo(Self self, VM vm,
                    atom_t& printName, UnstableNode& debugData);

public:
  void printReprToStream(Self self, VM vm, std::ostream& out, int depth) {
    out << "<Object>";
  }

private:
  StableNode _attrArity;
  size_t _attrCount;

  bool _GsInitialized;

  StableNode _clazz;
  StableNode _features;

  StableNode _Gs[2];
};

#ifndef MOZART_GENERATOR
#include "Object-implem-decl-after.hh"
#endif

}

#endif // __OBJECTS_DECL_H
