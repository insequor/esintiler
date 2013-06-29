// unit_test.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include <map>
#include <vector>

#include "../include/suite.h"
#include "../include/value.h"

using namespace esintiler;

/** 
 * 
 */

class Foo
{
public:
    Foo() : m_status(true) {}

    void update() { m_status = true;}
    void invalidate() { m_status = false; }
    
    bool m_status;
};

DECLARE_TESTER(Foo, FooTester)
{
    //Test methods should be given by user
    TESTER_METHOD(update, ())
    {
        value().update();
        return result(value().m_status);
    }

    TESTER_METHOD(invalidate, ())
    {
        value().invalidate();
        return result(!value().m_status);
    }

};
DECLARE_TESTER_VALUE(FooTester);


TEST_SUITE(FooSuite)
{
    TEST(TypeCaseShouldWork)
    {
        Foo foo;
        CHECK_THAT(&foo == value(&foo));
        //CHECK_THAT(foo == value(foo));
    }

    TEST(UpdateTestShouldWork)
    {
        Foo foo;
        CHECK_THAT(value(foo).should.update());
        CHECK_THAT(!value(foo).should.update().and.not.update());
        CHECK_THAT(!value(foo).should.not.update());

        CHECK_THAT(value(&foo).should.update());
        CHECK_THAT(!value(&foo).should.not.update());
        CHECK_THAT(!value(&foo).should.update().and.not.update());
    }

    
};
