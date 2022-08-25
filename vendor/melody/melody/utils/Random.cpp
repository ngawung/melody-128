#include "melody.hpp"
#include <cmath>
#include <chrono>

using namespace melody;

Random::Random() {
    m_w = 521288629;
    m_z = 362436069;
}

void Random::setSeed(uint32_t w) {
    if (w != 0) m_w = w;
}

void Random::setSeed(uint32_t w, uint32_t z) {
    if (w != 0) m_w = w;
    if (z != 0) m_z = z;
}

void Random::setSeedFromTime() {
    m_w = std::chrono::system_clock::now().time_since_epoch().count();
}

uint32_t Random::MWC() {
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;
}

float Random::UNI() {
    return MWC() * inv_uni;
}

float Random::range(uint32_t min, uint32_t max) {
    return min + (max-min) * UNI();
}

float Random::floorRange(uint32_t min, uint32_t max) {
    return std::floor(range(min, max));
}

float Random::xorshift_uni(uint32_t x) {
    return xorshift(x) * inv_uni;
}

float Random::xorshift64_uni(uint64_t x) {
    return xorshift64(x) * inv_uni64;
}

uint32_t Random::xorshift(uint32_t x) {
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

uint64_t Random::xorshift64(uint64_t x) {
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return x;
}