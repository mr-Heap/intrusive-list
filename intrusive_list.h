#pragma once

#include <iostream> // delete later

namespace intrusive {
struct default_tag;

struct list_base {

  list_base * prev;
  list_base * next;

//  void unlink(list_base * node);

  list_base() = default;

  list_base(list_base * prev, list_base * next)
      : prev(prev)
      , next(next) {
  }

  ~list_base() {
//    unlink(prev);
//    unlink(next);
  }
};

template <typename Tag = default_tag>
struct list_element : list_base {
  using list_base::list_base;
};

template <typename T, typename Tag = default_tag>
struct list {

public:
  template <bool is_const>
  class my_iterator
  {
  public:

    list_base * it;

    template<bool WasConst, class = std::enable_if_t<is_const || !WasConst>>
    my_iterator(my_iterator<WasConst> rhs) : it(rhs.it) {}
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = list_element<Tag>;
//    using pointer = T*;
//    using reference = T&;
    using pointer = typename std::conditional<is_const, const T*, T*>::type;
    using reference = typename std::conditional<is_const, const T&, T&>::type;

    my_iterator() = default;

    my_iterator(list_base * it)
        : it(it)
    {
    }

//    my_iterator& operator=(my_iterator & other) {                             // TODO
//      this->it = other.it;
//    }

//    my_iterator& operator=(const my_iterator other) {
//      this->it = other.it;
//    }

    friend bool operator==(const my_iterator & lhs, const my_iterator & rhs)
    {
      return lhs.it == rhs.it;
    }

    friend bool operator!=(const my_iterator & lhs, const my_iterator & rhs)
    {
      return lhs.it != rhs.it;
    }

    reference operator*() const
    {
      return static_cast<reference>(*it);
    }

    pointer operator->() const
    {
      return static_cast<pointer>(static_cast<list_element<Tag>*>(it));
    }


    my_iterator & operator++()
    {
      it = it->next;
      return *this;
    }

    my_iterator operator++(int)
    {
      auto tmp = *this;
      operator++();
      return tmp;
    }

    my_iterator & operator--()
    {
      it = it->prev;
      return *this;
    }

    my_iterator operator--(int)
    {
      auto tmp = *this;
      operator--();
      return tmp;
    }

  };
  using iterator = my_iterator<false>;
  using const_iterator = my_iterator<true>;


  iterator begin() const {
    return iterator(sentinel.next);
  }

  iterator end() const {
    return iterator(sentinel.next->prev);
  }

  iterator insert (iterator next, T & element) {                                // you can't insert an element twice
    for (auto i = this->begin(); i != this->end(); ++i) {
      if (&(*i) == &element) {
        if (i == next) {
          ++next;
        }
        i = this->erase(i);
        --i;
      }
    }
    iterator a = iterator(static_cast<list_element<Tag> *>(&element));
    a.it->next = next.it;
    a.it->prev = (--next).it;
    next.it->next = a.it;
    ++next;
    ++next;
    next.it->prev = a.it;
    return a;
  }

  iterator erase (iterator position) {
    --position;
    position.it->next = position.it->next->next;
    position.it->next->prev = position.it;
    return ++position;
  }

  list() {
    sentinel.prev = &sentinel;
    sentinel.next = &sentinel;
  }

  list(list && other)
  {
    if (other.empty()) {
      this->sentinel.prev = &sentinel;
      this->sentinel.next = &sentinel;
      return;
    }

    this->sentinel.prev = other.sentinel.prev;
    this->sentinel.next = other.sentinel.next;

    this->sentinel.prev->next = &sentinel;
    this->sentinel.next->prev = &sentinel;

    other.sentinel.prev = &other.sentinel;
    other.sentinel.next = &other.sentinel;
  }

  list& operator=(list && other) {
    if (other.empty()) {
      this->sentinel.prev = &sentinel;
      this->sentinel.next = &sentinel;
      return *this;
    }

    this->sentinel.prev = other.sentinel.prev;
    this->sentinel.next = other.sentinel.next;

    this->sentinel.prev->next = &sentinel;
    this->sentinel.next->prev = &sentinel;

    other.sentinel.prev = &other.sentinel;
    other.sentinel.next = &other.sentinel;
    return *this;
  }

  void splice(const_iterator start, list<T> & from,
              const_iterator position_from,
              const_iterator position_to) {
    if (position_to == position_from) return;

    --position_to;
    position_from.it->prev->next = position_to.it->next;
    position_to.it->next->prev = position_from.it->prev;

    position_from.it->prev = start.it->prev;
    position_to.it->next = start.it;
    position_from.it->prev->next = position_from.it;
    position_to.it->next->prev = position_to.it;

  }

  void push_back(T& element)  {
    auto* tmp = static_cast<list_element<Tag>*>(&element);
    if (tmp->prev == tmp->next) {
      tmp->prev->prev = tmp->prev;
      tmp->prev->next = tmp->prev;
    }
    tmp->next = static_cast<list_base*>(&sentinel);
    tmp->prev = static_cast<list_base*>(sentinel.prev);
    sentinel.prev->next = tmp;
    sentinel.prev = tmp;
  }

  void push_front(T& element) {
    auto* tmp = static_cast<list_element<Tag>*>(&element);
    tmp->next = static_cast<list_base*>(sentinel.next);
    tmp->prev = static_cast<list_base*>(&sentinel);
    sentinel.next->prev = tmp;
    sentinel.next = tmp;
  }

  T& front() {
    if (empty()) {
      throw 123;
    }
    return *static_cast<T *>(static_cast<list_element<Tag> *>(sentinel.next));
  }

  const T& front() const {
    if (empty()) {
      throw 123;
    }
    return *static_cast<T *>(static_cast<list_element<Tag> *>(sentinel.next));
  }

  T& back() {
    if (empty()) {
      throw 123;
    }
    return *static_cast<T *>(static_cast<list_element<Tag> *>(sentinel.prev));
  }

  const T& back() const {
    if (empty()) {
      throw 123;
    }
    return *static_cast<T *>(static_cast<list_element<Tag> *>(sentinel.prev));
  }

  void pop_front() {

    sentinel.next->next->prev = &sentinel;
    sentinel.next = sentinel.next->next;

  }

  void pop_back() {

    sentinel.prev->prev->next = &sentinel;
    sentinel.prev = sentinel.prev->prev;

  }

  bool empty() const {
    return sentinel.next == &sentinel && sentinel.prev == &sentinel;
  }

  // destructor - ?

  list_element<Tag> sentinel;

};

} // namespace intrusive
