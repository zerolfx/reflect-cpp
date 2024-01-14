#ifndef RFL_MSGPACK_READER_HPP_
#define RFL_MSGPACK_READER_HPP_
#include <msgpack.h>

#include <rfl/Result.hpp>
#include <rfl/always_false.hpp>

namespace rfl::msgpack {

struct Reader {
  struct Unpacker {
    msgpack_unpacker unparker{};
    Unpacker() {
      msgpack_unpacker_init(&unparker, MSGPACK_UNPACKER_INIT_BUFFER_SIZE);
    }
    ~Unpacker() { msgpack_unpacker_destroy(&unparker); }
  };

  using InputArrayType = msgpack_object_array;
  using InputObjectType = msgpack_object_map;
  using InputVarType = msgpack_object;

  static Result<InputVarType> get_field(const std::string& name,
                                        const InputObjectType& obj) noexcept {
    const auto size = obj.size;
    for (auto i = 0; i < size; ++i) {
      const auto key = obj.ptr[i].key;
      if (key.type != MSGPACK_OBJECT_STR) {
        return Error("Map contains non-string key.");
      }
      if (key.via.str.size == name.size() &&
          std::memcmp(key.via.str.ptr, name.data(), name.size()) == 0) {
        return obj.ptr[i].val;
      }
    }
    return Error("Object contains no field named '" + name + "'.");
  }

  static bool is_empty(const InputVarType& var) noexcept {
    return var.type == MSGPACK_OBJECT_NIL;
  }

  template <typename T>
  static Result<T> to_basic_type(const InputVarType& var) noexcept {
    using decayed_t = std::decay_t<T>;
    if constexpr (std::is_same_v<decayed_t, bool>) {
      if (var.type != MSGPACK_OBJECT_BOOLEAN) {
        return Error("Type mismatch, expected boolean.");
      }
      return var.via.boolean;
    } else if (std::is_same_v<T, std::string>) {
      if (var.type != MSGPACK_OBJECT_STR) {
        return Error("Type mismatch, expected string.");
      }
      return std::string(var.via.str.ptr, var.via.str.size);
    } else if (std::is_floating_point_v<decayed_t>) {
      if (var.type != MSGPACK_OBJECT_FLOAT) {
        return Error("Type mismatch, expected floating point.");
      }
      return var.via.f64;
    } else if (std::is_signed_v<decayed_t>) {
      if (var.type != MSGPACK_OBJECT_POSITIVE_INTEGER &&
          var.type != MSGPACK_OBJECT_NEGATIVE_INTEGER) {
        return Error("Type mismatch, expected signed integer.");
      }
      return var.via.i64;
    } else if (std::is_unsigned_v<decayed_t>) {
      if (var.type != MSGPACK_OBJECT_POSITIVE_INTEGER) {
        return Error("Type mismatch, expected unsigned integer.");
      }
      return var.via.u64;
    }
    static_assert(always_false_v<T>, "Unsupported type.");
  }

  template <size_t size, class Func>
  std::array<std::optional<InputVarType>, size> to_fields_array(
      const Func& get_idx_func, const InputObjectType& obj) const noexcept {
    std::array<std::optional<InputVarType>, size> f_arr;
    for (auto i = 0; i < obj.size; ++i) {
      if (obj.ptr[i].key.type != MSGPACK_OBJECT_STR) {
        continue;
      }
      const auto key =
          std::string(obj.ptr[i].key.via.str.ptr, obj.ptr[i].key.via.str.size);
      auto idx = get_idx_func(key);
      if (idx != -1) {
        f_arr[idx] = obj.ptr[i].val;
      }
    }
    return f_arr;
  }

  static Result<InputArrayType> to_array(const InputVarType& var) noexcept {
    if (var.type != MSGPACK_OBJECT_ARRAY) {
      return Error("Type mismatch, expected array.");
    }
    return var.via.array;
  }

  static Result<InputObjectType> to_object(const InputVarType& var) noexcept {
    if (var.type != MSGPACK_OBJECT_MAP) {
      return Error("Type mismatch, expected map.");
    }
    return var.via.map;
  }

  static std::vector<std::pair<std::string, InputVarType>> to_map(
      const InputObjectType& obj) noexcept {
    std::vector<std::pair<std::string, InputVarType>> m;
    for (auto i = 0; i < obj.size; ++i) {
      const auto key = obj.ptr[i].key;
      if (key.type != MSGPACK_OBJECT_STR) {
        // TODO: failable
      }
      const auto val = obj.ptr[i].val;
      m.emplace_back(std::string(key.via.str.ptr, key.via.str.size), val);
    }
    return m;
  }

  static std::vector<InputVarType> to_vec(const InputArrayType& arr) noexcept {
    std::vector<InputVarType> vec;
    for (auto i = 0; i < arr.size; ++i) {
      vec.push_back(arr.ptr[i]);
    }
    return vec;
  }

  // TODO
  template <class T>
  static constexpr bool has_custom_constructor = false;

  template <class T>
  static Result<T> use_custom_constructor(const InputVarType _var) noexcept {
    return Error("TODO");
  }
};

}  // namespace rfl::msgpack

#endif
