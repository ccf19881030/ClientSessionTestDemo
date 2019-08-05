#pragma once

#include <deque>
#include "type.h"
//#include <boost/thread/thread.hpp>
//#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

//���� ���緢������ʹ�õĽṹ��ͨ����һ�����ͻ�������һ��ʵ��Ҫ���͵ĳ���
class task
{
public:
	task();
	~task();
public:
	unsigned char data[2048];
	unsigned int len;//ʵ�ʷ��ͳ���
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

