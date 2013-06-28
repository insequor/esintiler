/**
 * Ozgur Yuksel, June, 2010
 * A simple unit test implementation
 * Highly influenced by igloo
 *
 *
 * Usage Example
 *
    TEST_SUITE(SampleSuite)
    {
        //Optional Initialization methods
        int Construct()
        {
            return 0;
        }
        void Destruct()
        {
        }
        int SetUp(const std::string &iName)
        {
            return 0;
        }
        void TearDown(const std::string &iName)
        {
        }

        //Test Methods
        TEST(ShouldTestSomething)
        {
            CHECK_THAT(true);
            CHECK_THAT(false);
        }
    };


 *
 * Above test suite can be executed either by giving a name:
 * TestManager::ExecuteSuite("SuiteTester");
 * Or without which executed all the test suites
 * TestManager::ExecuteSuite();
 *
 * ExecuteSuite should be called in main() method, but main.cpp does not need to 
 * include the code for the test suite or know about it.
 *
 */
#pragma once

#include <vector>
#include <string>

namespace esintiler 
{

/**
 * A simple logger which can be passed to the test manager to log the 
 * activities. 
 */
class Logger
{
public:
    virtual void log(const char *ipMsg) 
    {
        printf("%s\n", ipMsg);
    }
    
    void log(const std::string &iMsg) 
    { 
        log(iMsg.c_str()); 
    }
};

/**
 * Forward decleration for TestSuiteBase, it is requred for abstract method in TestBase
 */
struct TestSuiteBase;


/**
 * TestBase class is used as the base class for each test method object. Test method objects 
 * are defined as data member of Test Suite so their constructer is called whenever a Test Suite
 * is instantiated. This way each method can register itself to the test suite. 
 *
 * Registration is done in constructor, so we define a derived TestBase class for each test method. 
 */
struct TestBase
{
    /** 
     * Abastruct method to be called by test manager to trigger the each Test execution. 
     * Actual implementation of this method is given in derived classes which is defined by 
     * macro (TEST) for each test method instance.
     *
     * @ipSuite: Parent suite object passed to the method so it can trigger the actual method
     * on it.
     */
    virtual void Execute(TestSuiteBase* ipSuite) = 0;   

};

/**
 * Base class to define the collection of tests. User extends this class by 
 * defining test methods as well as overriding the SetUp and TearDown methods
 *
 */
struct TestSuiteBase
{
    /** 
     * This method will be called just after a TestSuite object is constructed but
     * before calling any test method on it. It replaces the constructor and used here 
     * to avoid name dependency in TestSuite class. 
     *
     * @return: 0 to indicate success. Any non-zero will make the suite as failed and no 
     * test will be executed
     */
    virtual int Construct() {return 0;}

    /**
     * This method will be called just before destroying the TestSuite object. It replaces
     * the destructor and used to avoid name dependency to the test suite class
     */
    virtual void Destruct() {};

    /**
     * This method will be called before each test method execution. If you need to 
     * provide one time set up for all tests you should use the constructor for your class
     * A single test suite object will be initialized to be used by all test methods provided 
     * in that suite.
     *
     * @return: 0 to indicate success. Any non-zero value will mark the suite as failed and 
     * following test method and TearDown calls will not be invoked
     */
    virtual int SetUp(const std::string &iName) {return 0; }
    
    /**
     * This method will be called after each test method execution. If you need to 
     * provide one time cleaning use destructor of your test suite.
     */
    virtual void TearDown(const std::string &iName) {}

    /**
     * Constructor will set the current object as the current suite. 
     */
    TestSuiteBase()
        : numAssertions(0)
        , numFailedAssertions(0)
        , logger(0)
    {
    }

    Logger *logger;
    
    //
    //
    //
    typedef std::vector<std::pair<std::string, TestBase*>> TestList;
    TestList Tests;

