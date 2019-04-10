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

	MultipleKinectsPlatformServer::NTPClient MRD7090_NTP(argv[1]);
	MultipleKinectsPlatformServer::NTPClient MRR8001_NTP(argv[2]);
	while(1) {
		double d7090_sec = MRD7090_NTP.RequestDatetime_UNIX_Offset();
		double d8001_sec = MRR8001_NTP.RequestDatetime_UNIX_Offset();

		if(abs(d7090_sec - d8001_sec) < 0.05)
			cout << "Processes synced : " << abs(d7090_sec - d8001_sec) << " seconds difference\n"
				 << endl;
		else
			cout << "EQS DAQ are out of sync : " << abs(d7090_sec - d8001_sec) << " seconds difference\n"
				 << endl;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}
