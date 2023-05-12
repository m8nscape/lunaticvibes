#pragma once
#include <vector>
#include "common/types.h"
#include "entry.h"
#include "entry_folder.h"
#include "common/coursefile/lr2crs.h"

namespace lunaticvibes
{

class EntryCourse;
class EntryFolderCourse : public EntryFolderBase
{
public:
	EntryFolderCourse() = delete;
	EntryFolderCourse(StringContentView name = "", StringContentView name2 = "") :
		EntryFolderBase(HashMD5(""), name, name2)
	{
		_type = eEntryType::COURSE_FOLDER;
	}
};

// entry for courses
class EntryCourse : public EntryBase
{
public:
	std::vector<HashMD5> charts;
	enum CourseType
	{
		UNDEFINED,
		GRADE,
	} courseType = UNDEFINED;

public:
	EntryCourse() = default;
	virtual ~EntryCourse() = default;

	EntryCourse(const CourseLr2crs::Course& lr2crs, long long addTime)
	{
		_type = eEntryType::COURSE;
		md5 = lr2crs.getCourseHash();
		_name = lr2crs.title;
		_addTime = addTime;
		switch (lr2crs.type)
		{
		case 2: courseType = GRADE; break;
		}
		charts = lr2crs.chartHash;
	}
};

}
