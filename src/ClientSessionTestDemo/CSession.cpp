#include "CSession.h"

CSession::CSession(
	const std::string IP,
	unsigned short port,
	int heartbeat_timer_minutes,
	boost::asio::io_service& io_service_)
	: m_resolver(io_service_),
	m_sock(io_service_),
	heartbeat_timer(io_service_),
	m_nConnectState(0),
	m_ep(boost::asio::ip::address::from_string(IP), port),
	m_heartbeat_timer_minutes(heartbeat_timer_minutes),
	m_nHeartbeatCount(0)
{
	read_buffer.fill(0);
}
void CSession::start()
{

	m_sock.async_connect(m_ep,
		boost::bind(&CSession::connect_handler,
			shared_from_this(),
			boost::asio::placeholders::error));
}

CSession::~CSession(void)
{
	stop();
}
void CSession::stop()
{
	heartbeat_timer.cancel();
	m_sock.close();

}


std::size_t CSession::check_frame(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
	//bytes_transferred �Ѿ����յ��ֽ���
	//����0 ��ʾ ��֤ͨ��
	//����1 ��ʾ ��Ҫ������֤
	return 0;
}
void CSession::parse_frame(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
	if (!ec)
	{

	}
}

void CSession::start_receive()
{
	m_sock.async_receive(boost::asio::buffer(read_buffer),
		boost::bind(&CSession::receive_handler,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}
void CSession::receive_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
	if (!ec)
	{

		//check_frame ��֤���յ����ݣ�����֤�ɹ�ʱ����ʼ
		boost::asio::async_read(m_sock, boost::asio::buffer(read_buffer),
			boost::bind(&CSession::check_frame,
				shared_from_this(), boost::asio::placeholders::error,
				bytes_transferred),

			boost::bind(&CSession::parse_frame,
				shared_from_this(),
				boost::asio::placeholders::error,
				bytes_transferred));





		m_sock.async_receive(boost::asio::buffer(read_buffer),
			boost::bind(&CSession::receive_handler,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		stop();
	}
}

void CSession::heartbeat()
{
	//unsigned char c[256];
	String heartStr = process_hartbeat("127.0.0.1", 8088, "123456789");//������������
	m_sock.async_write_some(boost::asio::buffer(heartStr.data(), heartStr.size()), 
		boost::bind(&CSession::heartbeat_handler, shared_from_this(), boost::asio::placeholders::error));
}


// ��������������
String CSession::process_hartbeat(String ip, uint16 port, String stCode)
{
	String heartStr = "@@" + ip +  std::to_string(port) + "##" + stCode;

	return heartStr;
}

// ���ɵ�¼����
String CSession::process_login(String usr, String pwd)
{
	String loginStr = "username:" + usr + "##password:" + pwd;

	return loginStr;
}

void CSession::heartbeat_handler(const boost::system::error_code &ec)
{
	if (!ec)
	{
		heartbeat_timer.expires_from_now(boost::posix_time::minutes(m_heartbeat_timer_minutes));
		heartbeat_timer.async_wait(boost::bind(&CSession::heartbeat, shared_from_this()));

	}
	else
	{
		stop();
	}
}


void CSession::login()//��¼
{
	//������� ���͵�¼֡
	//*******************************************************************************************
	//unsigned char c[256];
	//UINT nLen = process_login(c);//���ɵ�¼���� ���� c�д洢���û����ƺ������

	String loginStr = process_login("have", "123456");
	m_sock.async_write_some(boost::asio::buffer(loginStr.data(), loginStr.size()), 
		boost::bind(&CSession::login_handler,
		shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}
void CSession::login_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
	if (!ec)
	{
		start_receive();//��������
		heartbeat();
	}
	else
	{
		stop();
	}
}

void CSession::connect_handler(const boost::system::error_code &ec)
{

	if (!ec)
	{
		boost::this_thread::sleep(boost::posix_time::microseconds(500));

		login();//���ӳɹ�֮��ʼ��¼
	}
	else
	{

		boost::this_thread::sleep(boost::posix_time::microseconds(60000));
		start();//����ʧ�� ��Ҫ�ٴ�����



	}
}

void CSession::start_send()
{
	//���̰߳�ȫ����������л�ȡһ������ʼ����
	std::tuple<bool, task> ret = m_task_queue.get_nonblock();
	if (std::get<0>(ret))
	{
		unsigned char* c = std::get<1>(ret).data;
		int nLen = std::get<1>(ret).len;
		/**************************************************************************************************/
		m_sock.async_write_some(boost::asio::buffer(c, nLen), boost::bind(&CSession::send_handler,
			shared_from_this(),
			boost::asio::placeholders::error));
	}

}
void CSession::send_handler(const boost::system::error_code &ec)
{
	if (!ec)
	{
		start_send();//������в�Ϊ��ʱ����ʼ������һ������
	}
}