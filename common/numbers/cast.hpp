#pragma once
#include <exception>
#include <type_traits>
#include <utils/string/build.hpp>

namespace common {
namespace numbers {

template <typename Output, typename Input>
inline Output integer_cast(Input input) {

    static_assert(std::is_integral<Output>() && std::is_integral<Input>(),
                  "Input and output must be integers");

    Output output = static_cast<Output>(input);
    if (input != output) { // unlikely
        throw std::runtime_error(utils::build_string("Cannot convert ", std::is_signed<Input>() ? " signed " : " unsigned ",
                                                     sizeof(Input), " bytes long type with value ", input,
                                                     " to ", std::is_signed<Output>() ? " signed " : " unsigned ", sizeof(Output),
                                                     " bytes long type with truncated value ", output));
    }
    return output;
}
} // namespace numbers
} // namespace common