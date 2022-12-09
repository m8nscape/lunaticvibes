#include "arena_client.h"
#include "arena_data.h"
#include "arena_internal.h"
#include "cereal/archives/portable_binary.hpp"
#include "game/scene/scene_context.h"
#include "game/runtime/i18n.h"
#include "re2/re2.h"

std::shared_ptr<ArenaClient> g_pArenaClient = nullptr;


ArenaClient::~ArenaClient()
{
	if (gArenaData.isOnline())
	{
		leaveLobby();
	}
	if (socket)
	{
		ioc.stop();
		listen.wait();
		socket.reset();
	}
}

static void emptyHandleSend(std::shared_ptr<std::vector<unsigned char>> message, const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
{
	if (error)
	{
		LOG_WARNING << error.message();
	}
}


std::vector<ArenaLobbyInfo> ArenaClient::seekLobby()
{
	// TBD
	return {};
}


bool ArenaClient::joinLobby(const std::string& address)
{
	if (gArenaData.isOnline())
	{
		LOG_WARNING << "[Arena] Join failed! Please leave your current lobby first.";
		createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_IN_LOBBY));
		return false;
	}

	LOG_INFO << "[Arena] Resolving address " << address;
	udp::resolver resolver(ioc);
	try
	{
		if (auto v4addr = resolver.resolve(udp::v4(), address, ""); !v4addr.empty())
		{
			server = *v4addr.begin();
			server.port(ARENA_HOST_PORT);
		}
		else if (auto v6addr = resolver.resolve(udp::v6(), address, ""); v6addr.empty())
		{
			server = *v6addr.begin();
			server.port(ARENA_HOST_PORT);
		}
		else
		{
			createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_ADDRESS_ERROR));
			LOG_WARNING << "[Arena] Address error";
			return false;
		}
	}
	catch (...)
	{
		createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_ADDRESS_ERROR));
		LOG_WARNING << "[Arena] Address error";
		return false;
	}

	LOG_INFO << "[Arena] Address: " << server.address().to_string();
	LOG_INFO << "[Arena] Port: " << server.port();

	try
	{
		if (server.address().is_v4())
		{
			socket = std::make_shared<udp::socket>(ioc);
			socket->open(udp::v4());
		}
		else if (server.address().is_v6())
		{
			socket = std::make_shared<udp::socket>(ioc);
			socket->open(udp::v6());
		}
		else
		{
			LOG_WARNING << "[Arena] ???";
			assert(false);
			return false;
		}
	}
	catch (std::exception& e)
	{
		LOG_WARNING << "[Arena] Open socket failed: " << to_utf8(e.what(), eFileEncoding::LATIN1);
		return false;
	}

	LOG_INFO << "[Arena] Joining lobby...";

	auto n = std::make_shared<ArenaMessageJoinLobby>();
	n->messageIndex = ++sendMessageIndex;
	n->playerName = State::get(IndexText::PLAYER_NAME);

	auto payload = n->pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
	addTaskWaitingForResponse(n->messageIndex, payload);

	gArenaData.online = true;
	asyncRecv();
	listen = std::async(std::launch::async, [&] { ioc.run(); });

	// FIXME what is this code
	for (int i = 0; i < 50; ++i)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(100ms);

		if (joinLobbyErrorCode >= 0)
		{
			if (joinLobbyErrorCode > 0)
			{
				switch (joinLobbyErrorCode)
				{
				case 1: createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_FULL)); break;
				case 2: createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_IN_GAME)); break;
				case 255: createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_DUPLICATE_CLIENT)); break;
				}
				LOG_WARNING << "[Arena] Join failed: " << joinLobbyErrorCode; break;
				return false;
			}

			State::set(IndexText::ARENA_LOBBY_STATUS, "ARENA LOBBY");
			return true;
		}
	}

	createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_TIMEOUT));
	LOG_WARNING << "[Arena] Join failed: Timeout 5s";
	return false;
}

void ArenaClient::leaveLobby()
{
	if (!gArenaData.isOnline()) return;

	if (!gArenaData.isExpired())
	{
		auto n = std::make_shared<ArenaMessageLeaveLobby>();
		n->messageIndex = ++sendMessageIndex;

		auto payload = n->pack();
		socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
		// no response
	}

	ioc.stop();
	listen.wait();
	socket.reset();

	gArenaData.expired = true;
}

void ArenaClient::requestChart(const HashMD5& reqChart)
{
	requestChartHash = reqChart;
	auto n = std::make_shared<ArenaMessageRequestChart>();
	n->messageIndex = ++sendMessageIndex;
	n->chartHashMD5String = reqChart.hexdigest();
	
	auto payload = n->pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	addTaskWaitingForResponse(n->messageIndex, payload);
}

