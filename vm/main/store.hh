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

#ifndef __STORE_H
#define __STORE_H

#include "core-forward-decl.hh"
#include "memword.hh"
#include "storage.hh"
#include "type.hh"

/**
 * A value node in the store.
 * The store is entirely made of nodes. A node is basically a typed value.
 * Non-atomic values, such as records, contain references to other nodes in the
 * store, hence forming a graph, and the name "node".
 * There are two kinds of node: stable and unstable node. A stable node is
 * guaranteed never to change, whereas unstable node can change. In order to
 * maintain consistency in the store, non-atomic values are only allowed to
 * reference stable nodes. Unstable nodes are used for working data, and
 * inherently mutable data (such as the contents of a cell).
 */
class Node {
public:
  template<class T, class... Args>
  void make(VM vm, Args... args) {
    typedef Accessor<T, typename Storage<T>::Type> Access;
    Access::init(type, value, vm, args...);
  }

  inline void reset(VM vm);

  const Type* type;
  MemWord value;
};

/**
 * Stable node, which is guaranteed never to change
 */
class StableNode {
public:
  inline void init(VM vm, UnstableNode& from);
private:
  friend class UnstableNode;
public: // TODO make it private once the development has been bootstrapped
  union {
    Node node;

    // Garbage collector hack
    struct {
      StableNode* gcNext;
      Node* gcFrom;
    };
  };
};

/**
 * Unstable node, which is allowed to change over time
 */
class UnstableNode {
public:
  UnstableNode() {}

  UnstableNode(VM vm, StableNode& from) {
    copy(vm, from);
  }

  UnstableNode(VM vm, UnstableNode& from) {
    copy(vm, from);
  }

  inline void copy(VM vm, StableNode& from);
  inline void copy(VM vm, UnstableNode& from);
  inline void swap(UnstableNode& from);
  inline void reset(VM vm);

  template<class T, class... Args>
  void make(VM vm, Args... args) {
    node.make<T>(vm, args...);
  }
private:
  friend class StableNode;
public: // TODO make it private once the development has been bootstrapped
  union {
    Node node;

    // Garbage collector hack
    struct {
      UnstableNode* gcNext;
      Node* gcFrom;
    };
  };
};

/**
 * Base class for Self types
 */
template <class T>
class BaseSelf {
protected:
  typedef typename Storage<T>::Type StorageType;
  typedef Accessor<T, StorageType> Access;
public:
  BaseSelf(Node* node) : _node(node) {}

  template<class U, class... Args>
  void make(VM vm, Args... args) {
    _node->make<U>(vm, args...);
  }

  operator Node*() {
    return _node;
  }

  Node& operator*() {
    return *_node;
  }
protected:
  auto getBase() -> decltype(Access::get(MemWord())) {
    return Access::get(_node->value);
  }

  Node* _node;
};

/**
 * Self type for custom storage-based types
 */
template <class T>
class CustomStorageSelf: public BaseSelf<T> {
private:
  typedef Implementation<T> Impl;
public:
  CustomStorageSelf(Node* node) : BaseSelf<T>(node) {}

  Impl get() {
    return this->getBase();
  }
};

/**
 * Self type for default storage-based types
 */
template <class T>
class DefaultStorageSelf: public BaseSelf<T> {
private:
  typedef Implementation<T> Impl;
public:
  DefaultStorageSelf(Node* node) : BaseSelf<T>(node) {}

  Impl* operator->() {
    return &this->getBase();
  }
};

/**
 * Extractor function for the template parameters of ImplWithArray
 * Given
 *   typedef ImplWithArray<I, E> T;
 * this provides
 *   ExtractImplWithArray<T>::Impl === I
 *   ExtractImplWithArray<T>::Elem === E
 */
template <class S>
struct ExtractImplWithArray {};

template <class I, class E>
struct ExtractImplWithArray<ImplWithArray<I, E>> {
  typedef I Impl;
  typedef E Elem;
};

/**
 * Self type for ImplWithArray-based types
 */
template <class T>
class ImplWithArraySelf: public BaseSelf<T> {
private:
  typedef typename BaseSelf<T>::StorageType StorageType;
  typedef typename ExtractImplWithArray<StorageType>::Impl Impl;
  typedef typename ExtractImplWithArray<StorageType>::Elem Elem;
public:
  ImplWithArraySelf(Node* node) : BaseSelf<T>(node) {}

  Impl* operator->() {
    return get().operator->();
  }

  Elem& operator[](size_t i) {
    return get().operator[](i);
  }

  StaticArray<Elem> getArray(size_t size) {
    return get().getArray(size);
  }
private:
  ImplWithArray<Impl, Elem> get() {
    return ImplWithArray<Impl, Elem>(&this->getBase());
  }
};

/**
 * Helper for the metafunction SelfType
 */
template <class T, class S>
struct SelfTypeInner {
  typedef CustomStorageSelf<T> Self;
};

/**
 * Helper for the metafunction SelfType
 */
template <class T>
struct SelfTypeInner<T, DefaultStorage<T>> {
  typedef DefaultStorageSelf<T> Self;
};

/**
 * Helper for the metafunction SelfType
 */
template <class T, class I, class E>
struct SelfTypeInner<T, ImplWithArray<I, E>> {
  typedef ImplWithArraySelf<T> Self;
};

