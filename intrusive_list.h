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

    int *d_;

    template<bool WasConst, class = std::enable_if_t<is_const || !WasConst>>
    my_iterator(my_iterator<WasConst> rhs) : d_(rhs.d_) {}

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = list_element<Tag>;
    using pointer = T*;
    using reference = T&;

    my_iterator() = default;

    list_base * it;

    my_iterator(list_base * it)
        : it(it)
    {
    }

//    my_iterator& operator=(my_iterator & other) {
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
      return static_cast<T&>(*it);
    }

    pointer operator->() const
    {
      return static_cast<T*>(static_cast<list_element<Tag>*>(it));
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

  iterator insert (iterator next, T & element) {
      iterator a = iterator(static_cast<list_element<Tag> *>(&element));
      next.it->prev->next = a.it;
      a.it->next = next.it;
      a.it->prev = (--next).it;
      next.it->prev = a.it;
      ++next;
      ++next;
      return a;
  }

  iterator erase (iterator position) {
    --position;
    position.it->next = position.it->next->next;
    position.it->next->next->prev = position.it;
    return ++position;
  }

  list() {
    sentinel.prev = &sentinel;
    sentinel.next = &sentinel;
  }

  list(list && other)
  {
    this->sentinel = other.sentinel;
    if (other.empty()) {
      this->sentinel.prev = &sentinel;
      this->sentinel.next = &sentinel;
    }
    other.sentinel.prev = &other.sentinel;
    other.sentinel.next = &other.sentinel;
  }

  list& operator=(list && other) {
    this->sentinel = other.sentinel;
    if (other.empty()) {
      this->sentinel.prev = &sentinel;
      this->sentinel.next = &sentinel;
    }
    other.sentinel.prev = &other.sentinel;
    other.sentinel.next = &other.sentinel;
    return *this;
  }

  void splice(const_iterator start, list<T> & from,
              const_iterator position_from,
              const_iterator position_to) {
//    --start;
//    start.it->next = position_from.it;
//
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