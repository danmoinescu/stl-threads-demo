#include "stl-threads-demo.hpp"


int main()
{
    const int num_work_units = 499; //TODO: take from argv

    std::mutex global_result_lock;
    std::vector<long> global_results;
    global_results.reserve(num_work_units);

    Dispatcher dispatcher = {
        num_work_units
    };
    std::thread dispatcher_thread([&dispatcher] { dispatcher.run(); });

    // Spin lock. In C++ 20 we can use wait instead
    while(!dispatcher.is_ready()) {}

#define NUM_WORKERS 4
    std::vector<std::thread> workers;
    workers.reserve(NUM_WORKERS);
    for(int wid=1; wid <= NUM_WORKERS; ++wid)
    {
        workers.emplace_back([&, wid]
            {
                Worker worker = {
                    wid,
                    dispatcher,
                    global_results,
                    global_result_lock,
                };
                worker.run();
            });
    }

    for(auto &worker: workers)
    {
        worker.join();
    }
    dispatcher_thread.join();
    printf("Main ending\n");
    return 0;
}
