// delta.h
#ifndef DELTA_H
#define DELTA_H

#include <vector>
#include <cstdint>

std::vector<int16_t> delta_encode(const std::vector<int16_t>& input);
std::vector<int16_t> double_delta_encode(const std::vector<int16_t>& input);
std::vector<int16_t> delta_decode(const std::vector<int16_t>& delta);
#endif // DELTA_H