    int numAssertions;
    int numFailedAssertions;
};

/**
 * Factory object to instantiate TestSuite objects when requested. Test runners are created 
 * as soon as application is loaded since they all have static/global instances. However a 
 * test suite will be instantiated only when needed and it will be destroyed as soon as all 
 * tests are executed
 * 
 * This is an internal class and users do not need to interact with it.
 */
struct TestRunnerBase
{
    virtual TestSuiteBase *CreateSuite() = 0;
};


/**
 * User interacts with this class to run a single test suite or all of them
 */
class TestManager
{
private:
    typedef std::vector<std::pair<std::string, TestRunnerBase*>>  TestRunnerList;

public:
    /**
     * Wrapper method for the ExecuteSuite which triggers execution of all registered 
     * test suites
     *
     * @return: 0 if all tests are OK, non zero if any test failed 
     */
    static int ExecuteAllSuites(Logger *logger = new Logger())
    {
        return ExecuteSuite("", logger);
    }

    /**
     * Method to execute one (given) or all (given is empty) test suites registered by the 
     * manager
     *
     * @return: 0 if all tests are OK, non zero if any test failed 
     */
    static int ExecuteSuite(const std::string &iSuiteName, Logger *logger = new Logger())
    {
        int retVal = 0;
        TestRunnerList& testRunners = TestRunners();
        TestRunnerList::iterator it = testRunners.begin();
        for(; it != testRunners.end(); it++)
        {
            if(iSuiteName != "" && it->first != iSuiteName)
                continue;

            int numAssertions = 0;
            int numFailedAssertions = 0;

            logger->log(it->first);

            TestRunnerBase *pRunner = it->second;
            TestSuiteBase *pSuite = pRunner->CreateSuite();
            pSuite->logger = logger;
            
            if(pSuite->Construct() == 0)
            {
                TestSuiteBase::TestList::iterator itTest = pSuite->Tests.begin();
                for(; itTest != pSuite->Tests.end(); itTest++)
                {
                    if(pSuite->SetUp(itTest->first) != 0)
                    {
                        retVal ++;
                        continue;
                    }

                    logger->log(itTest->first);
                    itTest->second->Execute(pSuite);
                    
                    if(pSuite->numAssertions == numAssertions)
                    {
                        logger->log("...Failed (No Assertions)");
                        retVal ++;
                    }
                    else if(pSuite->numFailedAssertions != numFailedAssertions)
                    {
                        char pBuf[1024];
                        sprintf_s(pBuf, "...Failed (%i Assertions)", pSuite->numFailedAssertions - numFailedAssertions);
                        logger->log(pBuf);
                        retVal ++;
                    }
                    else
                        logger->log("...OK");
                    numAssertions = pSuite->numAssertions;
                    numFailedAssertions = pSuite->numFailedAssertions;
                    
                    pSuite->TearDown(itTest->first);
                }
            }
            else
                retVal ++;
            
            pSuite->Destruct();
            delete pSuite;
            pSuite = 0;
        }
        return retVal;
    }

    /**
     *
     */
    static TestRunnerList& TestRunners()
    {
        static TestRunnerList testRunners;
        return testRunners;
    }
};

/**
 * Template class definition to extend the TestRunnerBase with type information. 
 * Please refer to TestRunnerBase for more details.
 */
template<class SuiteName>
class TestRunner : public TestRunnerBase
{
public:
    TestRunner(const std::string &iName)
    {
        TestManager::TestRunners().push_back(std::make_pair(iName, this));
    }
    
