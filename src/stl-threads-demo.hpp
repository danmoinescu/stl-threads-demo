#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include "synchronized.hpp"


class Dispatcher
{
    public:
        std::mutex work_sync_mutex;
        std::condition_variable work_sync_cond;
        std::queue<long> work_queue;
    private:
        // In C++ 20 we can use binary_semaphore or latch instead
        std::atomic_bool start_signal;
        bool _is_all_work_done = false;
        long total_work_units;

    public:
        Dispatcher(long total_work_units) :
            start_signal(false),
            total_work_units(total_work_units)
        {}

        void run();
        bool is_all_work_done() { return _is_all_work_done; }
        bool is_ready() { return start_signal.load(); }


    private:
        bool is_work_available() { return total_work_units>0; }
        long next_available_work() { return total_work_units--; }
        bool fill_work_queue();
};


class Worker
{
    private:
        const int id;
        Dispatcher &dispatcher;
        Synchronized<std::vector<long>> &global_results;
        std::vector<long> local_results;

    public:
        Worker(
                int id,
                Dispatcher &dispatcher,
                Synchronized<std::vector<long>> &global_results) :
            id(id),
            dispatcher(dispatcher),
            global_results(global_results)
        {
#define MAX_LOCAL_RESULTS 10 // Arbitrary value for demo purposes
            local_results.reserve(MAX_LOCAL_RESULTS);
        }

        void run();

    private:
        std::pair<bool, long> get_work();
        long do_work(long work_unit);
        void purge_cache();
};
