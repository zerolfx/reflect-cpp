#ifndef RFL_MSGPACK_WRITER_HPP_
#define RFL_MSGPACK_WRITER_HPP_

#include <msgpack.h>

#include <string>

#define RFL_MSGPACK_PACK_VAL_FUNC(type, name) \
  void add_value(const type& var) noexcept {  \
    msgpack_pack_##name(&packer, var);        \
  }

namespace rfl::msgpack {

struct Packer {
  msgpack_packer packer{};
  msgpack_sbuffer buffer{};

  [[nodiscard]] std::string to_string() const noexcept {
    return {buffer.data, buffer.size};
  }

  Packer() {
    msgpack_sbuffer_init(&buffer);
    msgpack_packer_init(&packer, &buffer, msgpack_sbuffer_write);
  }

  ~Packer() { msgpack_sbuffer_destroy(&buffer); }

  RFL_MSGPACK_PACK_VAL_FUNC(uint8_t, uint8)
  RFL_MSGPACK_PACK_VAL_FUNC(uint16_t, uint16)
  RFL_MSGPACK_PACK_VAL_FUNC(uint32_t, uint32)
  RFL_MSGPACK_PACK_VAL_FUNC(uint64_t, uint64)
  RFL_MSGPACK_PACK_VAL_FUNC(int8_t, int8)
  RFL_MSGPACK_PACK_VAL_FUNC(int16_t, int16)
  RFL_MSGPACK_PACK_VAL_FUNC(int32_t, int32)
  RFL_MSGPACK_PACK_VAL_FUNC(int64_t, int64)
  RFL_MSGPACK_PACK_VAL_FUNC(float, float)
  RFL_MSGPACK_PACK_VAL_FUNC(double, double)

  void add_value(const bool& var) noexcept {
    if (var) {
      msgpack_pack_true(&packer);
    } else {
      msgpack_pack_false(&packer);
    }
  }

  void add_value(const std::string& var) noexcept {
    msgpack_pack_str(&packer, var.size());
    msgpack_pack_str_body(&packer, var.c_str(), var.size());
  }
};

struct Writer {
  using OutputArrayType = Packer;
  using OutputObjectType = Packer;
  using OutputVarType = Packer;

  static Packer array_as_root(const size_t size) noexcept {
    Packer parker;
    msgpack_pack_array(&parker.packer, size);
    return parker;
  }

  static Packer object_as_root(const size_t size) noexcept {
    Packer parker;
    msgpack_pack_map(&parker.packer, size);
    return parker;
  }

  static Packer null_as_root() noexcept {
    Packer parker;
    msgpack_pack_nil(&parker.packer);
    return parker;
  }

  template <typename T>
  static Packer value_as_root(const T& var) noexcept {
    Packer parker;
    parker.add_value(var);
    return parker;
  }

  static Packer add_array_to_array(const size_t size, Packer* array) noexcept {
    msgpack_pack_array(&array->packer, size);
    return *array;
  }

  static Packer add_object_to_array(const size_t size, Packer* array) noexcept {
    msgpack_pack_map(&array->packer, size);
    return *array;
  }

  static Packer add_array_to_object(const std::string& name, const size_t size,
                                    Packer* object) noexcept {
    object->add_value(name);
    msgpack_pack_array(&object->packer, size);
    return *object;
  }

  static Packer add_object_to_object(const std::string& name, const size_t size,
                                     Packer* object) noexcept {
    object->add_value(name);
    msgpack_pack_map(&object->packer, size);
    return *object;
  }

  template <typename T>
  static Packer add_value_to_array(const T& var, Packer* array) noexcept {
    array->add_value(var);
    return *array;
  }

  template <typename T>
  static Packer add_value_to_object(const std::string& name, const T& var,
                                    Packer* object) noexcept {
    object->add_value(name);
    object->add_value(var);
    return *object;
  }

  static Packer add_null_to_array(Packer* array) noexcept {
    msgpack_pack_nil(&array->packer);
    return *array;
  }

  static Packer add_null_to_object(const std::string& name,
                                   Packer* object) noexcept {
    object->add_value(name);
    msgpack_pack_nil(&object->packer);
    return *object;
  }

  static void end_array(Packer*) noexcept {}

  static void end_object(Packer*) noexcept {}
};

}  // namespace rfl::msgpack

#endif
