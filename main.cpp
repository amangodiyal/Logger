#include<thread>
#include<iostream>
#include<logger.h>
#include<cstring>



void logMessage(int i)
{
	/*
	* Main thread can call the logger using before priorities/severity
	* example: Logger::INFO("message");
	* Logger::CRITICAL("critical message");
	* This pushes the message in the logger queue and is read by a separate WriterThread and written to disk.
	* Current latency from the time the log is generated to the time the log is wriiten is less than 1 ms, can be verified from the logs, as we print both timestamps.
	*/
	Logger::INFO("[%s] This is info trace id: %d", Logger::getNowTime().c_str(), i);
	/*Logger::TRACE("This is TRACE trace id: %d", i);
	Logger::WARN("This is WARN trace id: %d", i);
	Logger::ERROR("This is ERROR trace id: %d",i);
	Logger::CRITICAL("This is CRITICAL trace id: %d", i);
	Logger::DEBUG("This is DEBUG trace id: %d", i);*/
}
int main(int argc, char *argv[])
{ 
	int i = 0;
	bool isFileOpen = Logger::isFileOpen(); 
	std::chrono::milliseconds t(10); //Increase/decrease this time to increase decrease logging.
	std::chrono::time_point starting_time = std::chrono::steady_clock::now();
	while (isFileOpen) {
		std::this_thread::sleep_until(starting_time + t);
		logMessage(i++);
		starting_time += t;
	}
	return 0;
}