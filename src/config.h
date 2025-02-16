#ifndef CONFIG_H
#define CONFIG_H 1

#include <cstdint>
#include "engines/fourku.h"
#include "engines/toy.h"
#include "engines/toy_tapered.h"
#include "engines/lynx.hpp"

#define TAPERED 1
//using TuneEval = Toy::ToyEval;
//using TuneEval = Toy::ToyEvalTapered;
//using TuneEval = Fourku::FourkuEval;
using TuneEval = Lynx;
constexpr int32_t data_load_thread_count = 8;
constexpr int32_t thread_count = 16;
constexpr tune_t preferred_k = 0;
constexpr int32_t max_epoch = 5001;
constexpr bool retune_from_zero = true;
constexpr bool enable_qsearch = false;
constexpr bool filter_in_check = false;
constexpr tune_t initial_learning_rate = 1;
constexpr int32_t learning_rate_drop_interval = 10000;
constexpr tune_t learning_rate_drop_ratio = 1;
constexpr bool print_data_entries = false;
constexpr bool print_eval = false;

constexpr bool parse_static_eval = true;
constexpr int32_t wdl_percentage = 0;
constexpr bool use_scaled_static_eval = true;

constexpr bool complete_step_ouput = true;
constexpr int32_t data_load_print_interval = 10000;
constexpr int32_t print_step_interval = 1000;

#endif // !CONFIG_H
