#ifndef RFL_MSGPACK_PARSER_HPP_
#define RFL_MSGPACK_PARSER_HPP_

#include "Reader.hpp"
#include "Writer.hpp"
#include "../parsing/Parser.hpp"

namespace rfl::msgpack {

template <class T>
using Parser = parsing::Parser<Reader, Writer, T>;

}  // namespace rfl

#endif
