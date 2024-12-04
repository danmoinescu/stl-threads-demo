#include "stl-threads-demo.hpp"


void Dispatcher::run()
{
    std::unique_lock<std::mutex> work_sync_lock(work_sync_mutex);

    /* Notify the main thread that we're ready (in C++ 20 we can add
       a call to notify_one here).
    */
    start_signal.store(true);

    while(true)
    {
        work_sync_cond.wait(work_sync_lock);
        if(!fill_work_queue())
        {
            _is_all_work_done = true;
            break;
        }
    }
    printf("Dispatcher done\n");
}


bool Dispatcher::fill_work_queue()
{
    bool have_more_work = true;
#define MAX_WORK_QUEUE_SIZE 10 // Arbitrary value for demo purposes
    while(work_queue.size() < MAX_WORK_QUEUE_SIZE)
    {
        if(is_work_available())
        {
            work_queue.push(next_available_work());
        }
        else
        {
            have_more_work = false;
            break;
        }
    }
    return have_more_work;
}
