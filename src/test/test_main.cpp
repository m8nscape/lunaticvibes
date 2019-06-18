#include <gtest/gtest.h>
#include <gmock/gmock.h>
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    int r = RUN_ALL_TESTS();
    system("pause");
    return r;
}