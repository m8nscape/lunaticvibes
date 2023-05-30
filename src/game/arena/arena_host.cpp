#include "common/pch.h"
#include "arena_host.h"
#include "arena_internal.h"
#include "db/db_song.h"
#include "game/runtime/i18n.h"
#include "game/data/data_types.h"
#include "game/ruleset/ruleset_bms_network.h"
#include "git_version.h"

namespace lunaticvibes
{

std::shared_ptr<ArenaHost> g_pArenaHost = nullptr;

ArenaHost::~ArenaHost()
{
	if (ArenaData.isOnline())
	{
		disbandLobby();
	}
}

std::vector<std::string> ArenaHost::getPlayerNameList(const std::string& keyExclude)
{
	std::vector<std::string> l;
	if (keyExclude != "host")
	{
		l.push_back(SystemData.playerName);
	}
	for (auto& [k, cc] : clients)
	{
		if (k != keyExclude)
			l.push_back(cc.name);
	}
	return l;
}

static void emptyHandleSend(std::shared_ptr<std::vector<unsigned char>> message, const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
{
}

bool ArenaHost::createLobby()
{
	if (ArenaData.isOnline())
	{
		LOG_WARNING << "[Arena] Host failed! Please leave or disband your current lobby first.";
		return false;
	}

	LOG_INFO << "[Arena] Hosting with port " << ARENA_HOST_PORT;
	try
	{
		v4 = std::make_shared<udp::socket>(ioc4, udp::endpoint(udp::v4(), ARENA_HOST_PORT));
	}
	catch (std::exception& e)
	{
		LOG_WARNING << "[Arena] ipv4 socket exception: " << to_utf8(e.what(), eFileEncoding::LATIN1);
	}
	if (v4 && !v4->is_open())
	{
		LOG_WARNING << "[Arena] ipv4 socket failed";
		v4.reset();
	}
	try
	{
		v6 = std::make_shared<udp::socket>(ioc6, udp::endpoint(udp::v6(), ARENA_HOST_PORT));
	}
	catch (std::exception& e)
	{
		LOG_WARNING << "[Arena] ipv6 socket exception: " << to_utf8(e.what(), eFileEncoding::LATIN1);
	}
	if (v6 && !v6->is_open())
	{
		LOG_WARNING << "[Arena] ipv6 socket failed";
		v6.reset();
	}

	if (!v4 && !v6)
	{
		LOG_WARNING << "[Arena] Both ipv4 and ipv6 socket failed. Check if port " << ARENA_HOST_PORT << " is in use.";
		return false;
	}
	LOG_INFO << "[Arena] Host success";

	if (v4)
	{
		ArenaData.online = true;
		asyncRecv4();
		l4 = std::async(std::launch::async, [&] { ioc4.run(); });
	}
	if (v6)
	{
		ArenaData.online = true;
		asyncRecv6();
		l6 = std::async(std::launch::async, [&] { ioc6.run(); });
	}

	return true;
}

void ArenaHost::disbandLobby()
{
	if (!ArenaData.isOnline()) return;

	if (!ArenaData.isExpired())
	{
		for (auto& [k, cc] : clients)
		{
			auto n = std::make_shared<ArenaMessageDisbandLobby>();
			n->messageIndex = ++cc.sendMessageIndex;

			auto payload = n->pack();
			cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
			// no response
		}

		// wait for msg send
		if (!clients.empty())
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	if (v4)
	{
		ioc4.stop();
		l4.wait();
	}
	if (v6)
	{
		ioc6.stop();
		l6.wait();
	}

	ArenaData.expired = true;
}

void ArenaHost::requestChart(const HashMD5& reqChart, const std::string clientKey)
{
	if (reqChart.empty())
	{
		requestChartHash.reset();
		hostRequestChartHash.reset();

		// Remove all ready stat
		decltype(std::declval<ArenaMessageHostReadyStat>().ready) ready;
		ready[0] = false;
		ArenaData.ready = false;
		for (auto& [k, cc] : clients)
		{
			cc.requestChartHash.reset();
			ready[cc.id] = false;
			ArenaData.data[cc.id].ready = false;
		}
		for (auto& [k, cc] : clients)
		{
			auto n1 = std::make_shared<ArenaMessageHostRequestChart>();
			n1->messageIndex = ++cc.sendMessageIndex;

			auto payload = n1->pack();
			cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
			cc.addTaskWaitingForResponse(n1->messageIndex, payload);
		}
		ArenaData.isArenaCancellingRequest = true;
		return;
	}

	// clear if chart hash not match
	if (reqChart != requestChartPending)
	{
		requestChartPending.reset();
		requestChartPendingClientKey.clear();
		requestChartPendingExistCount = 0;
	}
	if (reqChart != hostRequestChartHash)
	{
		hostRequestChartHash.reset();
	}
	for (auto& client : clients)
	{
		if (reqChart != client.second.requestChartHash)
			client.second.requestChartHash.reset();
	}

	if (reqChart == requestChartHash)
	{
		if (clientKey == "host")
		{
			// host has ready for this chart
			hostRequestChartHash = reqChart;
		}
		else
		{
			// client has ready for this chart
			Client& c = clients[clientKey];
			c.requestChartHash = reqChart;
		}

		// inform clients
		decltype(std::declval<ArenaMessageHostReadyStat>().ready) ready;
		ready[0] = hostRequestChartHash == reqChart;
		for (auto& [k, cc] : clients)
		{
			ready[cc.id] = cc.requestChartHash == reqChart;
		}
		for (auto& [k, cc] : clients)
		{
			auto n = std::make_shared<ArenaMessageHostReadyStat>();
			n->messageIndex = ++cc.sendMessageIndex;
			n->ready = ready;

			auto payload = n->pack();
			cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

			cc.addTaskWaitingForResponse(n->messageIndex, payload);
		}
	}
	else
	{
		// new chart requested. first check if host has this chart
		if (clientKey == "host" || !g_pSongDB->findChartByHash(reqChart).empty())
		{
			// then check if all users have this chart
			requestChartPending = reqChart;
			requestChartPendingClientKey = clientKey;
			requestChartPendingExistCount = clientKey == "host" ? 1 : 2;	// requester + host

			// send CHECK_CHART_EXIST to all clients, check in update()
			std::string hashStr = reqChart.hexdigest();
			for (auto& [k, cc] : clients)
			{
				if (k == clientKey) continue;

				auto n = std::make_shared<ArenaMessageCheckChartExist>();
				n->messageIndex = ++cc.sendMessageIndex;
				n->chartHashMD5String = hashStr;

				auto payload = n->pack();
				cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

				cc.addTaskWaitingForResponse(n->messageIndex, payload);
			}
		}
		else
		{
			// client requested, but host does not have this chart

			Client& c = clients[clientKey];
			if (c.alive)
			{
				auto n = std::make_shared<ArenaMessageNotice>();
				n->messageIndex = ++c.sendMessageIndex;
				n->i18nIndex = i18nText::ARENA_REQUEST_FAILED_PLAYER_NO_CHART;
				n->format = ArenaMessageNotice::S1;
				n->s1 = SystemData.playerName;

				auto payload = n->pack();
				c.serverSocket->async_send_to(boost::asio::buffer(*payload), c.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

				c.addTaskWaitingForResponse(n->messageIndex, payload);
			}
		}
	}

}


void ArenaHost::startPlaying()
{
	for (auto& [k, cc] : clients)
	{
		auto n = std::make_shared<ArenaMessageHostStartPlaying>();
		n->messageIndex = ++cc.sendMessageIndex;
		n->rulesetType = (uint32_t)RulesetType::BMS;
		n->chartHashMD5String = hostRequestChartHash.hexdigest();
		n->randomSeed = ArenaData.getRandomSeed();

		auto payload = n->pack();
		cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

		cc.addTaskWaitingForResponse(n->messageIndex, payload);
	}

	ArenaData.initPlaying(RulesetType::BMS);
	ArenaData.isArenaReady = true;
}

void ArenaHost::setLoadingFinished(int playStartTimeMs)
{ 
	_isLoadingFinished = true; 
	this->playStartTimeMs = playStartTimeMs;
}

void ArenaHost::setCreatedRuleset() 
{ 
	_isCreatedRuleset = true; 
}

void ArenaHost::setPlayingFinished()
{ 
	_isPlayingFinished = true; 
}

void ArenaHost::setResultFinished()
{ 
	_isResultFinished = true; 
}

void ArenaHost::asyncRecv4()
{
	v4->async_receive_from(boost::asio::buffer(v4_recv_buf), v4_remote_endpoint,
		std::bind(&ArenaHost::handleRecv4, this, std::placeholders::_1, std::placeholders::_2));
}

void ArenaHost::asyncRecv6()
{
	v6->async_receive_from(boost::asio::buffer(v6_recv_buf), v6_remote_endpoint,
		std::bind(&ArenaHost::handleRecv6, this, std::placeholders::_1, std::placeholders::_2));
}

void ArenaHost::handleRecv4(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		LOG_WARNING << "[Arena] ipv4 socket exception: " << to_utf8(error.message(), eFileEncoding::LATIN1);

		/*
		if (v4)
		{
			ioc4.stop();
			l4.wait();
		}
		if (v6)
		{
			ioc6.stop();
			l6.wait();
		}

		ArenaData.expired = true;
		return;
		*/
	}
	else
	{
		if (bytes_transferred > v4_recv_buf.size())
		{
			LOG_DEBUG << "[Arena] Ignored huge packet (" << bytes_transferred << ")";
			return;
		}

		handleRequest(v4_recv_buf.data(), bytes_transferred, v4_remote_endpoint);
	}

	if (ArenaData.isOnline() && !ArenaData.isExpired())
		asyncRecv4();
}

void ArenaHost::handleRecv6(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		LOG_WARNING << "[Arena] ipv6 socket exception: " << to_utf8(error.message(), eFileEncoding::LATIN1);

		/*
		if (v4)
		{
			ioc4.stop();
			l4.wait();
		}
		if (v6)
		{
			ioc6.stop();
			l6.wait();
		}

		ArenaData.expired = true;
		return;
		*/
	}
	else
	{
		if (bytes_transferred > v6_recv_buf.size())
		{
			LOG_DEBUG << "[Arena] Ignored huge packet (" << bytes_transferred << ")";
			return;
		}

		handleRequest(v6_recv_buf.data(), bytes_transferred, v6_remote_endpoint);
	}

	if (ArenaData.isOnline() && !ArenaData.isExpired())
		asyncRecv6();
}

void ArenaHost::handleRequest(const unsigned char* recv_buf, size_t recv_buf_len, const udp::endpoint& remote_endpoint)
{
	boost::asio::ip::address addr = remote_endpoint.address();

	auto pMsg = ArenaMessage::unpack(recv_buf, recv_buf_len);
	if (pMsg)
	{
		if (pMsg->type == Arena::SEEK_LOBBY)
		{
			// this is broadcast request
			ArenaLobbyResp subPayload;
			subPayload.name = SystemData.playerName;
			subPayload.address = addr.to_string();
			subPayload.port = ARENA_HOST_PORT;
			std::stringstream ss;
			try
			{
				::cereal::PortableBinaryOutputArchive ar(ss);
				ar(subPayload);
			}
			catch (...)
			{
			}
			size_t length = ss.tellp();
			if (length == 0)
			{
				LOG_WARNING << "[Arena] Pack SEEK_LOBBY resp payload failed";
				assert(false);
				return;
			}

			ArenaMessageResponse resp(*pMsg);
			resp.errorCode = 0;
			resp.payload.resize(length);
			ss.read((char*)&resp.payload[0], length);

			auto socket = addr.is_v4() ? v4 : v6;
			auto payload = resp.pack();
			socket->async_send_to(boost::asio::buffer(*payload), remote_endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

			return;
		}

		std::string key = addr.to_string() + ":" + std::to_string(remote_endpoint.port());
		if (pMsg->type == Arena::JOIN_LOBBY)
		{
			auto socket = addr.is_v4() ? v4 : v6;

			auto pJoinLobbyMsg = std::static_pointer_cast<ArenaMessageJoinLobby>(pMsg);
			if (pJoinLobbyMsg->version == (boost::format("%s %s") % GIT_BRANCH % GIT_COMMIT).str())
			{
				if (!ArenaData.playing)
				{
					if (clients.find(key) == clients.end())
					{
						if (ArenaData.getPlayerCount() < MAX_ARENA_PLAYERS)
						{
							std::unique_lock l(clientsMutex);
							clients[key].id = ++clientID;
							clients[key].serverSocket = socket;
							clients[key].endpoint = remote_endpoint;
						}
						else
						{
							ArenaMessageResponse resp(*pMsg);
							resp.errorCode = 1;	// FIXME no enough slots
							auto payload = resp.pack();
							socket->async_send_to(boost::asio::buffer(*payload), remote_endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
						}
					}
					else
					{
						ArenaMessageResponse resp(*pMsg);
						resp.errorCode = 255;	// FIXME duplicate address
						auto payload = resp.pack();
						socket->async_send_to(boost::asio::buffer(*payload), remote_endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
					}
				}
				else
				{
					ArenaMessageResponse resp(*pMsg);
					resp.errorCode = 2;	// FIXME host is playing
					auto payload = resp.pack();
					socket->async_send_to(boost::asio::buffer(*payload), remote_endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
				}
			}
			else
			{
				ArenaMessageResponse resp(*pMsg);
				resp.errorCode = 254;	// FIXME version not match
				auto payload = resp.pack();
				socket->async_send_to(boost::asio::buffer(*payload), remote_endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
			}
		}
		std::shared_lock l(clientsMutex);
		if (clients.find(key) == clients.end())
		{
			// unrecognized client
			return;
		}

		switch (pMsg->type)
		{
		case Arena::RESPONSE:                 handleResponse(key, pMsg); break;
		case Arena::JOIN_LOBBY:               handleJoinLobby(key, pMsg); break;
		case Arena::LEAVE_LOBBY:              handlePlayerLeft(key, pMsg); break;
		case Arena::REQUEST_CHART:            handleRequestChart(key, pMsg); break;
		case Arena::CLIENT_PLAY_INIT:         handlePlayInit(key, pMsg); break;
		case Arena::CLIENT_FINISHED_LOADING:  handleFinishedLoading(key, pMsg); break;
		case Arena::CLIENT_PLAYDATA:          handlePlayData(key, pMsg); break;
		case Arena::CLIENT_FINISHED_PLAYING:  handleFinishedPlaying(key, pMsg); break;
		case Arena::CLIENT_FINISHED_RESULT:   handleFinishedResult(key, pMsg); break;
		}
		if (pMsg->type != Arena::RESPONSE)
			clients[key].recvMessageIndex = std::max(clients[key].recvMessageIndex, pMsg->messageIndex);
	}
}


void ArenaHost::handleResponse(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageResponse>(msg);

	Client& c = clients[clientKey];

	std::shared_lock l(c.tasksWaitingForResponseMutex);
	if (c.tasksWaitingForResponse.find(pMsg->messageIndex) == c.tasksWaitingForResponse.end())
	{
		LOG_WARNING << "[Arena] Invalid req message index, or duplicate resp msg";
		return;
	}

	c.tasksWaitingForResponse[pMsg->messageIndex].received = true;

	if (pMsg->errorCode != 0)
	{
		LOG_WARNING << "[Arena] Req type " << pMsg->reqType << " error with code " << pMsg->errorCode;
		return;
	}

	switch (pMsg->reqType)
	{
	case Arena::HEARTBEAT:			handleHeartbeatResp(clientKey, msg); break;
	case Arena::CHECK_CHART_EXIST:	handleCheckChartExistResp(clientKey, msg); break;
	}
}

void ArenaHost::handleHeartbeatResp(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageResponse>(msg);

	Client& c = clients[clientKey];
	c.heartbeatPending = false;
	c.heartbeatRecvTime = Time();

	// RTT
	c.ping = (c.heartbeatRecvTime - c.heartbeatSendTime).norm();
}

void ArenaHost::handleJoinLobby(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageJoinLobby>(msg);

	Client& c = clients[clientKey];

	c.name = pMsg->playerName;

	ArenaData.data[c.id].name = pMsg->playerName;
	ArenaData.playerIDs.push_back(c.id);

	ArenaMessageResponse resp(*pMsg);

	ArenaJoinLobbyResp subPayload;
	subPayload.playerID = c.id;
	subPayload.playerName[0] = SystemData.playerName;
	for (const auto& [id, d] : ArenaData.data)
	{
		if (id == c.id) continue;
		subPayload.playerName[id] = d.name;
	}
	std::stringstream ss;
	try
	{
		::cereal::PortableBinaryOutputArchive ar(ss);
		ar(subPayload);
	}
	catch (...)
	{
	}
	size_t length = ss.tellp();
	if (length == 0)
	{
		LOG_WARNING << "[Arena] Pack JOIN_LOBBY resp payload failed";
		assert(false);
		return;
	}
	resp.payload.resize(length);
	ss.read((char*)&resp.payload[0], length);

	auto payload = resp.pack();
	c.serverSocket->async_send_to(boost::asio::buffer(*payload), c.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	createNotification((boost::format(i18n::c(i18nText::ARENA_PLAYER_JOINED)) % c.name).str());

	// update player list for all clients
	for (auto& [k, cc] : clients)
	{
		if (k == clientKey) continue;

		auto n = std::make_shared<ArenaMessagePlayerJoinedLobby>();
		n->messageIndex = ++cc.sendMessageIndex;
		n->playerID = c.id;
		n->playerName = c.name;

		auto payload = n->pack();
		cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
		cc.addTaskWaitingForResponse(n->messageIndex, payload);
	}
}

void ArenaHost::handlePlayerLeft(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageLeaveLobby>(msg);

	Client& c = clients[clientKey];
	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	c.serverSocket->async_send_to(boost::asio::buffer(*payload), c.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	c.alive = false;

	// update player list for all clients
	for (auto& [k, cc] : clients)
	{
		if (k == clientKey) continue;

		auto n = std::make_shared<ArenaMessagePlayerLeftLobby>();
		n->messageIndex = ++cc.sendMessageIndex;
		n->playerID = c.id;

		auto payload = n->pack();
		cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
		cc.addTaskWaitingForResponse(n->messageIndex, payload);
	}
}

void ArenaHost::handleRequestChart(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageRequestChart>(msg);

	Client& c = clients[clientKey];
	ArenaMessageResponse resp(*pMsg);

	HashMD5 reqChart(pMsg->chartHashMD5String);
	requestChart(reqChart, clientKey);

	auto payload = resp.pack();
	c.serverSocket->async_send_to(boost::asio::buffer(*payload), c.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
}

void ArenaHost::handleCheckChartExistResp(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageResponse>(msg);

	Client& c = clients[clientKey];
	ArenaCheckChartExistResp p;
	std::stringstream ss;
	ss.write((char*)&pMsg->payload[0], pMsg->payload.size());
	try
	{
		::cereal::PortableBinaryInputArchive ar(ss);
		ar(p);
	}
	catch (...)
	{
		LOG_WARNING << "[Arena] Unpack CHECK_CHART_EXIST resp payload failed";
		return;
	}
	if (p.exist)
	{
		requestChartPendingExistCount++;
	}
	else if (requestChartPendingClientKey == "host")
	{
		createNotification((boost::format(i18n::c(i18nText::ARENA_REQUEST_FAILED_PLAYER_NO_CHART)) % c.name).str());
	}
	else
	{
		Client& cc = clients[requestChartPendingClientKey];

		ArenaMessageNotice n;
		n.messageIndex = ++c.sendMessageIndex;
		n.i18nIndex = i18nText::ARENA_REQUEST_FAILED_PLAYER_NO_CHART;
		n.format = ArenaMessageNotice::S1;
		n.s1 = c.name;

		auto payload = n.pack();
		cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
	}
}


void ArenaHost::handlePlayInit(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageClientPlayInit>(msg);

	Client& c = clients[clientKey];

	if (ArenaData.data.find(c.id) == ArenaData.data.end())
	{
		LOG_WARNING << "[Arena] Invalid player ID: " << c.id;
		return;
	}

	if (ArenaData.data.at(c.id).ruleset == nullptr)
	{
		LOG_WARNING << "[Arena] Player data not initialized. ID: " << c.id;
		return;
	}

	ArenaData.data.at(c.id).ruleset->unpackInit(pMsg->payload);

	for (auto& [k, cc] : clients)
	{
		if (k == clientKey) continue;

		auto n = std::make_shared<ArenaMessageHostPlayInit>();
		n->messageIndex = ++cc.sendMessageIndex;
		n->playerID = c.id;
		n->rulesetPayload = pMsg->payload;

		auto payload = n->pack();
		cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
		cc.addTaskWaitingForResponse(n->messageIndex, payload);
	}

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	c.serverSocket->async_send_to(boost::asio::buffer(*payload), c.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
}

void ArenaHost::handleFinishedLoading(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageClientFinishedLoading>(msg);

	Client& c = clients[clientKey];
	c.isLoadingFinished = true;
	c.playStartTimeMs = pMsg->playStartTimeMs;

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	c.serverSocket->async_send_to(boost::asio::buffer(*payload), c.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
}

void ArenaHost::handlePlayData(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageClientPlayData>(msg);

	Client& c = clients[clientKey];

	if (pMsg->messageIndex > c.recvMessageIndex)
	{
		if (ArenaData.data.find(c.id) == ArenaData.data.end())
		{
			LOG_WARNING << "[Arena] Invalid player ID: " << c.id;
			return;
		}

		if (ArenaData.data.at(c.id).ruleset == nullptr)
		{
			LOG_WARNING << "[Arena] Player data not initialized. ID: " << c.id;
			return;
		}

		ArenaData.data.at(c.id).ruleset->unpackFrame(pMsg->payload);
	}
	// no response
}

void ArenaHost::handleFinishedPlaying(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageClientFinishedPlaying>(msg);

	Client& c = clients[clientKey];
	c.isPlayingFinished = true;

	ArenaMessageResponse resp(*pMsg);

	resp.errorCode = 0;
	auto payload = resp.pack();
	c.serverSocket->async_send_to(boost::asio::buffer(*payload), c.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
}

void ArenaHost::handleFinishedResult(const std::string& clientKey, std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageClientFinishedResult>(msg);

	Client& c = clients[clientKey];
	c.isResultFinished = true;

	ArenaMessageResponse resp(*pMsg);

	resp.errorCode = 0;
	auto payload = resp.pack();
	c.serverSocket->async_send_to(boost::asio::buffer(*payload), c.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
}

void ArenaHost::update()
{
	Time now;

	// wait response timeout
	{
		std::shared_lock l(clientsMutex);
		for (auto& [k, cc] : clients)
		{
			std::set<int> taskHasResponse;
			{
				std::shared_lock l(cc.tasksWaitingForResponseMutex);
				for (auto& [msgID, task] : cc.tasksWaitingForResponse)
				{
					if (task.received)
					{
						taskHasResponse.insert(msgID);
					}
					else if ((now - task.t).norm() > 5000)
					{
						if (task.retryTimes > 3)
						{
							// client has dead. Removing
							cc.alive = false;
						}
						else
						{
							task.retryTimes++;
							task.t = now;
							cc.serverSocket->async_send_to(boost::asio::buffer(*task.sentMessage), cc.endpoint, std::bind(emptyHandleSend, task.sentMessage, std::placeholders::_1, std::placeholders::_2));
						}
					}
				}
			}
			{
				std::unique_lock l(cc.tasksWaitingForResponseMutex);
				for (auto& msgID : taskHasResponse)
				{
					cc.tasksWaitingForResponse.erase(msgID);
				}
			}
		}
	}

	// heartbeat
	{
		std::shared_lock l(clientsMutex);
		for (auto& [k, cc] : clients)
		{
			if ((now - cc.heartbeatRecvTime).norm() > 5000 && !cc.heartbeatPending)
			{
				cc.heartbeatPending = true;
				cc.heartbeatSendTime = Time();

				auto n = std::make_shared<ArenaMessageHeartbeat>();
				n->messageIndex = ++cc.sendMessageIndex;

				auto payload = n->pack();
				cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
				cc.addTaskWaitingForResponse(n->messageIndex, payload);
			}
			if ((now - cc.heartbeatRecvTime).norm() > 30000)
			{
				// client has dead. Removing
				cc.alive = false;
			}
		}
	}

	// remove dead clients
	std::set<std::string> clientsRemoving;
	if (!ArenaData.playing)
	{
		std::shared_lock l(clientsMutex);
		for (auto& [k, cc] : clients)
		{
			if (!cc.alive)
			{
				clientsRemoving.insert(k);
			}
		}
	}
	{
		std::unique_lock l(clientsMutex);
		for (auto& k : clientsRemoving)
		{
			Client& c = clients[k];
			for (int i = 0; i < ArenaData.getPlayerCount(); ++i)
			{
				if (c.id == ArenaData.getPlayerID(i))
				{
					if (!requestChartPending.empty() && requestChartPending == c.requestChartHash)
					{
						requestChartPendingExistCount--;
					}

					ArenaData.playerIDs.erase(ArenaData.playerIDs.begin() + i);
					ArenaData.data.erase(c.id);
				}
			}
			createNotification((boost::format(i18n::c(i18nText::ARENA_PLAYER_LEFT)) % c.name).str());
			clients.erase(k);
		}
	}

	// pending chart
	{
		std::shared_lock l(clientsMutex);
		if (!requestChartPending.empty() && requestChartPendingExistCount >= clients.size() + 1)
		{
			requestChartHash = requestChartPending;
			requestChartPendingExistCount = 0;
			requestChartPending.reset();

			std::string requestPlayerName;
			if (requestChartPendingClientKey == "host")
			{
				// host select chart
				hostRequestChartHash = requestChartHash;
				requestPlayerName = SystemData.playerName;
				ArenaData.ready = true;
			}
			else
			{
				// notice success
				auto& cc = clients[requestChartPendingClientKey];
				cc.requestChartHash = requestChartHash;
				requestPlayerName = cc.name;
				ArenaData.data[cc.id].ready = true;
			}
			ArenaData.remoteRequestedPlayer = requestPlayerName;
			ArenaData.remoteRequestedChart = requestChartHash;

			for (auto& [k, cc] : clients)
			{
				auto n = std::make_shared<ArenaMessageHostRequestChart>();
				n->messageIndex = ++cc.sendMessageIndex;
				n->requestPlayerName = requestPlayerName;
				n->chartHashMD5String = requestChartHash.hexdigest();

				auto payload = n->pack();
				cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
				cc.addTaskWaitingForResponse(n->messageIndex, payload);
			}

			// inform clients that requester is ready
			decltype(std::declval<ArenaMessageHostReadyStat>().ready) ready;
			ready[0] = requestChartPendingClientKey == "host";
			for (auto& [k, cc] : clients)
			{
				ready[cc.id] = requestChartPendingClientKey == k;
			}
			for (auto& [k, cc] : clients)
			{
				auto n = std::make_shared<ArenaMessageHostReadyStat>();
				n->messageIndex = ++cc.sendMessageIndex;
				n->ready = ready;

				auto payload = n->pack();
				cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

				cc.addTaskWaitingForResponse(n->messageIndex, payload);
			}
		}
	}

	// start playing 
	if (!ArenaData.isArenaReady && !clients.empty() && !hostRequestChartHash.empty())
	{
		std::shared_lock l(clientsMutex);
		int ready = 0;
		for (auto& [k, cc] : clients)
		{
			if (cc.requestChartHash == hostRequestChartHash)
				++ready;
		}
		if (ready == clients.size())
		{
			// host decide

			LOG_WARNING << "[Arena] Decide";

			static std::random_device rd;
			ArenaData.randomSeed = ((uint64_t)rd() << 32) | rd();

			startPlaying();
		}
	}

	// host init ruleset
	if (!ArenaData.playing && isCreatedRuleset())
	{
		std::shared_lock l(clientsMutex);
		for (auto& [k, cc] : clients)
		{
			auto n = std::make_shared<ArenaMessageHostPlayInit>();
			n->messageIndex = ++cc.sendMessageIndex;
			n->playerID = 0;
			n->rulesetPayload = vRulesetNetwork::packInit(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);

			auto payload = n->pack();
			cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
			cc.addTaskWaitingForResponse(n->messageIndex, payload);
		}
	}

	// load complete check
	if (!ArenaData.playing && isLoadingFinished())
	{
		std::shared_lock l(clientsMutex);
		int ready = 0;
		for (auto& [k, cc] : clients)
		{
			if (cc.isLoadingFinished)
				ready++;
		}
		if (ready == clients.size())
		{
			LOG_WARNING << "[Arena] Start playing";

			// ping compensate
			int playStartTimeMs = this->playStartTimeMs;
			for (auto& [k, cc] : clients)
			{
				playStartTimeMs = std::max(playStartTimeMs, cc.playStartTimeMs + cc.ping / 2);
			}
			ArenaData.playStartTimeMs = playStartTimeMs;

			// host ready
			for (auto& [k, cc] : clients)
			{
				auto n = std::make_shared<ArenaMessageHostFinishedLoading>();
				n->messageIndex = ++cc.sendMessageIndex;
				n->playStartTimeMs = playStartTimeMs - cc.ping / 2;

				auto payload = n->pack();
				cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
				cc.addTaskWaitingForResponse(n->messageIndex, payload);
			}

			ArenaData.startPlaying();
		}
	}

	// gamedata
	if (ArenaData.playing)
	{
		ArenaData.updateGlobals();

		std::shared_lock l(clientsMutex);
		for (auto& [k, cc] : clients)
		{
			auto n = std::make_shared<ArenaMessageHostPlayData>();
			n->messageIndex = ++cc.sendMessageIndex;

			// pack host data
			n->payload[0] = vRulesetNetwork::packFrame(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);

			// pack clients data
			for (auto& [kp, cp] : clients)
			{
				if (k == kp) continue;
				n->payload[cp.id] = vRulesetNetwork::packFrame(ArenaData.data[cp.id].ruleset);
			}

			auto payload = n->pack();
			cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
			// no resp
		}
	}

	// finish check
	if (ArenaData.playing && isPlayingFinished())
	{
		std::shared_lock l(clientsMutex);
		int finished = 0;
		for (auto& [k, cc] : clients)
		{
			if (cc.isPlayingFinished)
				++finished;
		}
		if (finished == clients.size())
		{
			ArenaData.playingFinished = true;
			for (auto& [k, cc] : clients)
			{
				auto n = std::make_shared<ArenaMessageHostFinishedPlaying>();
				n->messageIndex = ++cc.sendMessageIndex;

				auto payload = n->pack();
				cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
				cc.addTaskWaitingForResponse(n->messageIndex, payload);
			}
		}
	}

	// result check
	if (isResultFinished())
	{
		std::shared_lock l(clientsMutex);
		int finished = 0;
		for (auto& [k, cc] : clients)
		{
			if (cc.isResultFinished)
				++finished;
		}
		if (finished == clients.size())
		{
			ArenaData.isArenaReady = false;
			ArenaData.stopPlaying();

			for (auto& [k, cc] : clients)
			{
				auto n = std::make_shared<ArenaMessageHostFinishedResult>();
				n->messageIndex = ++cc.sendMessageIndex;

				auto payload = n->pack();
				cc.serverSocket->async_send_to(boost::asio::buffer(*payload), cc.endpoint, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
				cc.addTaskWaitingForResponse(n->messageIndex, payload);

				cc.requestChartHash.reset();
				cc.isLoadingFinished = false;
				cc.isPlayingFinished = false;
				cc.isResultFinished = false;
			}

			requestChartHash.reset();
			hostRequestChartHash.reset();
			_isLoadingFinished = false;
			_isCreatedRuleset = false;
			_isPlayingFinished = false;
			_isResultFinished = false;
		}
	}
}

}
