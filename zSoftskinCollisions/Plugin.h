
// This file added in headers queue
// File: "Headers.h"

namespace GOTHIC_ENGINE {
  bool HasFlag( const int& flags, const int& flag ) {
    return (flags & flag) == flag;
  }
}