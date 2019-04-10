#include <thread>
#include <chrono>
#include <future>
#include <boost/bind.hpp>
#include "NTPClient.h"

using namespace std;

namespace MultipleKinectsPlatformServer
{
	/**
	 *  NTPClient
	 *  @Param i_hostname - The time server host name which you are connecting to obtain the time
	 *						eg. the pool.ntp.org project virtual cluster of timeservers
	 */
	NTPClient::NTPClient(string strhostname) :
		m_strHostName(strhostname), m_sPort(123),
		socket(io_service)
	{
		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), m_strHostName, "ntp");

		receiver_endpoint = *resolver.resolve(query);

		socket.open(boost::asio::ip::udp::v4());
		//Host name is defined by you and port number is 123 for time protocol
	}

	/**
	 * RequestDatetime_UNIX()
	 * @Returns long - number of seconds from the Unix Epoch start time
	 */
	double NTPClient::RequestDatetime_UNIX_Offset()
	{
		double dOffset;

		boost::array<unsigned char, 48> sendBuf = {0xb, 0, 0, 0, 0, 0, 0, 0, 0};

		auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
		auto duration_us = now_us.time_since_epoch().count();
		unsigned uTime_sec = ((duration_us / 1000000) + 2208988800U);
		float fTime_us = duration_us % 1000000;
		fTime_us *= 4294.967296f;
		unsigned uTime_us = (unsigned)fTime_us;

		uTime_sec = htonl(uTime_sec);
		memcpy(&sendBuf[40], &uTime_sec, sizeof(unsigned));
		uTime_us = ntohl(uTime_us);
		memcpy(&sendBuf[44], &uTime_us, sizeof(int));

		socket.send_to(boost::asio::buffer(sendBuf), receiver_endpoint);

		boost::array<unsigned long, 1024> recvBuf;
		boost::asio::ip::udp::endpoint sender_endpoint;

		try {
			SOCKET temphSocket = socket.native_handle();
			fd_set sReadSet;

			if(temphSocket == INVALID_SOCKET)
				throw std::exception("Socket closed");

			FD_ZERO(&sReadSet);
			FD_SET(temphSocket, &sReadSet); // populate set with the member socket to poll
			struct timeval SelectTime;
			SelectTime.tv_sec = 1; // set timeout
			SelectTime.tv_usec = 0;

			int iIsReadySocket = select(FD_SETSIZE, &sReadSet, NULL, NULL, &SelectTime);
			if(iIsReadySocket == SOCKET_ERROR) {
				throw std::exception("Socket closed");
			}

			if(iIsReadySocket) {
				if(!FD_ISSET(temphSocket, &sReadSet))
					return 0.0;
			}
			else
				return 0.0f;

			size_t len = socket.receive_from(boost::asio::buffer(recvBuf), sender_endpoint);
			auto end = std::chrono::system_clock::now();
			std::chrono::time_point<std::chrono::system_clock> Origin = GetTimePointFromNtp(recvBuf[6], recvBuf[7]);
			std::chrono::time_point<std::chrono::system_clock> Receive = GetTimePointFromNtp(recvBuf[8], recvBuf[9]);
			std::chrono::time_point<std::chrono::system_clock> Transmit = GetTimePointFromNtp(recvBuf[10], recvBuf[11]);

			//t1 = local clock, time request sent by client;
			//t2 = server clock, time request received by server;
			//t3 = server clock, time reply sent by server;
			//t4 = local clock, time reply received by client

			//	The offset o is then calculated as :

			//o = ((t2 - t1) + (t3 - t4)) / 2
			std::chrono::duration<double> offset_seconds = ((Receive - Origin) + (Transmit - end)) / 2;
			std::chrono::duration<double> delay_seconds = end - Origin;

			std::cout << m_strHostName;
			std::cout << ": Round trip delay: " << delay_seconds.count() << "\n";
			std::cout << "Local clock offset: " << offset_seconds.count() << endl;
			dOffset = offset_seconds.count();
		}
		catch(std::exception& e) {
			std::cerr << e.what() << std::endl;
		}

		return dOffset;
	}

	std::chrono::time_point<std::chrono::system_clock> NTPClient::GetTimePointFromNtp(unsigned uTime_sec, unsigned uTime_usec)
	{
		unsigned uTimeSec = ntohl((unsigned)uTime_sec);
		unsigned uTimeUs = ntohl((unsigned)uTime_usec);
		int64_t duration_us = (((int64_t)uTimeSec - 2208988800) * 1000000);
		float fTime_us = (float)uTimeUs / 4294.967296f;
		duration_us += (unsigned)fTime_us;
		std::chrono::microseconds dur(duration_us);

		return std::chrono::time_point<std::chrono::system_clock>(dur);
	}
}
