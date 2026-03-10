#include <benchmark/benchmark.h>
#include "EnigmaMachine.hpp"
#include "Rotor.hpp"
#include "Reflector.hpp"
#include "PlugBoard.hpp"
#include <memory>
#include <vector>
#include <random>
#include <atomic>
#include <cstdlib>

// --- Global Memory Tracker ---
std::atomic<size_t> g_current_usage{0};
std::atomic<size_t> g_peak_usage{0};
std::atomic<size_t> g_alloc_count{0};

void* operator new(std::size_t size) {
    g_alloc_count++;
    g_current_usage += size;
    if (g_current_usage > g_peak_usage) g_peak_usage = g_current_usage.load();
    
    // Store size at the beginning for delete
    void* p = std::malloc(size + sizeof(size_t));
    if (!p) throw std::bad_alloc();
    *(size_t*)p = size;
    return (char*)p + sizeof(size_t);
}

void operator delete(void* p) noexcept {
    if (!p) return;
    size_t* size_p = (size_t*)((char*)p - sizeof(size_t));
    g_current_usage -= *size_p;
    std::free(size_p);
}

void operator delete(void* p, std::size_t) noexcept {
    ::operator delete(p);
}

// Helper to get asset path - assumes running from build directory
const std::string ASSETS_DIR = "./assets/";
const std::string CONFIG_FILE = ASSETS_DIR + "EnigmaMachineConfig1.toml";

/**
 * @brief Subclass used ONLY for benchmarking to access internal constructors.
 */
class BenchmarkingEnigmaMachine : public EnigmaMachine {
public:
    explicit BenchmarkingEnigmaMachine(const EnigmaMachineConfig& config) 
        : EnigmaMachine(config) {}
};

constexpr int KB = 1024;

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

static void BM_EnigmaMachine_Memory_Snapshot(benchmark::State& state) {
    g_peak_usage = 0;
    g_current_usage = 0;
    
    for (auto _ : state) {
        try {
            EnigmaMachine machine(CONFIG_FILE, ASSETS_DIR);
            benchmark::DoNotOptimize(machine);
        } catch (...) {
            state.SkipWithError("Failed to initialize EnigmaMachine");
        }
    }
    state.counters["PeakHeapBytes"] = (double)g_peak_usage;
}
BENCHMARK(BM_EnigmaMachine_Memory_Snapshot)->Unit(benchmark::kMillisecond);

static void BM_EnigmaMachine_Memory_HotPath(benchmark::State& state) {
    EnigmaMachine machine(CONFIG_FILE, ASSETS_DIR);
    int message_length = 10 * 1000 * 1000; // 10M chars
    
    std::vector<int> message(message_length, 0);
    
    for (auto _ : state) {
        size_t start_allocs = g_alloc_count.load();
        for (int i = 0; i < message_length; ++i) {
            int out = machine.keyTransform(message[i]);
            benchmark::DoNotOptimize(out);
        }
        size_t end_allocs = g_alloc_count.load();
        state.counters["HotPathAllocations"] = (double)(end_allocs - start_allocs);
    }
}
BENCHMARK(BM_EnigmaMachine_Memory_HotPath)->Unit(benchmark::kMillisecond);

class StackSpy : public IEnigmaObserver {
public:
    size_t deepest_stack = 0;
    void onRotorStepped(int, int) override {
        volatile char p;
        deepest_stack = (size_t)&p;
    }
    void onCharEncrypted(char, char) override {}
};

static void BM_EnigmaMachine_StackDepth(benchmark::State& state) {
    EnigmaMachine machine(CONFIG_FILE, ASSETS_DIR);
    StackSpy spy;
    machine.registerObserver(&spy);
    
    volatile char base_ptr;
    
    for (auto _ : state) {
        int out = machine.keyTransform(0);
        benchmark::DoNotOptimize(out);
        
        if (spy.deepest_stack != 0) {
            state.counters["StackDepthBytes"] = (double)((size_t)&base_ptr - spy.deepest_stack);
        }
    }
}
BENCHMARK(BM_EnigmaMachine_StackDepth);

static void BM_EnigmaMachine_Throughput_Scaling(benchmark::State& state) {
    int nRotors = state.range(0);
    int message_length = KB;

    EnigmaMachineConfig config;
    config.rotorCount = nRotors;
    config.rotorPositions.resize(nRotors, 0);
    config.rotors.resize(nRotors);
    
    // Shared wiring for all rotors for simplicity
    std::array<int, TRANSFORMER_SIZE> wiring = {{1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24}};
    for(int i = 0; i < nRotors; ++i) {
        config.rotors[i].wiring = wiring;
        config.rotors[i].notchPosition = 16;
    }
    
    config.reflector.wiring = {{25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}};
    
    BenchmarkingEnigmaMachine machine(config);
    
    std::vector<int> message(message_length);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, TRANSFORMER_SIZE - 1);
    for (int i = 0; i < message_length; ++i) {
        message[i] = distribution(generator);
    }

    for (auto _ : state) {
        for (int i = 0; i < message_length; ++i) {
            int out = machine.keyTransform(message[i]);
            benchmark::DoNotOptimize(out);
        }
    }
    state.SetBytesProcessed(state.iterations() * message_length);
}
BENCHMARK(BM_EnigmaMachine_Throughput_Scaling)->Arg(3)->Arg(4)->Arg(5)->Arg(8);

