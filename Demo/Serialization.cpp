#include <Utils/I0/Serialization.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

struct Character {
    std::uint32_t id;
    std::int32_t health;
    double experience;
    char name[32];
};

static_assert(std::is_trivially_copyable_v<Character>);
int main()
{
    const std::string filename = "/tmp/hw7_serdes.bin";

    Character original{};
    original.id = 42;
    original.health = 100;
    original.experience = 122345.678;

    std::strcpy(original.name, "Mesheek");

    I0::Serialize(original, filename);

    Character restored = I0::Deserialize<Character>(filename);

    assert(std::memcmp(&original, &restored, sizeof(Character)) == 0);

    //Primitive types.
    const std::uint64_t value = 0x123456789ABCDEF0ULL;

    I0::Serialize(value, filename);

    const auto result = I0::Deserialize<std::uint64_t>(filename);

    assert(value == result);
}