    TestSuiteBase *CreateSuite()
    {
        return new SuiteName();
    }
};

/**
 * MACRO definition to define a new test suite. All test suites will be registered as soon
 * as the application is loaded however they will be instantiated only when they are needed
 */
#define TEST_SUITE(SuiteName) \
    TEST_SUITE_IMPL(SuiteName, true)

/** 
 * MACRO definition for deactive test suites. This macro can be used to temporarily disabled 
 * all the tests. Test suite will still be registered to the manager, however it will have no 
 * registered test methods so nothing will be executed.
 */
#define _TEST_SUITE(SuiteName) \
    TEST_SUITE_IMPL(SuiteName, false)

/**
 * MACRO definition, actual implementation of TEST_SUITE macros. User one of the above ones 
 * in your code
 *
 * @SuiteName: Name of the user provided test suite class
 * @Active: Enable/Disable test method registration for the test suite
 *
 * It will do the following code for you:
 * * Define a global instance of TestRunner for your TestSuite class so it is automatically 
 *   instantiated as soon as application is started
 * * Define a base class as "_SuiteName" which will contain some internal code for your test suite
 *   - typedef for the test suite so inner classes can refer to the class name
 *   - static method to keep the instance of the test suite, this will force you that you can not 
 *     instantiate two instances of the same suite at the same time, limit parallel testing but this 
 *     is very unlikely scenario
 *   - Constructor which will set the current instance for future access by test method instances
 *   - Starter for your test suite class. It uses "struct" rather than "class" to avoid necessary 
 *     "public" keyword afterwards. This causes all methods to be public but test suites are not 
 *     intended to be used outside the test manager.
 */
#define TEST_SUITE_IMPL(SuiteName, Active)                                      \
    struct SuiteName;                                                           \
    TestRunner<SuiteName> SuiteRunner_##SuiteName(#SuiteName);                  \
    struct _##SuiteName : public TestSuiteBase                                  \
    {                                                                           \
        _##SuiteName() : TestSuiteBase() { if(Active) CurrentTestSuite = this; }\
        typedef SuiteName CurrentSuiteName ;                                    \
        static TestSuiteBase* CurrentTestSuite;                                 \
    };                                                                          \
    TestSuiteBase * _##SuiteName::CurrentTestSuite = 0;                         \
struct SuiteName : public _##SuiteName


/**
 * MACRO definition to define a new test method and register it to the current suite.
 * Unlike test suites, test methods are registered only when their test suite is instantiated. 
 * All registiration takes place before the constuctor of the user's test suite definition
 */
#define TEST(TestName)                                                                          \
    struct Test_##TestName : public TestBase                                                    \
    {                                                                                           \
        Test_##TestName()                                                                       \
        {                                                                                       \
            if(CurrentTestSuite)                                                                \
                CurrentTestSuite->Tests.push_back(std::make_pair(#TestName, this));             \
        }                                                                                       \
        void Execute(TestSuiteBase *ipSuite)                                                    \
        {                                                                                       \
            ((CurrentSuiteName*)ipSuite)->##TestName();                                         \
        }                                                                                       \
    } Test_##TestName;                                                                          \
    void TestName()


/**
 * MACRO definition for disabled test methods. There is no registration so they are simply methods
 * defined in test suite and they will not be automatically invoked. This is used to temporarily 
 * disable a test method during runtime
 *
 * TODO: Shall we report the disabled test methods? This way we can still report some warning message 
 * at the end of the test execution saying test passed but you have disabled test methods...
 */
#define _TEST(TestName) \
    void TestName()

/**
 * MACRO definition for testing, it will check the given statement and log a message if statement 
 * is false.
 */
#define CHECK_THAT(statement) \
    ASSERT_INTERNAL(statement, 0)

/**
 * MACRO definition for testing, it will check the given statement and log a message if statement 
 * is false. It will also return from current test method so no other code will be executed 
 */
#define ASSERT_THAT(statement)      \
    ASSERT_INTERNAL(statement, 1)

/**
 * MACRO for actual assert implementation. Use one of the above macros
 */
#define ASSERT_INTERNAL(statement, shallReturn) \
    {                               \
        numAssertions ++;           \
        if(!(statement))              \
        {                           \
            numFailedAssertions++;  \
            char pBuf[2048];        \
            sprintf_s(pBuf, "#statement: %s", #statement); logger->log(pBuf);    \
            sprintf_s(pBuf, "#file     : %s", __FILE__); logger->log(pBuf);      \
            sprintf_s(pBuf, "#line     : %i", __LINE__); logger->log(pBuf);      \
            if(shallReturn)         \
                return;             \
        }                           \
    }

/**
 * MACRO definitions to disable the checks temporarily. Test lines can be commented 
 * out as well, but this provides us logging capabilities for future if needed and 
 * code looks better.
 */
#define _CHECK_THAT(statement) ;
#define _ASSERT_THAT(statement) ;

}; //namespace
