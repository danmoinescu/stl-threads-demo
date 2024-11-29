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
        do_work(work.second);
    }
    printf("Worker %d ending\n", id);
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
    printf("Worker %d got %ld\n", id, work_unit);
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    return work_unit * 1000 + id;
}
