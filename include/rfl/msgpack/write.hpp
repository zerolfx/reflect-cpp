
#ifndef RFL_MSGPACK_WRITE_HPP_
#define RFL_MSGPACK_WRITE_HPP_
#include "Parser.hpp"
#include "rfl/parsing/Parent.hpp"

namespace rfl::msgpack {

template <class T>
std::string write(const T& obj) {
  using ParentType = parsing::Parent<Writer>;
  auto w = Writer();
  auto res = Parser<T>::write(w, obj, ParentType::Root{});
  return res;
}

}  // namespace rfl::msgpack

#endif
