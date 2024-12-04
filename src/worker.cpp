#include "stl-threads-demo.hpp"


void Worker::run()
{
    while(true)
    {
        std::pair<bool, long> work = get_work();
        if(!work.first)
        {
            break;
        }
        dispatcher.work_sync_cond.notify_one();
        if(local_results.size() == local_results.capacity())
        {
            // local cache is full, save results into global location
            purge_cache();
        }
        auto result = do_work(work.second);
        local_results.push_back(result);
    }
    // save remaining cached results
    purge_cache();
    printf("Worker %d ending\n", id);
}


void Worker::purge_cache()
{
    auto locked_global_results = global_results.get_instance();
    for(auto it = local_results.cbegin(); it != local_results.cend(); it++)
    {
        locked_global_results->insert(
                locked_global_results->begin(),
                *it);
    }
    local_results.clear();
}


std::pair<bool, long> Worker::get_work()
{
    bool is_available = true;
    std::unique_lock<std::mutex> work_sync_lock(dispatcher.work_sync_mutex);
    long work_unit = 0;
    while(true)
    {
        if(dispatcher.work_queue.size() > 0)
        {
            work_unit = dispatcher.work_queue.front();
            dispatcher.work_queue.pop();
            break;
        }
        else if (dispatcher.is_all_work_done())
        {
            is_available = false;
            break;
        }
        else
        {
            /* There is more work, but none is yet available: notify
               the dispatcher and wait for it to put work in the queue.
            */
            dispatcher.work_sync_cond.notify_one();
            dispatcher.work_sync_cond.wait(work_sync_lock);
        }
    }
    return std::make_pair(is_available, work_unit);
}


long Worker::do_work(long work_unit)
{
    std::this_thread::sleep_for(std::chrono::microseconds(
                10 * (4 + work_unit % 3)));
    return work_unit * 1000 + id;
}
