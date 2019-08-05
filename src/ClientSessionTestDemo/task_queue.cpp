#include "task_queue.h"


//task
task::task()
{
	for (int i = 0; i < 2048; i++)
	{
		data[i] = 0;
	}	
}

task::~task()
{

}


// task queue
task_queue::task_queue()
{


}
task_queue::~task_queue()
{


}
void task_queue::add(const task& task)
{
	boost::unique_lock<boost::mutex> lock(tasks_mutex);//不允许其他线程执行 
	tasks.push_back(task);
	lock.unlock();
	cv.notify_one();//通知其他线程继续
}
std::tuple<bool, task> task_queue::get_nonblock()
{
	boost::lock_guard<boost::mutex> lock(tasks_mutex);
	std::tuple<bool, task> ret;
	if (!tasks.empty())
	{
		ret = std::make_tuple(true, tasks.front());
		tasks.pop_front();
	}
	else
	{
		task tmp;
		ret = std::make_tuple(false, tmp);
	}
	return ret;
}

task task_queue::get_block()
{
	boost::unique_lock<boost::mutex> lock(tasks_mutex);
	while (tasks.empty())
	{
		cv.wait(lock);
	}
	task ret = tasks.front();
	tasks.pop_front();

	return ret;
}