#pragma once
#include <vector>
#include "common/types.h"
#include "entry.h"
#include "entry_folder.h"
#include "common/coursefile/lr2crs.h"

class EntryCourse;
class EntryFolderCourse : public EntryFolderBase
{
public:
	EntryFolderCourse() = delete;
	EntryFolderCourse(const StringContent& name = "", const StringContent& name2 = "") :
		EntryFolderBase(HashMD5(""), "")
	{
		_type = eEntryType::COURSE_FOLDER;
		_name = name;
		_name2 = name2;
	}
	
protected:
	std::vector<std::shared_ptr<EntryCourse>> courses;
	size_t idx = 0;

public:
	std::shared_ptr<EntryCourse> getEntry(size_t idx);
	void pushEntry(std::shared_ptr<EntryCourse> f);
	virtual size_t getContentsCount() { return courses.size(); }
	virtual bool empty() { return courses.empty(); }
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