void ArenaClient::setLoadingFinished(int playStartTimeMs)
{
	if (_isLoadingFinished) return;

	_isLoadingFinished = true;

	auto n = std::make_shared<ArenaMessageClientFinishedLoading>();
	n->messageIndex = ++sendMessageIndex;
	n->playStartTimeMs = playStartTimeMs;

	auto payload = n->pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	addTaskWaitingForResponse(n->messageIndex, payload);
}

void ArenaClient::setCreatedRuleset()
{
	if (_isCreatedRuleset) return;

	_isCreatedRuleset = true;

	auto n = std::make_shared<ArenaMessageClientPlayInit>();
	n->messageIndex = ++sendMessageIndex;
	n->payload = vRulesetNetwork::packInit(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]);

	auto payload = n->pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	addTaskWaitingForResponse(n->messageIndex, payload);
}

void ArenaClient::setPlayingFinished()
{
	if (_isPlayingFinished) return;

	_isPlayingFinished = true;

	auto n = std::make_shared<ArenaMessageClientFinishedPlaying>();
	n->messageIndex = ++sendMessageIndex;

	auto payload = n->pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	addTaskWaitingForResponse(n->messageIndex, payload);
}

void ArenaClient::setResultFinished()
{
	if (_isResultFinished) return;

	_isResultFinished = true;

	auto n = std::make_shared<ArenaMessageClientFinishedResult>();
	n->messageIndex = ++sendMessageIndex;

	auto payload = n->pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	addTaskWaitingForResponse(n->messageIndex, payload);
}

void ArenaClient::asyncRecv()
{
	socket->async_receive_from(boost::asio::buffer(recv_buf), remote_endpoint, 
		std::bind(&ArenaClient::handleRecv, this, std::placeholders::_1, std::placeholders::_2));
}

void ArenaClient::handleRecv(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		if (error == boost::asio::error::operation_aborted)
		{
			LOG_INFO << "[Arena] socket closed";
		}
		else
		{
			LOG_WARNING << "[Arena] socket exception: " << error.message();
		}

		ioc.stop();
		listen.wait();
		socket.reset();

		gArenaData.expired = true;
		return;
	}
	
	if (bytes_transferred > recv_buf.size())
	{
		LOG_DEBUG << "[Arena] Ignored huge packet (" << bytes_transferred << ")";
		return;
	}
	handleRequest(recv_buf.data(), bytes_transferred);

	if (gArenaData.isOnline() && !gArenaData.isExpired())
		asyncRecv();
}

void ArenaClient::handleRequest(const unsigned char* recv_buf, size_t recv_buf_len)
{
	auto pMsg = ArenaMessage::unpack(recv_buf, recv_buf_len);
	if (pMsg)
	{
		switch (pMsg->type)
		{
		case Arena::RESPONSE:                  handleResponse(pMsg); break;
		case Arena::HEARTBEAT:                 handleHeartbeat(pMsg); break;
		case Arena::NOTICE:              	   handleNotice(pMsg); break;
		case Arena::DISBAND_LOBBY:		       handleDisbandLobby(pMsg); break;
		case Arena::PLAYER_JOINED_LOBBY:       handlePlayerJoined(pMsg); break;
		case Arena::PLAYER_LEFT_LOBBY:		   handlePlayerLeft(pMsg); break;
		case Arena::CHECK_CHART_EXIST:		   handleCheckChartExist(pMsg); break;
		case Arena::HOST_REQUEST_CHART:        handleHostRequestChart(pMsg); break; 
		case Arena::HOST_START_PLAYING:  	   handleHostStartPlaying(pMsg); break;
		case Arena::HOST_PLAY_INIT:			   handleHostPlayInit(pMsg); break;
		case Arena::HOST_FINISHED_LOADING:	   handleHostFinishedLoading(pMsg); break;
		case Arena::HOST_PLAYDATA:			   handleHostPlayData(pMsg); break;
		case Arena::HOST_FINISHED_PLAYING:	   handleHostFinishedPlaying(pMsg); break;
		case Arena::HOST_FINISHED_RESULT:	   handleHostFinishedResult (pMsg); break;
		}
	}
	recvMessageIndex = std::max(recvMessageIndex, pMsg->messageIndex);
}

void ArenaClient::handleResponse(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageResponse>(msg);

	std::shared_lock l(tasksWaitingForResponseMutex);
	if (tasksWaitingForResponse.find(pMsg->messageIndex) == tasksWaitingForResponse.end())
	{
		LOG_WARNING << "[Arena] Invalid req message index, or duplicate resp msg";
		return;
	}

	tasksWaitingForResponse[pMsg->messageIndex].received = true;

	if (pMsg->errorCode != 0)
	{
		LOG_WARNING << "[Arena] Req type " << pMsg->reqType << " error with code " << pMsg->errorCode;
		return;
	}

	switch (pMsg->reqType)
	{
	case Arena::JOIN_LOBBY: handleJoinLobbyResp(pMsg); break;
	}
}

