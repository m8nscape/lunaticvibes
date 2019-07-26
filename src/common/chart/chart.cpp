#include "chart.h"

Path vChart::getDirectory() const
{
    return _filePath.parent_path();
}