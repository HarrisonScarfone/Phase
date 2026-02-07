#ifndef BOOK_H
#define BOOK_H

#include <cstdint>
#include <string>

namespace book {

// Initialize the opening book
void init();

// Probe the book for a position
// Returns a move in UCI format (e.g., "e2e4") or empty string if not in book
std::string probe(uint64_t hash);

// Check if book is enabled
bool is_enabled();

// Enable/disable book
void set_enabled(bool enabled);

}  // namespace book

#endif