void ArenaClient::handleJoinLobbyResp(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageResponse>(msg);

	joinLobbyErrorCode = pMsg->errorCode;
	if (pMsg->errorCode == 0)
	{
		ArenaJoinLobbyResp p;
		std::stringstream ss;
		ss.write((char*)&pMsg->payload[0], pMsg->payload.size());
		try
		{
			cereal::PortableBinaryInputArchive ar(ss);
			ar(p);
		}
		catch (...)
		{
			LOG_WARNING << "[Arena] Unpack JOIN_LOBBY resp payload failed";
			return;
		}

		playerID = p.playerID;
		for (const auto& [id, name] : p.playerName)
		{
			if (id == playerID) continue;
			gArenaData.playerIDs.push_back(id);
			gArenaData.data[id].name = name;
		}
		gArenaData.updateTexts();
	}
}

void ArenaClient::handleHeartbeat(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageHeartbeat>(msg);

	heartbeatTime = Time();

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
}

void ArenaClient::handleNotice(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageNotice>(msg);

	static const LazyRE2 reS1{ R"(.*%s.*)" };
	static const LazyRE2 reS2{ R"(.*%s.*%s.*)" };
	static const LazyRE2 reD1{ R"(.*%d.*)" };
	static const LazyRE2 reD2{ R"(.*%d.*%d.*)" };
	switch (pMsg->format)
	{
	case ArenaMessageNotice::PLAIN:
		createNotification(i18n::s(pMsg->i18nIndex));
		break;

	case ArenaMessageNotice::S1:
		if (RE2::FullMatch(i18n::s(pMsg->i18nIndex), *reS1))
			createNotification((boost::format(i18n::s(pMsg->i18nIndex)) % pMsg->s1).str());
		break;

	case ArenaMessageNotice::S2:
		if (RE2::FullMatch(i18n::s(pMsg->i18nIndex), *reS2))
			createNotification((boost::format(i18n::s(pMsg->i18nIndex)) % pMsg->s1 % pMsg->s2).str());
		break;

	case ArenaMessageNotice::D1:
		if (RE2::FullMatch(i18n::s(pMsg->i18nIndex), *reD1))
			createNotification((boost::format(i18n::s(pMsg->i18nIndex)) % pMsg->d1).str());
		break;

	case ArenaMessageNotice::D2:
		if (RE2::FullMatch(i18n::s(pMsg->i18nIndex), *reD2))
			createNotification((boost::format(i18n::s(pMsg->i18nIndex)) % pMsg->d1 % pMsg->d2).str());
		break;
	}

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
}

void ArenaClient::handleDisbandLobby(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageDisbandLobby>(msg);

	createNotification(i18n::s(i18nText::ARENA_LOBBY_DISBANDED));
	LOG_WARNING << "[Arena] Host disbanded";

	socket->close();
	socket.reset();

	gArenaData.expired = true;
}


void ArenaClient::handlePlayerJoined(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessagePlayerJoinedLobby>(msg);

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	if (gArenaData.data.find(pMsg->playerID) != gArenaData.data.end())
	{
		LOG_WARNING << "[Arena] player ID " << pMsg->playerID << " already exists, removing old";

		for (int i = 0; i < gArenaData.getPlayerCount(); ++i)
		{
			if (gArenaData.playerIDs[i] == pMsg->playerID)
			{
				gArenaData.playerIDs.erase(gArenaData.playerIDs.begin() + i);
				gArenaData.data.erase(i);
				break;
			}
		}
	}
	
	gArenaData.data[pMsg->playerID].name = pMsg->playerName;
	gArenaData.playerIDs.push_back(pMsg->playerID);

	createNotification((boost::format(i18n::c(i18nText::ARENA_PLAYER_JOINED)) % pMsg->playerName).str());
}

void ArenaClient::handlePlayerLeft(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessagePlayerLeftLobby>(msg);

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	if (gArenaData.data.find(pMsg->playerID) == gArenaData.data.end())
	{
		LOG_WARNING << "[Arena] player ID " << pMsg->playerID << " does not exist";
	}
	else
	{
		for (int i = 0; i < gArenaData.getPlayerCount(); ++i)
		{
			if (gArenaData.playerIDs[i] == pMsg->playerID)
			{
				createNotification((boost::format(i18n::c(i18nText::ARENA_PLAYER_LEFT)) % gArenaData.data[i].name).str());
				gArenaData.playerIDs.erase(gArenaData.playerIDs.begin() + i);
				gArenaData.data.erase(i);
				break;
			}
		}
	}
}

