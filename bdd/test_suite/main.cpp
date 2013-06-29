// unit_test.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include <map>
#include <vector>

#include "../include/suite.h"

using namespace esintiler;

int& MappedValue(const std::string &val);


class FooLogger : public Logger
{
public:
    void log(const char* ipMsg) 
    {
        m_log.push_back(ipMsg);
        //Logger::log(ipMsg);
    }

    std::vector<std::string> m_log;
};

/*
*/
int checkLog(const char *pTest, const FooLogger &iLogger, const char* ipRef[])
{
    unsigned int i = 0;
    for(i = 0; ipRef[i] != 0; i++)
    {
        if(i >= iLogger.m_log.size())
            break;
        if(ipRef[i][0] == '#' && iLogger.m_log[i][0] == '#')
            continue;
        if(iLogger.m_log[i] != ipRef[i]) {
            printf("%s != %s\n", iLogger.m_log[i].c_str(), ipRef[i]);        
            break;
        }
    }
    if(i == iLogger.m_log.size())
        return 0;

    printf("Log Does Not Match the Reference: %i : %i : %s\n", iLogger.m_log.size(), i, pTest);
    for(i = 0; i < iLogger.m_log.size(); i++)
        printf("%s\n", iLogger.m_log[i].c_str());
    
    return 1;
}


//
//
//
TEST_SUITE(SuiteTester)
{
    TEST("AllMethodsShouldBeCalledInOrder")
    {
        const char* pRef[] = {
            "SampleSuite",
            "Construct",
            "SetUp(fooTest1)",
            "fooTest1",
            "...OK",
            "TearDown(fooTest1)",
            "SetUp(fooTest2)",
            "fooTest2",
            "...OK",
            "TearDown(fooTest2)",
            "Destruct",
            0
        };
        FooLogger mlogger;
        CHECK_THAT(TestManager::ExecuteSuite("SampleSuite", &mlogger) == 0);
        CHECK_THAT(checkLog("@1", mlogger, pRef) == 0);
    }

    TEST("AllTestsShouldBeSkippedIfConstructorFails")
    {
        MappedValue("Construct") = 1;
        const char* pRef[] = {
            "SampleSuite",
            "Construct",
            "Could not Initialize the Test Suite, all tests will be skipped",
            "Destruct",
            "...Failed (No Assertions)",
            0
        };
        FooLogger mlogger;
        CHECK_THAT(TestManager::ExecuteSuite("SampleSuite", &mlogger) > 0);
        CHECK_THAT(checkLog("@2", mlogger, pRef) == 0);
        MappedValue("Construct") = 0;
    }
    
    TEST("TestShouldBeSkippedIfSetUpFails")
    {
        MappedValue("fooTest1") = 1;
        const char* pRef[] = {
            "SampleSuite",
            "Construct",
            "SetUp(fooTest1)",
            "SetUp(fooTest2)",
            "fooTest2",
            "...OK",
            "TearDown(fooTest2)",
            "Destruct",
            0
        };
        FooLogger mlogger;
        CHECK_THAT(TestManager::ExecuteSuite("SampleSuite", &mlogger) > 0);
        CHECK_THAT(checkLog("@3", mlogger, pRef) == 0);
        MappedValue("fooTest1") = 0;
    }

    TEST("CheckMacroShouldNotTerminateTest")
    {
        MappedValue("CheckFails") = 1;
        const char* pRef[] = {
            "SampleSuite",
            "Construct",
            "SetUp(fooTest1)",
            "fooTest1",
            "#",
            "#",
            "#",
            "#",
            "#",
            "#",
            "...Failed (2 Assertions)",
            "TearDown(fooTest1)",
            "SetUp(fooTest2)",
            "fooTest2",
            "...OK",
            "TearDown(fooTest2)",
            "Destruct",
            0
        };
        FooLogger mlogger;
        CHECK_THAT(TestManager::ExecuteSuite("SampleSuite", &mlogger) > 0);
        CHECK_THAT(checkLog("@4", mlogger, pRef) == 0);
        MappedValue("CheckFails") = 0;
    }

    TEST("AssertMacroShouldTerminateTest")
    {
        MappedValue("AssertFails") = 1;
        const char* pRef[] = {
            "SampleSuite",
            "Construct",
            "SetUp(fooTest1)",
            "fooTest1",
            "#",
            "#",
            "#",
            "...Failed (1 Assertions)",
            "TearDown(fooTest1)",
            "SetUp(fooTest2)",
            "fooTest2",
            "...OK",
            "TearDown(fooTest2)",
            "Destruct",
            0
        };
        FooLogger mlogger;
        CHECK_THAT(TestManager::ExecuteSuite("SampleSuite", &mlogger) > 0);
        CHECK_THAT(checkLog("@4", mlogger, pRef) == 0);
        MappedValue("AssertFails") = 0;
    }

    TEST("CHECK and ASSERT Objects")
    {
        CHECK.True(true);
        ASSERT.True(true);
    }

};

int main(int argc, char* argv[])
{
    int retVal = TestManager::ExecuteSuite("SuiteTester");
    printf("%i\n", retVal);
}

