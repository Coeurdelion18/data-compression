#include "Delta_preprocessing.h"

std::vector<int16_t> delta_encode(const std::vector<int16_t>& input) {
   /*size_t size = input.size();
    if (size == 0) return {};
    
    std::vector<int16_t> delta(size);
    delta[0] = input[0];
    for (size_t i = 1; i < size; ++i) {
        delta[i] = input[i] - input[i - 1];
    }
    return delta;*/
    return input;
}

std::vector<int16_t> double_delta_encode(const std::vector<int16_t>& input) {
    std::vector<int16_t> first_pass = delta_encode(input);
    return delta_encode(first_pass);  // second delta on the result
}

std::vector<int16_t> delta_decode(const std::vector<int16_t>& delta) {
    /*size_t size = delta.size();
    if (size == 0) return {};
    
    std::vector<int16_t> input(size);
    input[0] = delta[0];
    for (size_t i = 1; i < size; ++i) {
        input[i] = delta[i] + input[i - 1];
    }
    return input;
    */
   return delta;
}