void ArenaClient::handleCheckChartExist(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageCheckChartExist>(msg);

	ArenaMessageResponse resp(*pMsg);

	ArenaCheckChartExistResp subPayload;
	subPayload.exist = !g_pSongDB->findChartByHash(pMsg->chartHashMD5String).empty();
	std::stringstream ss;
	try
	{
		cereal::PortableBinaryOutputArchive ar(ss);
		ar(subPayload);
	}
	catch (...)
	{
	}
	size_t length = ss.tellp();
	if (length == 0)
	{
		LOG_WARNING << "[Arena] Pack CHECK_CHART_EXIST resp payload failed";
		return;
	}
	resp.payload.resize(length);
	ss.read((char*)&resp.payload[0], length);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

}

void ArenaClient::handleHostRequestChart(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageHostRequestChart>(msg);

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	auto hash = HashMD5(pMsg->chartHashMD5String);

	// select chart
	gSelectContext.remoteRequestedPlayer = pMsg->requestPlayerName;
	gSelectContext.remoteRequestedChart = hash;
}

void ArenaClient::handleHostStartPlaying(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageHostStartPlaying>(msg);

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	gArenaData.initPlaying(eRuleset(pMsg->rulesetType));
	gSelectContext.isArenaReady = true;
}

void ArenaClient::handleHostPlayInit(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageHostPlayInit>(msg);

	ArenaMessageResponse resp(*pMsg);

	if (gArenaData.data.find(pMsg->playerID) != gArenaData.data.end())
	{
		gArenaData.data[pMsg->playerID].ruleset->unpackInit(pMsg->rulesetPayload);
	}
	else
	{
		resp.errorCode = 2;
	}

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

}

void ArenaClient::handleHostFinishedLoading(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageHostFinishedLoading>(msg);

	gArenaData.playStartTimeMs = pMsg->playStartTimeMs;
	gArenaData.startPlaying();

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
}

void ArenaClient::handleHostPlayData(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageHostPlayData>(msg);

	if (pMsg->messageIndex > recvMessageIndex)
	{
		for (auto& [id, payload] : pMsg->payload)
		{
			if (gArenaData.data.find(id) == gArenaData.data.end()) continue;
			gArenaData.data[id].ruleset->unpackFrame(payload);
		}
	}
}

void ArenaClient::handleHostFinishedPlaying(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageHostFinishedPlaying>(msg);

	gArenaData.playingFinished = true;

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

}

void ArenaClient::handleHostFinishedResult(std::shared_ptr<ArenaMessage> msg)
{
	auto pMsg = std::static_pointer_cast<ArenaMessageHostFinishedResult>(msg);

	ArenaMessageResponse resp(*pMsg);

	auto payload = resp.pack();
	socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));

	gSelectContext.isArenaReady = false;
	gArenaData.stopPlaying();


	_isLoadingFinished = false;
	_isCreatedRuleset = false;
	_isPlayingFinished = false;
	_isResultFinished = false;
}

void ArenaClient::update()
{
	Time now;

	bool alive = true;

	// wait response timeout
	{
		std::set<int> taskHasResponse;
		{
			std::shared_lock l(tasksWaitingForResponseMutex);
			for (auto& [msgID, task] : tasksWaitingForResponse)
			{
				if (task.received)
				{
					taskHasResponse.insert(msgID);
				}
				else if ((now - task.t).norm() > 5000)
				{
					if (task.retryTimes > 3)
					{
						// server has dead. Removing
						alive = false;
						break;
					}
					else
					{
						task.retryTimes++;
						task.t = now;
						socket->async_send_to(boost::asio::buffer(*task.sentMessage), server, std::bind(emptyHandleSend, task.sentMessage, std::placeholders::_1, std::placeholders::_2));
					}
				}
			}
		}
		{
			std::unique_lock l(tasksWaitingForResponseMutex);
			for (auto& msgID : taskHasResponse)
			{
				tasksWaitingForResponse.erase(msgID);
			}
		}
	}

	// heartbeat
	if ((now - heartbeatTime).norm() > 30000)
	{
		alive = false;
	}

	if (alive)
	{
		if (gArenaData.playing)
		{
			gArenaData.updateGlobals();

			// send gamedata
			auto n = std::make_shared<ArenaMessageClientPlayData>();
			n->messageIndex = ++sendMessageIndex;
			n->payload = vRulesetNetwork::packFrame(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]);

			auto payload = n->pack();
			socket->async_send_to(boost::asio::buffer(*payload), server, std::bind(emptyHandleSend, payload, std::placeholders::_1, std::placeholders::_2));
			// no resp
		}
	}
	else
	{
		if (!gArenaData.playing)
		{
			leaveLobby();
		}
		createNotification(i18n::s(i18nText::ARENA_LEAVE_TIMEOUT));
	}

}