#ifndef CONFIG_H
#define CONFIG_H 1

#include <cstdint>
#include "engines/lynx.hpp"

//using TuneEval = Toy::ToyEval;
//using TuneEval = Toy::ToyEvalTapered;
//using TuneEval = Fourku::FourkuEval;
using TuneEval = Lynx;
constexpr int32_t data_load_thread_count = 16;
constexpr int32_t thread_count = 32;
constexpr bool print_data_entries = false;
constexpr bool print_eval = false;
constexpr bool complete_step_output = true;
constexpr int32_t data_load_print_interval = 10000;
constexpr int32_t print_step_interval = 1000;

#endif // !CONFIG_H
