#pragma once


namespace common {
namespace thread {

// flag for run a thread
// TODO: move to core 
void setup_run_flag(bool const& run_flag);

// return run flag
bool run();
} // namespace thread
} // namespace common