/**
 * Metafunction from type to its Self type
 * Use as SelfType<T>::Self
 */
template <class T>
struct SelfType {
  typedef typename SelfTypeInner<T, typename Storage<T>::Type>::Self Self;
};

/**
 * Result of the call to a builtin.
 * It always represents a node that must be waited upon. The value 'nullptr' is
 * valid, and denotes that no value must be waited upon, i.e., the execution can
 * continue.
 * Throwing an exception is achieved by pointing to a failed value.
 */
typedef Node* BuiltinResult;

const BuiltinResult BuiltinResultContinue = nullptr;

/**
 * Strange and magical class that allows to call methods on storage-typed nodes
 */
template<class T, class R, class M, M m>
class Impl {
public:
  typedef Accessor<T, typename Storage<T>::Type> Type;

  template<class... Args>
  static R f(Node* it, Args... args) {
    return (Type::get(it->value).*m)(typename SelfType<T>::Self(it), args...);
  }
};

#define IMPL(ResType, Type, method, args...) \
  (Impl<Type, ResType, decltype(&Implementation<Type>::method), \
    &Implementation<Type>::method>::f(args))

/**
 * Strange and magical class that allows to call methods on storage-typed nodes
 */
template<class T, class R, class M, M m>
class ImplNoSelf {
public:
  typedef Accessor<T, typename Storage<T>::Type> Type;

  template<class... Args>
  static R f(Node* it, Args... args) {
    return (Type::get(it->value).*m)(args...);
  }
};

#define IMPLNOSELF(ResType, Type, method, args...) \
  (ImplNoSelf<Type, ResType, decltype(&Implementation<Type>::method), \
    &Implementation<Type>::method>::f(args))

///////////////
// Reference //
///////////////

class Reference;

template <>
class Storage<Reference> {
public:
  typedef StableNode* Type;
};

template <>
class Implementation<Reference> {
public:
  Implementation<Reference>(StableNode* dest) : _dest(dest) {}
  static StableNode* build(VM, StableNode* dest) { return dest; }

  StableNode* dest() const { return _dest; }
private:
  StableNode* _dest;
};

/**
 * Type of a reference
 */
class Reference: public Type {
public:
  Reference() : Type("Reference", true) {}

  typedef Node* Self;

  static const Reference* const type() {
    static const Reference rawType;
    return &rawType;
  }

  void gCollect(GC gc, Node& from, StableNode& to) const {
    // TODO
  }

  void gCollect(GC gc, Node& from, UnstableNode& to) const {
    // TODO
  }

  // This is optimized for the 0- and 1-dereference paths
  // Normally it would have been only a while loop
  static Node& dereference(Node& node) {
    if (node.type != type())
      return node;
    else {
      Node* result = &IMPLNOSELF(StableNode*, Reference, dest, &node)->node;
      if (result->type != type())
        return *result;
      else
        return dereferenceLoop(result);
    }
  }

  static void makeFor(VM vm, UnstableNode& node) {
    StableNode* stable = new (vm) StableNode;
    stable->init(vm, node);
  }

  static void makeFor(VM vm, Node& node) {
    UnstableNode temp;
    temp.node = node;
    makeFor(vm, temp);
    node = temp.node;
  }

  // This is optimized for the 0- and 1-dereference paths
  // Normally the else case would have been only a while loop
  static StableNode* getStableRefFor(VM vm, Node& node) {
    if (node.type != type()) {
      makeFor(vm, node);
      return IMPLNOSELF(StableNode*, Reference, dest, &node);
    } else {
      StableNode* result = IMPLNOSELF(StableNode*, Reference, dest, &node);
      if (result->node.type != type())
        return result;
      else
        return getStableRefForLoop(result);
    }
  }

  static StableNode* getStableRefFor(VM vm, UnstableNode& node) {
    return getStableRefFor(vm, node.node);
  }

  static StableNode* getStableRefFor(VM vm, StableNode& node) {
    if (node.node.type != type())
      return &node;
    else
      return getStableRefFor(vm, node.node);
  }
private:
  static Node& dereferenceLoop(Node* node) {
    while (node->type == type())
      node = &(IMPLNOSELF(StableNode*, Reference, dest, node)->node);
    return *node;
  }

  static StableNode* getStableRefForLoop(StableNode* node) {
    do {
      node = IMPLNOSELF(StableNode*, Reference, dest, &node->node);
    } while (node->node.type == type());

    return node;
  }
};

/////////////////////////
// Node implementation //
/////////////////////////

void Node::reset(VM vm) {
  type = nullptr;
  value.init<void*>(vm, nullptr);
}

void StableNode::init(VM vm, UnstableNode& from) {
  node = from.node;
  if (!node.type->isCopiable())
    from.make<Reference>(vm, this);
}

void UnstableNode::copy(VM vm, StableNode& from) {
  if (from.node.type->isCopiable())
    node = from.node;
  else
    make<Reference>(vm, &from);
}

void UnstableNode::copy(VM vm, UnstableNode& from) {
  if (!from.node.type->isCopiable())
    Reference::makeFor(vm, from);
  node = from.node;
}

void UnstableNode::reset(VM vm) {
  node.reset(vm);
}

void UnstableNode::swap(UnstableNode& from) {
  Node temp = node;
  node = from.node;
  from.node = temp;
}

#include "vm.hh"

#endif // __STORE_H