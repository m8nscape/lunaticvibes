#include "entry_course.h"


std::shared_ptr<EntryCourse> EntryFolderCourse::getEntry(size_t idx)
{
    idx %= getContentsCount();
    return courses[idx];
}

void EntryFolderCourse::pushEntry(std::shared_ptr<EntryCourse> c)
{
    courses.push_back(c);
}
