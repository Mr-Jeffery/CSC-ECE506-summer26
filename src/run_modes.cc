/// @file run_modes.cc
/// @brief Implementation of the three modes of operation: Single Metrics, Batch Metrics, and Interactive

#include <barrier>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>

#include "main.h"
#include "memory_system.h"
#include "interactive_mode_coherence.h"
#include "interactive_mode_replacer.h"

/// @brief The number of traces to buffer at a time
#define N_TRACE_BUF 1000000

/// @brief Parse a single text trace line "<proc>\t<r|w>\t<hexaddr>" into a packed record
/// @param line The line to parse (must be non-empty)
/// @param rec Output record, packed as { op = (proc << 1) | is_write, addr }
/// @return True if the line parsed successfully
static bool parseTraceLine(const std::string& line, trace_t& rec) {
    const char* p = line.c_str();
    char* end;
    long proc = std::strtol(p, &end, 10);
    if (end == p) return false;
    while (*end == '\t' || *end == ' ') end++;
    char op = *end++;
    if (op != 'r' && op != 'w') return false;
    while (*end == '\t' || *end == ' ') end++;
    const char* addr_start = end;
    unsigned long addr = std::strtoul(addr_start, &end, 16);
    if (end == addr_start) return false;
    rec.op = (uint8_t)(((proc & 0x7F) << 1) | (op == 'w' ? 1 : 0));
    rec.addr = (addr_t)addr;
    return true;
}

/// @brief Read and parse up to 'max' text traces from a stream into 'buf'
/// @param in The trace stream
/// @param buf Output buffer of at least 'max' records
/// @param max The maximum number of records to read
/// @return The number of records parsed (0 signals end of input)
static uint32_t readTextTraces(std::istream& in, trace_t* buf, uint32_t max) {
    uint32_t count = 0;
    std::string line;
    while (count < max && std::getline(in, line)) {
        if (line.empty()) continue;
        if (parseTraceLine(line, buf[count])) count++;
    }
    return count;
}

void runBatchMetrics(int argc, char* argv[]) {
    // Configurations vector
    std::vector<cache_config> configs;
    readConfigurations(configs, argv[ARG_CONFIG]);

    // Get trace file, trace limit, and trace file buffers
    std::ifstream trace_file;
    size_t trace_limit = getTrace(argc, argv, trace_file, ARG_M_COUNT);

    // The trace file chunks will be "double buffered" to allow for simultaneous reading and processing
    trace_t* trace_swap = new trace_t[N_TRACE_BUF];
    trace_t* trace_buf = new trace_t[N_TRACE_BUF];
    uint32_t n_records;        // parsed records available in trace_buf
    uint32_t next_records = 0; // parsed records read ahead into trace_swap

    // Setup synchronization objects
    auto sync_point_task = [&]() {
        // Switch to the new chunk
        std::swap(trace_buf, trace_swap);
        n_records = next_records;
        };
    std::barrier sync_point(configs.size() + 1, sync_point_task);
    std::mutex stats_print_mutex;

    // Set up worker threads
    auto batch_metrics_task = [&](cache_config config) {
        // Create memory system
        MemorySystem* memory_system = (*directory_map)[config.directory](config);

        // Process each block as it arrives
        size_t line_count = 0;
        while (n_records) {
            // Execute traces in current block
            for (uint32_t i = 0; i < n_records; i++) {
                uint8_t op = trace_buf[i].op;
                addr_t addr = trace_buf[i].addr;
                if (op & 1) memory_system->issuePrWr(addr, op >> 1
#ifdef WRITE_TIMESTAMP
                    , line_count
#endif
                );
                else memory_system->issuePrRd(addr, op >> 1
#ifdef WRITE_TIMESTAMP
                    , line_count
#endif
                );
                line_count++;

                // Exit the while loop if trace limit is reached
                if (trace_limit && line_count == trace_limit) goto print_stats;
            }

            // Wait for the next block to be read in
            sync_point.arrive_and_wait();
        }

        // Print statistics (Note the mutex is automatically released as part of the implicit destructor call)
    print_stats:
        std::lock_guard guard(stats_print_mutex);
        memory_system->printStats();
        };
    std::vector<std::thread> workers;
    workers.reserve(configs.size());

    // Read first chunk
    n_records = readTextTraces(trace_file, trace_buf, N_TRACE_BUF);

    // Start each worker thread
    printStatsHeader(); // Ensure CSV header prints first
    for (cache_config& config : configs)
        workers.emplace_back(batch_metrics_task, config);

    // Read each subsequent chunk while the worker threads process the current one
    size_t line_count = n_records;
    while (n_records && !(trace_limit && line_count >= trace_limit)) {
        next_records = readTextTraces(trace_file, trace_swap, N_TRACE_BUF);
        sync_point.arrive_and_wait();
        line_count += n_records;
    }

    // Wait for worker threads
    for (std::thread& worker : workers)
        worker.join();

    // Cleanup
    delete[] trace_buf;
    delete[] trace_swap;
}

void runSingleMetrics(int argc, char* argv[]) {
    // Get configuration
    cache_config config = { 0 };
    getConfig(argc, argv, config);

    // Get trace file and limit
    std::ifstream trace_file;
    size_t trace_limit = getTrace(argc, argv, trace_file, ARG_S_COUNT);

    // Create memory system
    MemorySystem* memory_system = (*directory_map)[config.directory](config);

    // Execute traces
    std::string line;
    trace_t rec;
    size_t line_count = 0;
    while (!(trace_limit && line_count == trace_limit) && std::getline(trace_file, line)) {
        if (line.empty() || !parseTraceLine(line, rec)) continue;
        if (rec.op & 1) memory_system->issuePrWr(rec.addr, rec.op >> 1
#ifdef WRITE_TIMESTAMP
            , line_count
#endif
        );
        else memory_system->issuePrRd(rec.addr, rec.op >> 1
#ifdef WRITE_TIMESTAMP
            , line_count
#endif
        );
        line_count++;
    }

    // Print statistics
    printStatsHeader();
    memory_system->printStats();
}

void runInteractiveMode(char* name_of_showcased) {
    // Get the correct interactive mode class
    InteractiveMode* interactive_mode;
    if (coherence_map->count(name_of_showcased)) interactive_mode = new InteractiveModeCoherence(name_of_showcased);
    else if (replacement_map->count(name_of_showcased)) interactive_mode = new InteractiveModeReplacer(name_of_showcased);
    else {
        std::cerr << ARG_INTERACTIVE << '@' << 0 << ": " << "Couldn't find a coherence protocol or replacement policy with that name!" << std::endl;
        exit(ARG_INTERACTIVE);
    }

    // Setup sigint catch (so that bottom border of table can be printed)
    signal(SIGINT, [](int signum) {
        fclose(stdin);
        std::cout << "\r";
        });

    // Process commands
    std::string line;
    for (uint32_t line_count = 1; std::getline(std::cin, line); line_count++) {
        // Move cursor up one line if the output and input are the terminal
        if (isatty(fileno(stdin)) && isatty(fileno(stdout))) std::cout << "\e[A";

        // Ignore empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        // Evaluate command
        if (!interactive_mode->evalutateCommand(line)) {
            // Bad command
            if (!isatty(fileno(stdin))) std::cerr << "Line " << line_count << ": ";
            interactive_mode->printCmdFormatMessage();
        }
    }

    // Cleanup
    delete interactive_mode;
}
