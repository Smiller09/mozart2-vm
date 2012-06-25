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

#ifndef __STORAGE_H
#define __STORAGE_H

#include "core-forward-decl.hh"

#include "memword.hh"
#include "arrays.hh"

namespace mozart {

template<class I, class E>
class ImplWithArray {
  I* p;
public:
  ImplWithArray(I* p) : p(p) {}

  I* operator->() {
    return p;
  }

  E& operator[](size_t i) {
    return getRawArray()[i];
  }

  StaticArray<E> getArray(size_t size) {
    return StaticArray<E>(getRawArray(), size);
  }
private:
  template <class T, class U>
  friend class Accessor;

  E* getRawArray() {
    return static_cast<E*>(static_cast<void*>(
      static_cast<char*>(static_cast<void*>(p)) + sizeof(I)));
  }
};

// Marker class that specifies to use the default storage (pointer to value)
template<class T>
class DefaultStorage {
};

// Meta-function from Type to its storage
template<class T>
class Storage {
public:
  typedef DefaultStorage<T> Type;
};

template<class T, class U>
class Accessor {
public:
  template<class... Args>
  static void init(const Type*& type, MemWord& value, VM vm, Args&&... args) {
    type = T::type();
    value.alloc<U>(vm);
    Implementation<T>::build(value.get<U>(), vm, std::forward<Args>(args)...);
  }

  static Implementation<T> get(MemWord value) {
    return Implementation<T>(value.get<U>());
  }
};

template<class T>
class Accessor<T, DefaultStorage<T>> {
public:
  typedef Implementation<T> Impl;

  template<class... Args>
  static void init(const Type*& type, MemWord& value, VM vm, Args&&... args) {
    type = T::type();
    Impl* val = new (vm) Impl(vm, std::forward<Args>(args)...);
    value.init<Impl*>(vm, val);
  }

  static Impl& get(MemWord value) {
    return *(value.get<Impl*>());
  }
};

template<class T, class E>
class Accessor<T, ImplWithArray<Implementation<T>, E>> {
public:
  typedef Implementation<T> Impl;

  template<class... Args>
  static void init(const Type*& type, MemWord& value, VM vm,
                   size_t elemCount, Args&&... args) {
    // Allocate memory
    void* memory = operator new (sizeof(Impl) + elemCount*sizeof(E), vm);
    ImplWithArray<Impl, E> implWithArray(static_cast<Impl*>(memory));

    // Initialize the array
    E* array = implWithArray.getRawArray();
    new (array) E[elemCount];

    // Initialize the impl
    Impl* impl = implWithArray.operator->();
    new (impl) Impl(vm, elemCount, implWithArray.getArray(elemCount),
                    std::forward<Args>(args)...);

    // Fill in output parameters
    type = T::type();
    value.init<Impl*>(vm, impl);
  }

  static Impl& get(MemWord value) {
    return *(value.get<Impl*>());
  }
};

}

#endif // __STORAGE_H
