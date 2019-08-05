#include "CSession.h"
#include <boost/thread.hpp>
#include <iostream>

using namespace std;

//�����̳���enable_shared_from_this��Ķ���ʱ����ʹ������ָ��
typedef boost::shared_ptr<CSession> ClientPtr; 

std::vector<ClientPtr> client_list;//�洢���ɵĿͻ���ʵ��

int main(int argc, char* argv[])
{
	boost::asio::io_context io_context_;
	typedef boost::asio::io_context::executor_type ExecutorType;
	boost::asio::executor_work_guard<ExecutorType> work_guard_(boost::asio::make_work_guard(io_context_));

	std::string IP = "127.0.0.1";
	unsigned short port = 8104;
	int heartbeat_timer_minutes = 1;	// ����ʱ����Ϊ1����

	int clientNum = 10; // ���ӿͻ��˵���Ŀ

	for (int i = 0; i < clientNum; i++)
	{
		boost::shared_ptr<CSession> pClientSession(new CSession(IP, port, heartbeat_timer_minutes,
			io_context_));
		client_list.push_back(std::move(pClientSession));
		client_list.at(i)->start();
	}
	boost::thread thr(boost::bind(&boost::asio::io_service::run, boost::ref(io_context_)));

	//������һ���߳�����
	int thread_count = (std::max)(static_cast<int>(boost::thread::hardware_concurrency()), 1);
	
	boost::thread_group threads_;
	for (int i = 0; i < thread_count; i++)
	{
		threads_.create_thread(boost::bind(&boost::asio::io_service::run, boost::ref(io_context_)));
		boost::this_thread::sleep(boost::posix_time::seconds(5));
	}

	return 0;
}
