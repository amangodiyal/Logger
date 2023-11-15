#pragma once

#include <mutex>
#include<cstdio>
#include<deque>
#include <boost/date_time/posix_time/posix_time.hpp>

class Logger {
public:
	enum Priority
	{
		Trace, Debug, Info, Warn, Error, Critical
	};
	struct Message
	{
		std::string message;
		Logger::Priority p;
		Message(Logger::Priority prior, std::string  m)
		{
			message = m;
			p = prior;
		}
	};
private:
	Logger(){}
	Logger(const Logger&) = delete;
	Logger& operator= (const Logger&) = delete;
	Priority logPriority = Info;
	std::mutex mtx;
	std::condition_variable cv;
	std::deque<Message*> logQueue;
	std::FILE* file = 0;
	
	~Logger()
	{

	}
	
	static void writeToFile()
	{
		Logger &instance = getInstance();
		std::unique_lock<std::mutex> lck(instance.mtx);
		instance.cv.wait(lck, [&instance] { return !instance.logQueue.empty(); }); //wait for main thread to finish pushing to queue.
		while (true)
		{
			Message* log = instance.logQueue.front();
			instance.logQueue.pop_front();
			if (log->p >= instance.logPriority) //log is printed if message priority is greater than or equal to current logPriority, which can be set by main thread by calling setPriority function.
			{
				//printf("%s : %s \n", getNowTime().c_str(), log->message.c_str()); //prints to console
				if (instance.file)
					fprintf(instance.file, "%s : %s \n", getNowTime().c_str(), log->message.c_str()); //print to file
			}
			delete log;
			if (instance.logQueue.empty()) //print the messages untill queue is empty()
				break;
		}
		lck.unlock();
		instance.cv.notify_one(); //notify main thread
	}

	void pushInQueue(Logger::Priority p, std::string s)
	{
		std::thread WriterThread(writeToFile);
		std::unique_lock<std::mutex> lck(mtx);
		logQueue.emplace_back(new Message(p, s));
		lck.unlock();
		cv.notify_one(); //notify WriterThread that main thread has finished writing to queue
		
		WriterThread.join();
	}
	bool isFileReady()
	{
		if (file)
		{
			std::fclose(file);
			file = 0;
		}
		file = std::fopen("log.txt", "a");
		if (file == 0)
		{
			return false;
		}

		return true;
	}

public:

	static Logger& getInstance()
	{
		static Logger instance; //Singleton function to get the instance of Logger
		return instance;
	}
	static void setPriority(Priority p)
	{
		getInstance().logPriority = p;
	}

	static bool isFileOpen()
	{
		return getInstance().isFileReady();
	}
	
	static std::string getNowTime() //returns time uptill millisecond precision
	{
		const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		const boost::posix_time::time_duration td = now.time_of_day();

		const long hours = td.hours();
		const long minutes = td.minutes();
		const long seconds = td.seconds();
		const long milliseconds = td.total_milliseconds() -
			((hours * 3600 + minutes * 60 + seconds) * 1000);

		char buf[40];
		sprintf(buf, "%02ld:%02ld:%02ld.%03ld",	hours, minutes, seconds, milliseconds);

		return buf;
	}

	template<typename ...Args>
	static void INFO(const char* message, Args ...args)
	{
		char buff[200];
		memset(buff,0,sizeof(buff));
		sprintf(buff, message, args...);
		std::string strBuff = "[INFO]" + std::string(buff);
		getInstance().pushInQueue(Priority::Info, strBuff);
	}
	template<typename ...Args>
	static void TRACE(const char* message, Args ...args)
	{
		char buff[200];
		memset(buff, 0, sizeof(buff));
		sprintf_s(buff, message, args...);
		std::string strBuff = "[TRACE]" + std::string(buff);
		getInstance().pushInQueue(Priority::Trace, strBuff);
	}
	template<typename ...Args>
	static void DEBUG(const char* message, Args ...args)
	{
		char buff[200];
		memset(buff, 0, sizeof(buff));
		sprintf_s(buff, message, args...);
		std::string strBuff = "[DEBUG]" + std::string(buff);
		getInstance().pushInQueue(Priority::Debug, strBuff);
	}
	template<typename ...Args>
	static void WARN(const char* message, Args ...args)
	{
		char buff[200];
		memset(buff, 0, sizeof(buff));
		sprintf_s(buff, message, args...);
		std::string strBuff = "[WARN]" + std::string(buff);
		getInstance().pushInQueue(Priority::Warn, strBuff);
	}

	template<typename ...Args>
	static void ERROR(const char* message, Args ...args)
	{
		char buff[200];
		memset(buff, 0, sizeof(buff));
		sprintf_s(buff, message, args...);
		std::string strBuff = "[ERROR]" + std::string(buff);
		getInstance().pushInQueue(Priority::Error, strBuff);
	}
	template<typename ...Args>
	static void CRITICAL(const char* message, Args ...args)
	{
		char buff[200];
		memset(buff, 0, sizeof(buff));
		sprintf_s(buff, message, args...);
		std::string strBuff = "[CRITICAL]" + std::string(buff);
		getInstance().pushInQueue(Priority::Critical, strBuff);
	}

};
