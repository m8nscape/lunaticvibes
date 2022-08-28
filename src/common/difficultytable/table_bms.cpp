#include "table_bms.h"
#include "../entry/entry_song.h"
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "tao/json.hpp"
#include "tidy.h"
#include "tidybuffio.h"
#include "re2/re2.h"


namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;	// from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

enum class GetResult
{
	OK,
	ERR_SNI_HOSTNAME,
	ERR_RESOLVE,
	ERR_CONNECT,
	ERR_HANDSHAKE,
	ERR_WRITE,
	ERR_READ,
	ERR_TIMEOUT
};

class session : public std::enable_shared_from_this<session>
{
	// This class is mostly taken from Boost sample, with some modifications
	// Original author: Vinnie Falco (vinnie dot falco at gmail dot com))

	tcp::resolver resolver_;
	beast::tcp_stream stream_;
	beast::ssl_stream<beast::tcp_stream> ssl_stream_;
	beast::flat_buffer buffer_; // (Must persist between reads)
	http::request<http::string_body> req_;
	http::response<http::string_body> res_;

	std::function<void(GetResult, const std::string&)> callback;
	std::string target;
	std::string body;

	bool ssl = false;

public:
	// Objects are constructed with a strand to
	// ensure that handlers do not execute concurrently.
	explicit
		session(net::io_context& ioc, ssl::context& sslctx)
		: resolver_(net::make_strand(ioc))
		, stream_(net::make_strand(ioc))
		, ssl_stream_(net::make_strand(ioc), sslctx)
	{
	}

