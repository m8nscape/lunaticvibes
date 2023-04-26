#pragma once

class CourseLr2crs
{
public:
	struct Course
	{
		std::string title;
		int line = 7;
		enum {
			COURSE_NONSTOP,
			COURSE_EXPERT,
			COURSE_GRADE,
		} type = COURSE_EXPERT;
		std::string hashTop;
		std::vector<HashMD5> chartHash;

		HashMD5 getCourseHash() const;
	};
	std::vector<Course> courses;
	long long addTime = 0;

public:
	CourseLr2crs(const Path& filePath);
	~CourseLr2crs() = default;
};