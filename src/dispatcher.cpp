#include "stl-threads-demo.hpp"

template<typename W>
void Dispatcher<W>::run()
{
    std::unique_lock<std::mutex> work_sync_lock(work_sync_mutex);

    // Notify any interested threads that we're ready
    start_signal.store(true);
    start_signal.notify_all();

    while(true)
    {
        work_sync_cond.wait(work_sync_lock);
        if(!fill_work_queue())
        {
            _is_all_work_done = true;
            break;
        }
    }
}


template<typename W>
bool Dispatcher<W>::fill_work_queue()
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


// explicit template instantiations to keep the linker happy
template void Dispatcher<long>::run();
