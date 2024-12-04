#include "stl-threads-demo.hpp"


int main()
{
    const int num_work_units = 499; //TODO: take from argv

    Dispatcher dispatcher = {
        num_work_units
    };
    std::thread dispatcher_thread([&dispatcher] { dispatcher.run(); });

    // Spin lock. In C++ 20 we can use wait instead
    while(!dispatcher.is_ready()) {}

    Synchronized<std::vector<long>> global_results;
    global_results->reserve(num_work_units);
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
                };
                worker.run();
            });
    }

    for(auto &worker: workers)
    {
        worker.join();
    }
    dispatcher_thread.join();

    auto global_results_instance = global_results.get_instance();
    printf("Got %ld result(s):", global_results->size());
    for(auto result_iter = global_results_instance->cbegin();
            result_iter != global_results_instance->cend(); result_iter++)
    {
        printf(" %ld", *result_iter);
    }
    printf("\n");
    return 0;
}
