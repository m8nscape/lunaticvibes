#pragma once
#include <string>
#include <vector>
#include <future>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "common/types.h"
#include "common/asynclooper.h"

#ifndef CALLBACK
#define CALLBACK
#endif

// unused
struct ArenaLobbyInfo
{
	std::string address;	// IP, supports ipv6
	std::string name;		// host player name
	unsigned players = 0;		// 
};

class ArenaMessage;
class ArenaClient: public AsyncLooper
{
public:
	ArenaClient() : AsyncLooper("Arena Client", std::bind(&ArenaClient::update, this), 60) {}
	virtual ~ArenaClient();

private:
	using udp = boost::asio::ip::udp;
	boost::asio::io_context ioc;
	udp::endpoint server;
	std::shared_ptr<udp::socket> socket;
	std::array<unsigned char, 1024> recv_buf;
	udp::endpoint remote_endpoint;
	std::future<void> listen;

	int joinLobbyErrorCode = -1;

	HashMD5 requestChartHash;
	bool _isLoadingFinished = false; 
	bool _isCreatedRuleset = false;	
	bool _isPlayingFinished = false; 
	bool _isResultFinished = false;  

	int sendMessageIndex = 0;
	int recvMessageIndex = 0;

	Time heartbeatTime;

	int playerID = 0;

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
		tasksWaitingForResponse[messageIndex] = { Time(), msg, 0, false };
	}

public:
	// lobby
	std::vector<ArenaLobbyInfo> seekLobby();
	bool joinLobby(const std::string& address);
	void leaveLobby();

	// select song
	void requestChart(const HashMD5& chart);

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
	void asyncRecv();
	void handleRecv(const boost::system::error_code& error, size_t bytes_transferred);

	void handleRequest(const unsigned char* recv_buf, size_t recv_buf_len);
	void handleResponse(std::shared_ptr<ArenaMessage> msg);

	void handleJoinLobbyResp(std::shared_ptr<ArenaMessage> msg);
	void handleHeartbeat(std::shared_ptr<ArenaMessage> msg);
	void handleNotice(std::shared_ptr<ArenaMessage> msg);
	void handleDisbandLobby(std::shared_ptr<ArenaMessage> msg);
	void handlePlayerJoined(std::shared_ptr<ArenaMessage> msg);
	void handlePlayerLeft(std::shared_ptr<ArenaMessage> msg);
	void handleCheckChartExist(std::shared_ptr<ArenaMessage> msg);
	void handleHostRequestChart(std::shared_ptr<ArenaMessage> msg);
	void handleHostStartPlaying(std::shared_ptr<ArenaMessage> msg);
	void handleHostPlayInit(std::shared_ptr<ArenaMessage> msg);
	void handleHostFinishedLoading(std::shared_ptr<ArenaMessage> msg);
	void handleHostPlayData(std::shared_ptr<ArenaMessage> msg);
	void handleHostFinishedPlaying(std::shared_ptr<ArenaMessage> msg);
	void handleHostFinishedResult(std::shared_ptr<ArenaMessage> msg);

protected:
	void update();
};