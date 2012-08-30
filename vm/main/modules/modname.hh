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

#ifndef __MODNAME_H
#define __MODNAME_H

#include "../mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

namespace builtins {

/////////////////
// Name module //
/////////////////

class ModName: public Module {
public:
  ModName(): Module("Name") {}

  class New: public Builtin<New> {
  public:
    New(): Builtin("new") {}

    void operator()(VM vm, Out result) {
      result = OptName::build(vm);
    }
  };

  class NewUnique: public Builtin<NewUnique> {
  public:
    NewUnique(): Builtin("newUnique") {}

    void operator()(VM vm, In atom, Out result) {
      bool isAtom = false;
      RecordLike(atom).isRecord(vm, isAtom);

      if (isAtom) {
        result = UniqueName::build(vm, unique_name_t(atom.as<Atom>().value()));
      } else {
        return raiseTypeError(vm, MOZART_STR("Atom"), atom);
      }
    }
  };

  class NewNamed: public Builtin<NewNamed> {
  public:
    NewNamed(): Builtin("newNamed") {}

    void operator()(VM vm, In atom, Out result) {
      bool isAtom = false;
      RecordLike(atom).isRecord(vm, isAtom);

      if (isAtom) {
        result = NamedName::build(vm, atom);
      } else {
        return raiseTypeError(vm, MOZART_STR("Atom"), atom);
      }
    }
  };

  class Is: public Builtin<Is> {
  public:
    Is(): Builtin("is") {}

    void operator()(VM vm, In value, Out result) {
      bool boolResult = false;
      NameLike(value).isName(vm, boolResult);

      result = Boolean::build(vm, boolResult);
    }
  };
};

}

}

#endif // MOZART_GENERATOR

#endif // __MODNAME_H
