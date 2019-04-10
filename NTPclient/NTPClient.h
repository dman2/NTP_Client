#ifndef NTPClIENT_H
#define NTPCLIENT_H
// ---------------------------------------------------------------------------
// COMPANY              : PERALEX ELECTRONICS (PTY) LTD
// ---------------------------------------------------------------------------
// COPYRIGHT NOTICE :
//
// The copyright, manufacturing and patent rights stemming from this document
// in any form are vested in PERALEX ELECTRONICS (PTY) LTD.
//
// (c) PERALEX ELECTRONICS (PTY) LTD 2018
//
// PERALEX ELECTRONICS (PTY) LTD has ceded these rights to its clients where contractually agreed.
// ---------------------------------------------------------------------------
// DESCRIPTION :
//
// Class to check the time difference between two NTP servers
//
// ---------------------------------------------------------------------------
// Date:       $LastChangedDate: 2018-07-19 13:19:01 +0200 (Thu, 19 Jul 2018) $
// Revision:   $LastChangedRevision: 80416 $
// $HeadURL: svn://iserver.ct/raid/SVN/repos/EWDev/Users/Dave/NTPclient/NTPclient/NTPClient.h $
// ---------------------------------------------------------------------------
// SYSTEM INCLUDE FILES
#include <string>
#include <iostream>
#include <chrono>
//Components of the Boost Library
#include <boost/array.hpp>
#include <boost/asio.hpp>

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES

namespace MultipleKinectsPlatformServer
{
	/**
	*  A Network Time Protocol Client that queries the DateTime from the Time Server located at hostname
	*/
	class NTPClient
	{
	private:
		std::string m_strHostName;
		unsigned short m_sPort;

		boost::asio::io_service io_service;
		boost::asio::ip::udp::endpoint receiver_endpoint;
		boost::asio::ip::udp::socket socket;

		std::chrono::time_point<std::chrono::system_clock> GetTimePointFromNtp(unsigned uTime_sec, unsigned uTime_usec);

	public:
		NTPClient(std::string strhostname);
		double RequestDatetime_UNIX_Offset();
	};
}

#endif
