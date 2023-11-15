This is a Visual studio sln.(Standard C++ 17)
External dependencies: 
boost_1_83_0 : please download and include in local dir and update the path in the project include path.

/*
* You can call the logger using before priorities/severity
* example: Logger::INFO("message");
* Logger::CRITICAL("critical message");
* New line is automatically done. 
* This pushes the message in the logger queue and is read by a separate WriterThread and written to disk.
* Current latency from the time the log is generated to the time the log is written is less than 1 ms, which can be verified from the logs, as we print both timestamps.
* Sample log: 14:10:28.184 : [INFO][14:10:28.183] This is info trace id: 244 
*	Time of writing to file : [Priority][Time the log is generated] Log Message 
*/


To build and run:

Build the sln in visual studio (Standard C++ 17)

Main function:
	creates a log.txt in local folder to write the logs.
	std::chrono::milliseconds t(10); // Increase/decrease this time to increase or decrease logging.
	
