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


template <typename Work>
class Dispatcher
{
    public:
        std::mutex work_sync_mutex;
        std::condition_variable work_sync_cond;
        std::queue<Work> work_queue;
    private:
        // We can also use binary_semaphore or latch instead
        std::atomic_bool start_signal;
        bool _is_all_work_done = false;
        int total_work_units;

    public:
        Dispatcher(int total_work_units) :
            start_signal(false),
            total_work_units(total_work_units)
        {}

        void run();
        bool is_all_work_done() { return _is_all_work_done; }
        void wait_until_ready() { start_signal.wait(false); }


    private:
        bool is_work_available() { return total_work_units>0; }
        Work next_available_work() { return Work(total_work_units--); }
        bool fill_work_queue();
};


template <typename Work, typename Result>
class Worker
{
    private:
        const int id;
        Dispatcher<Work> &dispatcher;
        Synchronized<std::vector<Result>> &global_results;
        std::vector<Result> local_results;

    public:
        Worker(
                int id,
                Dispatcher<Work> &dispatcher,
                Synchronized<std::vector<Result>> &global_results) :
            id(id),
            dispatcher(dispatcher),
            global_results(global_results)
        {
#define MAX_LOCAL_RESULTS 10 // Arbitrary value for demo purposes
            local_results.reserve(MAX_LOCAL_RESULTS);
        }

        void run();

    private:
        std::pair<bool, Work> get_work();
        Result do_work(Work &work_unit);
        void purge_cache();
};
