#include "common/pch.h"
#include "lr2crs.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace lunaticvibes
{

CourseLr2crs::CourseLr2crs(const Path& filePath)
{
	// .lr2crs is a standard xml file. A file may contain muiltiple courses.

	// load file
	std::ifstream ifs(filePath);
	if (ifs.fail())
	{
		LOG_WARNING << "[lr2crs] " << filePath.u8string() << " File ERROR";
		return;
	}
	std::stringstream ss;
	ss << ifs.rdbuf();
	ss.sync();
	ifs.close();

	addTime = getFileLastWriteTime(filePath);

	// convert codepage
	auto encoding = getFileEncoding(ss);
	std::stringstream ssUTF8;
	std::string lineBuf;
	while (!ss.eof())
	{
		std::getline(ss, lineBuf);

#ifdef WIN32
		static const auto localeUTF8 = std::locale(".65001");
#else
		static const auto localeUTF8 = std::locale("en_US.UTF-8");
#endif
		// convert codepage
		lineBuf = to_utf8(lineBuf, encoding);

		ssUTF8 << lineBuf;
	}
	ssUTF8.sync();
	ss.clear();

	// parse as XML
	try
	{
		namespace pt = boost::property_tree;
		pt::ptree tree;
		pt::read_xml(ssUTF8, tree);
		for (auto& course : tree.get_child("courselist"))
		{
			courses.emplace_back();
			Course& c = courses.back();
			for (auto& [name, value] : course.second)
			{
				if (strEqual(name, "title"))
					c.title = value.data();
				else if (strEqual(name, "line"))
					c.line = toInt(value.data());
				else if (strEqual(name, "type"))
					c.type = decltype(c.type)(toInt(value.data()));
				else if (strEqual(name, "hash"))
				{
					// first 32 characters are course metadata, of which we didn't make use; the 10th char is course type, the last 4 are assumed to be uploader ID
					// after that, each 32 chars indicates a chart MD5
					const std::string& hash = value.data();
					c.hashTop = hash.substr(0, 32);
					for (size_t count = 1; count < hash.size() / 32; ++count)
					{
						c.chartHash.push_back(HashMD5(hash.substr(count * 32, 32)));
					}
				}
			}
		}
	}
	catch (...)
	{
		courses.clear();
	}
}

HashMD5 CourseLr2crs::Course::getCourseHash() const
{
	std::stringstream ss;
	ss << (int)type;
	ss << chartHash.size();
	for (auto& c : chartHash) ss << c.hexdigest();
	return md5(ss.str());
}

}
