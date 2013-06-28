// unit_test.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include <map>
#include <vector>

#include "../include/suite.h"

using namespace esintiler;


int& MappedValue(const std::string &val)
{
    static std::map<std::string, int> _MappedValues;
    return _MappedValues[val];
}

TEST_SUITE(SampleSuite)
{
    int Construct()
    {
        logger->log("Construct");
        return MappedValue("Construct");
    }

    void Destruct()
    {
        logger->log("Destruct");
    }

    int SetUp(const std::string &iName)
    {
        logger->log(std::string("SetUp(") + iName + ")");
        return MappedValue(iName);
    }

    void TearDown(const std::string &iName)
    {
        logger->log(std::string("TearDown(") + iName + ")");
    }

    TEST(fooTest1)
    {
        ASSERT_THAT(!MappedValue("AssertFails"));
        CHECK_THAT(!MappedValue("CheckFails"));
        CHECK_THAT(!MappedValue("CheckFails"));
    }
    TEST(fooTest2)
    {
        CHECK_THAT(1);
    }
};
