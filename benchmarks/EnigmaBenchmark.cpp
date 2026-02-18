#include <benchmark/benchmark.h>
#include "EnigmaMachine.hpp"
#include "Rotor.hpp"
#include "PlugBoard.hpp"
#include <memory>
#include <vector>
#include <random>

// Helper to get asset path - assumes running from build directory
// where assets have been copied to the local assets/ folder.
const std::string ASSETS_DIR = "./assets/";
const std::string CONFIG_FILE = ASSETS_DIR + "EnigmaMachineConfig1.toml";

static void BM_EnigmaMachine_Initialization(benchmark::State& state) {
    for (auto _ : state) {
        try {
            EnigmaMachine machine(CONFIG_FILE, ASSETS_DIR);
            benchmark::DoNotOptimize(machine);
        } catch (...) {
            state.SkipWithError("Failed to initialize EnigmaMachine");
        }
    }
}
BENCHMARK(BM_EnigmaMachine_Initialization);

static void BM_EnigmaMachine_KeyTransform(benchmark::State& state) {
    EnigmaMachine machine(CONFIG_FILE, ASSETS_DIR);
    int message_length = state.range(0);
    
    // Pre-generate random message
    std::vector<int> message(message_length);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 25);
    for (int i = 0; i < message_length; ++i) {
        message[i] = distribution(generator);
    }

    for (auto _ : state) {
        for (int i = 0; i < message_length; ++i) {
            int out = machine.keyTransform(message[i]);
            benchmark::DoNotOptimize(out);
        }
    }
    state.SetItemsProcessed(state.iterations() * message_length);
}
BENCHMARK(BM_EnigmaMachine_KeyTransform)->Range(1024, 1024 * 64);

static void BM_Rotor_Transform(benchmark::State& state) {
    // Manually create a rotor config for benchmarking
    RotorConfig config;
    config.wiring = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
    config.notchPosition = 16;
    
    Rotor rotor(config);
    rotor.setPosition(0);
    
    int input = 0;
    for (auto _ : state) {
        int out = rotor.transform(input);
        benchmark::DoNotOptimize(out);
        input = (input + 1) % 26;
    }
}
BENCHMARK(BM_Rotor_Transform);

static void BM_PlugBoard_Swap(benchmark::State& state) {
    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs;
    for(int i=0; i<10; ++i) {
        pairs[i] = {i*2, i*2 + 1};
    }
    
    PlugBoard pb(pairs);
    
    int input = 0;
    for (auto _ : state) {
        int out = pb.swap(input);
        benchmark::DoNotOptimize(out);
        input = (input + 1) % 26;
    }
}
BENCHMARK(BM_PlugBoard_Swap);

BENCHMARK_MAIN();
