#pragma once
#include <boost/asio.hpp> 
#include <boost/array.hpp> 
#include <boost/bind.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <queue>

#include "task_queue.h"

class CSession :
	public boost::enable_shared_from_this<CSession>
{
public:
	CSession(const std::string IP, unsigned short port, int m_heartbeat_timer_minutes, 
				boost::asio::io_context& io_context_);
	~CSession(void);

private:
	
	void receive_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);  //接收数据处理结果
	void connect_handler(const boost::system::error_code &ec);     //连接处理结果
	void heartbeat_handler(const boost::system::error_code &ec);   //心跳处理结果
	void login_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);    //登录结果的处理
	void start_receive();    //开始接收数据
	void send_handler(const boost::system::error_code &ec);
	std::size_t check_frame(const boost::system::error_code &ec, std::size_t bytes_transferred);   //校验数据
	void parse_frame(const boost::system::error_code &ec, std::size_t bytes_transferred);          //解析数据
public:
	void start_send();   //其他地方调用时 只需要把任务加到任务队列 执行 io_service_.post(boost::bind(&CSession::start_send, this));
	void start();        //启动停止
	void stop();
	void login();       //登录
	void heartbeat();   //心跳

	// 生成心跳包数据
	String process_hartbeat(String ip, uint16 port, String stCode);

	// 生成登录数据
	String process_login(String usr, String pwd);

public:
	task_queue m_task_queue;   //线程安全的任务队列

	boost::asio::ip::tcp::endpoint m_ep;
	boost::asio::ip::tcp::socket m_sock;
	boost::asio::ip::tcp::resolver m_resolver;


	boost::array<unsigned char, 4096> read_buffer;		// 接收缓冲区
	boost::array<unsigned char, 4096> write_buffer;		// 发送缓冲区

	boost::asio::deadline_timer heartbeat_timer;
	int m_heartbeat_timer_minutes;    //心跳间隔 以分为单位

	int m_nConnectState;
		
	int m_nHeartbeatCount;
};
