// Copyright © 2012, Université catholique de Louvain
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

#ifndef __ATOM_H
#define __ATOM_H

#include "mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

//////////
// Atom //
//////////

#include "Atom-implem.hh"

void Atom::create(atom_t& self, VM vm, GR gr, Self from) {
  self = gr->copyAtom(from.get().value());
}

bool Atom::equals(VM vm, Self right) {
  return value() == right.get().value();
}

int Atom::compareFeatures(VM vm, Self right) {
  atom_t lhs = value();
  atom_t rhs = right.get().value();

  return lhs.compare(rhs);
}

void Atom::compare(Self self, VM vm, RichNode right, int& result) {
  atom_t rightAtomValue;
  getArgument(vm, rightAtomValue, right, MOZART_STR("atom"));

  result = value().compare(rightAtomValue);
}

void Atom::toString(Self self, VM vm, std::basic_ostream<nchar>& sink) {
  atom_t a = value();
  if (a != vm->coreatoms.nil && a != vm->coreatoms.sharp) {
    sink.write(a.contents(), a.length());
  }
}

void Atom::vsLength(Self self, VM vm, nativeint& result) {
  atom_t a = value();
  if (a == vm->coreatoms.nil || a == vm->coreatoms.sharp)
    result = 0;
  else
    result = codePointCount(makeLString(a.contents(), a.length()));
}

void Atom::printReprToStream(Self self, VM vm, std::ostream& out, int depth) {
  atom_t a = value();
  out << '\'' << toUTF<char>(makeLString(a.contents(), a.length())) << '\'';
}

}

#endif // MOZART_GENERATOR

#endif // __ATOM_H
