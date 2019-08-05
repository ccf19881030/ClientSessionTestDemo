#pragma once

#include <deque>
#include "type.h"
//#include <boost/thread/thread.hpp>
//#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

//任务 网络发送任务使用的结构，通常有一个发送缓冲区和一个实际要发送的长度
class task
{
public:
	task();
	~task();
public:
	unsigned char data[2048];
	unsigned int len;//实际发送长度
};


class task_queue
{
private:
	std::deque<task> tasks;
	boost::mutex tasks_mutex;
	boost::condition_variable cv;
public:
	task_queue();
	~task_queue();


	void add(const task& task);
	std::tuple<bool, task> get_nonblock();
	task get_block();
};

