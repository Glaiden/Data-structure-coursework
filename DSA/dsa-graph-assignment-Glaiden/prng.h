/**
 * @file prng.h
 * @brief Pseudo Random Number Generator
 */

namespace Utils {
unsigned rand();                // returns a random 32-bit integer
void srand(unsigned, unsigned); // seed the generator
int randInt(int low, int high);  // range
} // namespace Utils