	// Start the asynchronous operation
	void
		GET(
			const std::string& url,
			std::function<void(GetResult, const std::string&)> cb,
			int version = 10)
	{
		callback = cb;

		std::string https, host, port, target;
		static LazyRE2 regexURL{ R"(http(s?)\:\/\/(.+?)(?:\:([\d]{1,5}))?(\/(?:.*)*))" };
		if (RE2::FullMatch(url, *regexURL, &https, &host, &port, &target))
		{
			ssl = !https.empty();
			if (port.empty())
			{
				port = ssl ? "443" : "80";
			}
		}
		else
		{
			callback(GetResult::ERR_RESOLVE, body);
			return;
		}

		this->target = "GET "s + target;

		if (ssl)
		{
			// Set SNI Hostname (many hosts need this to handshake successfully)
			if (!SSL_set_tlsext_host_name(ssl_stream_.native_handle(), host.c_str()))
			{
				beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
				callback(GetResult::ERR_SNI_HOSTNAME, body);
				return;
			}
		}

		// Set up an HTTP GET request message
		req_.version(version);
		req_.method(http::verb::get);
		req_.target(target);
		req_.set(http::field::host, host);
		req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		// Look up the domain name
		resolver_.async_resolve(
			host,
			port,
			std::bind(
				&session::on_resolve,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void
		on_resolve(
			boost::system::error_code ec,
			tcp::resolver::results_type results)
	{
		if (ec)
		{
			callback(GetResult::ERR_RESOLVE, body);
			return;
		}

		if (ssl)
		{
			// Set a timeout on the operation
			beast::get_lowest_layer(ssl_stream_).expires_after(std::chrono::seconds(5));

			// Make the connection on the IP address we get from a lookup
			beast::get_lowest_layer(ssl_stream_).async_connect(
				results,
				std::bind(
					&session::on_connect_ssl,
					shared_from_this(),
					std::placeholders::_1));
		}
		else
		{
			// Set a timeout on the operation
			stream_.expires_after(std::chrono::seconds(5));

			// Make the connection on the IP address we get from a lookup
			stream_.async_connect(
				results,
				std::bind(
					&session::on_connect,
					shared_from_this(),
					std::placeholders::_1));
		}
	}

	void
		on_connect_ssl(boost::system::error_code ec)
	{
		beast::get_lowest_layer(ssl_stream_).expires_never();
		if (ec)
		{
			callback((ec == boost::system::errc::stream_timeout) ? GetResult::ERR_TIMEOUT : GetResult::ERR_CONNECT, body);
			return;
		}

		// Perform the SSL handshake
		ssl_stream_.async_handshake(
			ssl::stream_base::client,
			beast::bind_front_handler(
				&session::on_handshake,
				shared_from_this()));
	}

	void
		on_handshake(beast::error_code ec)
	{
		if (ec)
		{
			callback(GetResult::ERR_HANDSHAKE, body);
			return;
		}

		// Set a timeout on the operation
		beast::get_lowest_layer(ssl_stream_).expires_after(std::chrono::seconds(5));

		// Send the HTTP request to the remote host
		http::async_write(ssl_stream_, req_,
			beast::bind_front_handler(
				&session::on_write_ssl,
				shared_from_this()));
	}

	void
		on_connect(boost::system::error_code ec)
	{
		stream_.expires_never();
		if (ec)
		{
			callback((ec == boost::system::errc::stream_timeout) ? GetResult::ERR_TIMEOUT : GetResult::ERR_CONNECT, body);
			return;
		}

		// Set a timeout on the operation
		stream_.expires_after(std::chrono::seconds(5));

		// Send the HTTP request to the remote host
		http::async_write(stream_, req_,
			std::bind(
				&session::on_write,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void
		on_write_ssl(
			boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		beast::get_lowest_layer(ssl_stream_).expires_never();
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			callback((ec == boost::system::errc::stream_timeout) ? GetResult::ERR_TIMEOUT : GetResult::ERR_WRITE, body);
			return;
		}

		// Set a timeout on the operation
		beast::get_lowest_layer(ssl_stream_).expires_after(std::chrono::seconds(10));

		// Receive the HTTP response
		http::async_read(ssl_stream_, buffer_, res_,
			std::bind(
				&session::on_read,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void
		on_write(
			boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		stream_.expires_never();
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			callback((ec == boost::system::errc::stream_timeout) ? GetResult::ERR_TIMEOUT : GetResult::ERR_WRITE, body);
			return;
		}

		// Set a timeout on the operation
		stream_.expires_after(std::chrono::seconds(10));

		// Receive the HTTP response
		http::async_read(stream_, buffer_, res_,
			std::bind(
				&session::on_read,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void
		on_read_ssl(
			boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		beast::get_lowest_layer(ssl_stream_).expires_never();
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			callback((ec == boost::system::errc::stream_timeout) ? GetResult::ERR_TIMEOUT : GetResult::ERR_READ, body);
			return;
		}

		body = res_.body();

		// callback
		callback(GetResult::OK, body);

		// Set a timeout on the operation
		beast::get_lowest_layer(ssl_stream_).expires_after(std::chrono::seconds(5));

		// If we get here then the connection is closed gracefully
		ssl_stream_.async_shutdown(
			beast::bind_front_handler(
				&session::on_shutdown_ssl,
				shared_from_this()));
	}

	void
		on_read(
			boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		stream_.expires_never();
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			callback((ec == boost::system::errc::stream_timeout) ? GetResult::ERR_TIMEOUT : GetResult::ERR_READ, body);
			return;
		}

		body = res_.body();

		// callback
		callback(GetResult::OK, body);

		// Gracefully close the socket
		stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes so don't bother reporting it.
		if (ec && ec != boost::system::errc::not_connected)
		{
			return;
		}

		// If we get here then the connection is closed gracefully
	}

	void
		on_shutdown_ssl(beast::error_code ec)
	{
		beast::get_lowest_layer(ssl_stream_).expires_never();
		if (ec == net::error::eof)
		{
			// Rationale:
			// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
			ec = {};
		}
		if (ec)
		{
		}

		// If we get here then the connection is closed gracefully
	}
};

void DifficultyTableBMS::updateFromUrl(std::function<void(DifficultyTable::UpdateResult)> finishedCallback)
{
	if (!strEqual(url.substr(0, 7), "http://", true) && strEqual(url.substr(0, 8), "https://", true) ||
		url.substr(8).find('/') == url.npos)
	{
		LOG_ERROR << "[TableBMS] URL error: " << url;
		finishedCallback(UpdateResult::WEB_PATH_ERROR);
		return;
	}
	std::string remotePath = url.substr(0, url.find_last_of('/') + 1);
	
	std::string webUrl, headerUrl, dataUrl;

	// parse HTML
	webUrl = url;
	if (!strEqual(webUrl.substr(webUrl.length() - 5), ".json", true))
	{
		std::string headerFileName;

		net::io_context ioc;
		ssl::context ctx{ ssl::context::tlsv12_client };
		ctx.set_verify_mode(ssl::verify_none);	// Disable verify since table is not so sensitive
		std::make_shared<session>(ioc, ctx)->GET(webUrl, [this, &headerFileName, &remotePath, finishedCallback](GetResult result, const std::string& body)
			{
				if (result == GetResult::OK)
				{
					TidyDoc tdoc = tidyCreate();
					tidyOptSetBool(tdoc, TidyXmlOut, yes);
					tidyOptSetBool(tdoc, TidyForceOutput, yes);
					if (tidyParseString(tdoc, body.c_str()) >= 0 &&
						tidyCleanAndRepair(tdoc) >= 0)
					{
						char* output = new char[16384];
						uint outputSize = 0;

						if (int rc = tidySaveString(tdoc, output, &outputSize); (rc == ENOMEM || outputSize > sizeof(output)))
						{
							delete[] output;
							output = new char[outputSize];
						}
						tidySaveString(tdoc, output, &outputSize);

						namespace pt = boost::property_tree;

						pt::ptree tree;
						std::stringstream ss(std::string(output, outputSize));
						pt::read_xml(ss, tree);
						for (auto& [name, value] : tree.get_child("html.head"))
						{
							bool isBmstable = false;
							if (name == "meta")
							{
								for (auto& [metaName, metaValue] : value.get_child("<xmlattr>"))
								{
									if (strEqual(metaName, "name") && strEqual(metaValue.data(), "bmstable"))
									{
										isBmstable = true;
										break;
									}
								}
								if (isBmstable)
								{
									for (auto& [metaName, metaValue] : value.get_child("<xmlattr>"))
									{
										if (strEqual(metaName, "content"))
										{
											headerFileName = metaValue.data();
											break;
										}
									}
								}
							}
							if (!headerFileName.empty()) break;
						}

						delete[] output;
					}
					tidyRelease(tdoc);
				}
				else
				{
					switch (result)
					{
					case GetResult::ERR_SNI_HOSTNAME:
					case GetResult::ERR_RESOLVE:	  finishedCallback(DifficultyTable::UpdateResult::WEB_PATH_ERROR); break;
					case GetResult::ERR_CONNECT:
					case GetResult::ERR_HANDSHAKE:
					case GetResult::ERR_WRITE:
					case GetResult::ERR_READ:		  finishedCallback(DifficultyTable::UpdateResult::WEB_CONNECT_ERR); break;
					case GetResult::ERR_TIMEOUT:	  finishedCallback(DifficultyTable::UpdateResult::WEB_TIMEOUT); break;
					}
				}
			});
		ioc.run();

		if (!headerFileName.empty())
		{
			if (strEqual(headerFileName.substr(0, 7), "http://", true) || strEqual(headerFileName.substr(0, 8), "https://", true))
				headerUrl = headerFileName;
			else
				headerUrl = remotePath + headerFileName;
		}
		else
		{
			finishedCallback(UpdateResult::WEB_PARSE_FAILED);
			return;
		}
	}
	else
	{
		headerUrl = webUrl;
	}

	// parse Header
	if (!headerUrl.empty())
	{
		std::string dataFileName;

		net::io_context ioc;
		ssl::context ctx{ ssl::context::tlsv12_client };
		ctx.set_verify_mode(ssl::verify_none);	// Disable verify since table is not so sensitive
		std::make_shared<session>(ioc, ctx)->GET(headerUrl, [this, &dataFileName, &remotePath, finishedCallback](GetResult result, const std::string& body)
			{
				if (result == GetResult::OK)
				{
					tao::json::value header = tao::json::from_string(body);
					try
					{
						name = header["name"].get_string();
						symbol = header["symbol"].get_string();
						dataFileName = header["data_url"].get_string();
					}
					catch (std::bad_variant_access& e)
					{

					}

					if (!body.empty())
					{
						auto tablePath = getFolderPath();
						if (!fs::exists(tablePath)) fs::create_directories(tablePath);
						std::ofstream ofs(tablePath / "header.json", std::ios_base::binary);
						ofs << body;
						ofs.close();
					}
				}
				else
				{
					switch (result)
					{
					case GetResult::ERR_SNI_HOSTNAME:
					case GetResult::ERR_RESOLVE:	  finishedCallback(DifficultyTable::UpdateResult::HEADER_PATH_ERROR); break;
					case GetResult::ERR_CONNECT:
					case GetResult::ERR_HANDSHAKE:
					case GetResult::ERR_WRITE:
					case GetResult::ERR_READ:		  finishedCallback(DifficultyTable::UpdateResult::HEADER_CONNECT_ERR); break;
					case GetResult::ERR_TIMEOUT:	  finishedCallback(DifficultyTable::UpdateResult::HEADER_TIMEOUT); break;
					}
				}
			});
		ioc.run();

		if (!dataFileName.empty() && !name.empty())
		{
			if (strEqual(dataFileName.substr(0, 7), "http://", true) || strEqual(dataFileName.substr(0, 8), "https://", true))
				dataUrl = dataFileName;
			else
				dataUrl = remotePath + dataFileName;
		}
		else
		{
			finishedCallback(UpdateResult::HEADER_PARSE_FAILED);
			return;
		}
	}

	// parse Data
	if (!dataUrl.empty())
	{
		net::io_context ioc;
		ssl::context ctx{ ssl::context::tlsv12_client };
		ctx.set_verify_mode(ssl::verify_none);	// Disable verify since table is not so sensitive
		std::make_shared<session>(ioc, ctx)->GET(dataUrl, [this, &remotePath, finishedCallback](GetResult result, const std::string& body)
			{
				if (result == GetResult::OK)
				{
					std::vector<tao::json::value> header = tao::json::from_string(body).get_array();
					for (auto& entry : header)
					{
						std::string level, md5, name;
						try
						{
							switch (entry["level"].type())
							{
							case tao::json::type::UNSIGNED:    level = std::to_string(entry["level"].get_unsigned()); break;
							case tao::json::type::SIGNED:      level = std::to_string(entry["level"].get_signed()); break;
							case tao::json::type::STRING:      level = entry["level"].get_string(); break;
							case tao::json::type::STRING_VIEW: level = entry["level"].get_string_view(); break;
							default:						   level = "0"; break;
							}
							md5 = entry["md5"].get_string();
							name = entry["title"].get_string();
						}
						catch (std::bad_variant_access& e)
						{

						}

						if (!level.empty() && !md5.empty())
						{
							auto pEntry = std::make_shared<EntryChart>();
							pEntry->md5 = md5;
							pEntry->_name = name;
							entries[level].push_back(pEntry);
						}
					}

					if (!body.empty())
					{
						auto tablePath = getFolderPath();
						if (!fs::exists(tablePath)) fs::create_directories(tablePath);
						std::ofstream ofs(tablePath / "data.json", std::ios_base::binary);
						ofs << body;
						ofs.close();
					}
				}
				else
				{
					switch (result)
					{
					case GetResult::ERR_SNI_HOSTNAME:
					case GetResult::ERR_RESOLVE:	  finishedCallback(DifficultyTable::UpdateResult::DATA_PATH_ERROR); break;
					case GetResult::ERR_CONNECT:
					case GetResult::ERR_HANDSHAKE:
					case GetResult::ERR_WRITE:
					case GetResult::ERR_READ:		  finishedCallback(DifficultyTable::UpdateResult::DATA_CONNECT_ERR); break;
					case GetResult::ERR_TIMEOUT:	  finishedCallback(DifficultyTable::UpdateResult::DATA_TIMEOUT); break;
					}
				}
			});
		ioc.run();

		if (!entries.empty())
		{
		}
		else
		{
			finishedCallback(UpdateResult::DATA_PARSE_FAILED);
			return;
		}
	}

	finishedCallback(UpdateResult::OK);
}

bool DifficultyTableBMS::loadFromFile()
{
	auto tablePath = getFolderPath();
	entries.clear();

	if (!fs::exists(tablePath)) return false;

	Path headerPath = tablePath / "header.json";
	Path dataPath = tablePath / "data.json";
	if (!fs::exists(headerPath) || !fs::exists(dataPath))
	{
		LOG_ERROR << "[TableBMS] header.json or data.json not found!";
		return false;
	}

	// parse Header
	{
		tao::json::value header = tao::json::from_file(headerPath);
		try
		{
			name = header["name"].get_string();
			symbol = header["symbol"].get_string();
		}
		catch (std::bad_variant_access& e)
		{

		}
	}

	// parse Data
	{
		std::vector<tao::json::value> header = tao::json::from_file(dataPath).get_array();
		for (auto& entry : header)
		{
			std::string level, md5, name;
			try
			{
				switch (entry["level"].type())
				{
				case tao::json::type::UNSIGNED:    level = std::to_string(entry["level"].get_unsigned()); break;
				case tao::json::type::SIGNED:      level = std::to_string(entry["level"].get_signed()); break;
				case tao::json::type::STRING:      level = entry["level"].get_string(); break;
				case tao::json::type::STRING_VIEW: level = entry["level"].get_string_view(); break;
				default:						   level = "0"; break;
				}
				md5 = entry["md5"].get_string();
				name = entry["title"].get_string();
			}
			catch (std::bad_variant_access& e)
			{

			}

			if (!level.empty() && !md5.empty())
			{
				auto pEntry = std::make_shared<EntryChart>();
				pEntry->md5 = md5;
				pEntry->_name = name;
				entries[level].push_back(pEntry);
			}
		}
	}

	return true;
}

Path DifficultyTableBMS::getFolderPath() const
{
	Path tablePath(GAMEDATA_PATH);
	tablePath /= "table";
	tablePath /= md5(url).hexdigest();
	return tablePath;
}

std::string DifficultyTableBMS::getSymbol() const
{
	return symbol;
}