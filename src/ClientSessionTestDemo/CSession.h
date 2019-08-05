#pragma once
#include <boost/asio.hpp> 
#include <boost/array.hpp> 
#include <boost/bind.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <queue>
#include "task_queue.h"

//typedef std::function<void()> taskFun;
//typedef std::queue<taskFun> task;

class CSession :
	public boost::enable_shared_from_this<CSession>
{
public:
	CSession(const std::string IP, unsigned short port, int m_heartbeat_timer_minutes, 
				boost::asio::io_service& io_service_);
	~CSession(void);

private:
	
	void receive_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);//�������ݴ�����
	void connect_handler(const boost::system::error_code &ec); //���Ӵ�����
	void heartbeat_handler(const boost::system::error_code &ec); //����������
	void login_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);//��¼����Ĵ���
	void start_receive();//��ʼ��������
	void send_handler(const boost::system::error_code &ec);
	std::size_t check_frame(const boost::system::error_code &ec, std::size_t bytes_transferred);//У������
	void parse_frame(const boost::system::error_code &ec, std::size_t bytes_transferred);//��������
public:
	void start_send();//�����ط�����ʱ ֻ��Ҫ������ӵ�������� ִ�� io_service_.post(boost::bind(&CSession::start_send, this));
	void start();//����ֹͣ
	void stop();
	void login();//��¼
	void heartbeat();//����

	// ��������������
	String process_hartbeat(String ip, uint16 port, String stCode);

	// ���ɵ�¼����
	String process_login(String usr, String pwd);

public:
	task_queue m_task_queue;//�̰߳�ȫ���������

	boost::asio::ip::tcp::endpoint m_ep;
	boost::asio::ip::tcp::socket m_sock;
	boost::asio::ip::tcp::resolver m_resolver;


	boost::array<unsigned char, 4096> read_buffer;
	boost::array<unsigned char, 4096> write_buffer;

	boost::asio::deadline_timer heartbeat_timer;
	int m_heartbeat_timer_minutes;//������� �Է�Ϊ��λ

	int m_nConnectState;
		
		
	int m_nHeartbeatCount;
};