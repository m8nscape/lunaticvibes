#pragma once

#include <future>
#include <shared_mutex>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "common/asynclooper.h"

class ArenaMessage;
class ArenaHost: public AsyncLooper
{
public:
	ArenaHost() : AsyncLooper("Arena Host", std::bind(&ArenaHost::update, this), 60) {}
	virtual ~ArenaHost();

private:
	using udp = boost::asio::ip::udp;
	boost::asio::io_context ioc4;
	boost::asio::io_context ioc6;
	std::shared_ptr<udp::socket> v4;
	std::shared_ptr<udp::socket> v6;
	std::array<unsigned char, 1024> v4_recv_buf;
	udp::endpoint v4_remote_endpoint;
	std::array<unsigned char, 1024> v6_recv_buf;
	udp::endpoint v6_remote_endpoint;
	std::future<void> l4;
	std::future<void> l6;

	HashMD5 hostRequestChartHash;		// host request
	HashMD5 requestChartHash;			// global 

	HashMD5 requestChartPending;
	std::string requestChartPendingClientKey;		// who has requested this chart
	int requestChartPendingExistCount = 0;	// if count == clients.size(), inform all clients

	bool _isLoadingFinished = false; // host status
	bool _isCreatedRuleset = false;	// host status
	bool _isPlayingFinished = false; // host status
	bool _isResultFinished = false;  // host status

	int playStartTimeMs = 0;

	struct Client
	{
		std::shared_ptr<udp::socket> serverSocket;	// v4 / v6
		udp::endpoint endpoint;

		int id = 0;
		std::string name;

		int ping = 0;
		int sendMessageIndex = 0;
		int recvMessageIndex = 0;

		bool heartbeatPending = false;
		Time heartbeatSendTime;
		Time heartbeatRecvTime;

		HashMD5 requestChartHash;

		int playDataIndex = -1;

		bool isLoadingFinished = false;
		bool isPlayingFinished = false;
		bool isResultFinished = false;

		int playStartTimeMs = 0;

		struct Task
		{
			Time t;
			std::shared_ptr<std::vector<unsigned char>> sentMessage;
			int retryTimes = 0;
			bool received = false;
		};
		std::shared_mutex tasksWaitingForResponseMutex;
		std::map<int, Task> tasksWaitingForResponse;
		void addTaskWaitingForResponse(int messageIndex, std::shared_ptr<std::vector<unsigned char>> msg)
		{
			std::unique_lock l(tasksWaitingForResponseMutex);
			tasksWaitingForResponse[messageIndex] = {Time(), msg, 0, false};
		}
	};
	int clientID = 0;
	std::map<std::string, Client> clients;
	std::shared_mutex clientsMutex;

	std::vector<std::string> getPlayerNameList(const std::string& keyExclude);

public:
	// lobby
	bool createLobby();
	void disbandLobby();

	// select song
	void requestChart(const HashMD5& chart, const std::string clientKey = "host");

	// request start playing
	void startPlaying();

	// status update
	void setLoadingFinished(int playStartTimeMs);
	void setCreatedRuleset();
	void setPlayingFinished();
	void setResultFinished();
	bool isLoadingFinished() const { return _isLoadingFinished; }
	bool isCreatedRuleset() const { return _isCreatedRuleset; }
	bool isPlayingFinished() const { return _isPlayingFinished; }
	bool isResultFinished() const { return _isResultFinished; }


protected:
	void asyncRecv4();
	void asyncRecv6();
	void handleRecv4(const boost::system::error_code& error, size_t bytes_transferred);
	void handleRecv6(const boost::system::error_code& error, size_t bytes_transferred);

	void handleRequest(const unsigned char* recv_buf, size_t recv_buf_len, const udp::endpoint& remote_endpoint);
	void handleResponse(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);

	void handleHeartbeatResp(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handleJoinLobby(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handlePlayerLeft(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handleRequestChart(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handleCheckChartExistResp(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handlePlayInit(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handleFinishedLoading(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handlePlayData(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handleFinishedPlaying(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);
	void handleFinishedResult(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg);

protected:
	void update();

};