#include <iostream>
#include <thread>
#include <chrono>

#include "NTPClient.h"

using namespace std;

int main(int argc, char *argv[])
{
	if(argc != 3) {
		cerr << "Require IP addresses of both NTP servers i.e. NTPClient.exe 192.168.1.1 192.168.2.2" << endl;
		return -1;
	}

	MultipleKinectsPlatformServer::NTPClient First_NTP_Server(argv[1]);
	MultipleKinectsPlatformServer::NTPClient Second_NTP_Server(argv[2]);
	while(1) {
		double dFirst_sec = First_NTP_Server.RequestDatetime_UNIX_Offset();
		double dSecond_sec = Second_NTP_Server.RequestDatetime_UNIX_Offset();

		if(abs(dFirst_sec - dSecond_sec) < 0.05)
			cout << "Processes synced : " << abs(dFirst_sec - dSecond_sec) << " seconds difference\n"
				 << endl;
		else
			cout << "EQS DAQ are out of sync : " << abs(dFirst_sec - dSecond_sec) << " seconds difference\n"
				 << endl;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}
