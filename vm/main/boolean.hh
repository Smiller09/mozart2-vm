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

#ifndef __BOOLEAN_H
#define __BOOLEAN_H

#include "mozartcore.hh"

#ifndef MOZART_GENERATOR

namespace mozart {

/////////////
// Boolean //
/////////////

#include "Boolean-implem.hh"

void Boolean::create(bool& self, VM vm, GR gr, Self from) {
  self = from.get().value();
}

bool Boolean::equals(VM vm, Self right) {
  return value() == right.get().value();
}

int Boolean::compareFeatures(VM vm, Self right) {
  if (value() == right.get().value())
    return 0;
  else if (value())
    return 1;
  else
    return -1;
}

void Boolean::toString(Self self, VM vm, std::basic_ostream<nchar>& sink) {
  sink << (value() ? MOZART_STR("true") : MOZART_STR("false"));
}

nativeint Boolean::vsLength(Self self, VM vm) {
  return value() ? 4 : 5;
}

}

#endif // MOZART_GENERATOR

#endif // __BOOLEAN_H
