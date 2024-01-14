#include <rfl/msgpack/write.hpp>

int main() {
  const auto s = rfl::msgpack::write(std::string("123"));
  std::cout << s.length() << std::endl;
  for (auto c : s) {
    printf("%02x ", c);
  }
}