static void BM_EnigmaMachine_PlugBoard_Scaling(benchmark::State& state) {
    int nSwaps = state.range(0);
    int message_length = KB;

    EnigmaMachineConfig config;
    config.rotorCount = 3;
    config.rotorPositions = {0, 0, 0};
    config.rotors.resize(3);
    
    std::array<int, TRANSFORMER_SIZE> wiring = {{1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24}};
    for(int i = 0; i < 3; ++i) {
        config.rotors[i].wiring = wiring;
        config.rotors[i].notchPosition = 16;
    }
    config.reflector.wiring = {{25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}};
    
    // Fill with nSwaps
    for(int i = 0; i < nSwaps; ++i) {
        config.plugBoardPairs[i] = {i*2, i*2 + 1};
    }
    
    BenchmarkingEnigmaMachine machine(config);
    
    std::vector<int> message(message_length);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, TRANSFORMER_SIZE - 1);
    for (int i = 0; i < message_length; ++i) {
        message[i] = distribution(generator);
    }

    for (auto _ : state) {
        for (int i = 0; i < message_length; ++i) {
            int out = machine.keyTransform(message[i]);
            benchmark::DoNotOptimize(out);
        }
    }
    state.SetBytesProcessed(state.iterations() * message_length);
}
BENCHMARK(BM_EnigmaMachine_PlugBoard_Scaling)->Arg(0)->Arg(PLUGBOARD_MAX_PAIRS);

static void BM_EnigmaMachine_KeyTransform(benchmark::State& state) {
    EnigmaMachine machine(CONFIG_FILE, ASSETS_DIR);
    int message_length = state.range(0);
    
    // Pre-generate random message
    std::vector<int> message(message_length);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, TRANSFORMER_SIZE - 1);
    for (int i = 0; i < message_length; ++i) {
        message[i] = distribution(generator);
    }

    for (auto _ : state) {
        for (int i = 0; i < message_length; ++i) {
            int out = machine.keyTransform(message[i]);
            benchmark::DoNotOptimize(out);
        }
    }
    state.SetBytesProcessed(state.iterations() * message_length);
}
BENCHMARK(BM_EnigmaMachine_KeyTransform)->Range(KB, 128 * KB);

static void BM_Rotor_Transform(benchmark::State& state) {
    // Manually create a rotor config for benchmarking
    RotorConfig config;
    config.wiring = {{1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24}};
    config.notchPosition = 16;
    
    Rotor rotor(config);
    rotor.setPosition(0);
    
    int input = 0;
    for (auto _ : state) {
        int out = rotor.transform(input);
        benchmark::DoNotOptimize(out);
        input = (input + 1) % TRANSFORMER_SIZE;
    }
}
BENCHMARK(BM_Rotor_Transform);

static void BM_Rotor_Rotate(benchmark::State& state) {
    RotorConfig config;
    config.wiring = {{1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24}};
    config.notchPosition = 16;
    Rotor rotor(config);

    for (auto _ : state) {
        int stepped = rotor.rotate();
        benchmark::DoNotOptimize(stepped);
    }
}
BENCHMARK(BM_Rotor_Rotate);

static void BM_Reflector_Transform(benchmark::State& state) {
    ReflectorConfig config;
    config.wiring = {{25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}};
    Reflector reflector(config);

    int input = 0;
    for (auto _ : state) {
        int out = reflector.transform(input);
        benchmark::DoNotOptimize(out);
        input = (input + 1) % TRANSFORMER_SIZE;
    }
}
BENCHMARK(BM_Reflector_Transform);

static void BM_PlugBoard_Swap(benchmark::State& state) {
    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs;
    for(int i=0; i < PLUGBOARD_MAX_PAIRS; ++i) {
        pairs[i] = {i*2, i*2 + 1};
    }
    
    PlugBoard pb(pairs);
    
    int input = 0;
    for (auto _ : state) {
        int out = pb.swap(input);
        benchmark::DoNotOptimize(out);
        input = (input + 1) % TRANSFORMER_SIZE;
    }
}
BENCHMARK(BM_PlugBoard_Swap);

BENCHMARK_MAIN();
