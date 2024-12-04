#include "stl-threads-demo.hpp"
#include <cstdlib>


static void usage(const char *me)
{
    fprintf(stderr, "Usage: %s num_workers work_units\n", me);
}


int main(int argc, const char* const argv[])
{
    if(argc<3)
    {
        usage(argv[0]);
        return 1;
    }
    const int num_workers = atoi(argv[1]);
    const int total_work_units = atoi(argv[2]);
    if(num_workers <= 0 || total_work_units <= 0)
    {
        usage(argv[0]);
        return 1;
    }

    Dispatcher dispatcher = {
        total_work_units
    };
    std::thread dispatcher_thread([&dispatcher] { dispatcher.run(); });

    // Spin lock. In C++ 20 we can use wait instead
    while(!dispatcher.is_ready()) {}

    Synchronized<std::vector<long>> global_results;
    global_results->reserve(total_work_units);
    std::vector<std::thread> workers;
    workers.reserve(num_workers);
    for(int wid=1; wid <= num_workers; ++wid)
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
