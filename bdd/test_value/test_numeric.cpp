
#include "../include/suite.h"
#include "../include/numeric.h"

using namespace esintiler;

/** 
 * Tests for provided numeric value testers. It also serves as tester for the value 
 * mechanism
 */
TEST_SUITE(Numeric)
{
    int Construct()
    {
        cv = 'a';
        iv = 5;
        dv = 9.5;
        return 0;
    }

    char    cv;
    int     iv;
    double  dv;

    TEST(TypeCaseShouldWork)
    {
        CHECK_THAT('a' == value(cv));
        CHECK_THAT(5 == value(iv));
        CHECK_THAT(9.5 == value(dv));
    }

    TEST(EqualCheckShouldBePossible)
    {

        NumericValue<char> cvv(cv);
        CHECK_THAT(value(cv).should.equal_to('a'));
        CHECK_THAT(value(iv).should.equal_to(5));
        CHECK_THAT(value(dv).should.equal_to(9.5));

        CHECK_THAT(!value(cv).should.equal_to('b'));
        CHECK_THAT(!value(iv).should.equal_to(6));
        CHECK_THAT(!value(dv).should.equal_to(9.6));
    }

    TEST(LessThanCheckShouldBePossible)
    {
        CHECK_THAT(value(cv).should.be_less_than('b'));
        CHECK_THAT(value(iv).should.be_less_than(6));
        CHECK_THAT(value(dv).should.be_less_than(9.6));

        CHECK_THAT(!value(cv).should.be_less_than('a'));
        CHECK_THAT(!value(iv).should.be_less_than(5));
        CHECK_THAT(!value(dv).should.be_less_than(9.5));
    }

    TEST(GreaterThanCheckShouldBePossible)
    {
        CHECK_THAT(value(cv).should.be_greater_than('A'));
        CHECK_THAT(value(iv).should.be_greater_than(3));
        CHECK_THAT(value(dv).should.be_greater_than(9.3));

        CHECK_THAT(!value(cv).should.be_greater_than('b'));
        CHECK_THAT(!value(iv).should.be_greater_than(5));
        CHECK_THAT(!value(dv).should.be_greater_than(9.5));
    }

    TEST(NotOperatorShouldAddNegation)
    {
        CHECK_THAT(value(iv).should.not.equal_to(6));
        CHECK_THAT(value(iv).should.not.be_less_than(4));
        CHECK_THAT(value(iv).should.not.be_greater_than(6));

        CHECK_THAT(!value(iv).should.not.equal_to(5));
        CHECK_THAT(!value(iv).should.not.be_less_than(6));
        CHECK_THAT(!value(iv).should.not.be_greater_than(4));
    }
    
    TEST(OrOperatorShouldCombineTwoChecks)
    {
        /* true  || true */CHECK_THAT(value(iv).should.be_less_than(8).or.be_less_than(9));
        /* true  || !false */CHECK_THAT(value(iv).should.be_less_than(8).or.not.equal_to(9));
        /* !false || !false */CHECK_THAT(value(iv).should.not.be_less_than(4).or.not.equal_to(9));

        /* true  || false*/CHECK_THAT(value(iv).should.equal_to(5).or.equal_to(6));
        /* false || true */CHECK_THAT(value(iv).should.equal_to(6).or.be_greater_than(3));

        /* false || false*/CHECK_THAT(!value(iv).should.equal_to(8).or.equal_to(6));
        /* false || !true*/CHECK_THAT(!value(iv).should.equal_to(8).or.not.equal_to(5));
        /* !true || !true*/CHECK_THAT(!value(iv).should.not.be_less_than(8).or.not.equal_to(5));
    }

    TEST(AndOperatorShouldCombineTwoChecks)
    {
        /* true  && true */CHECK_THAT(value(iv).should.be_less_than(8).and.be_less_than(9));
        /* true  && false*/CHECK_THAT(!value(iv).should.equal_to(5).and.equal_to(6));
        /* false && true */CHECK_THAT(!value(iv).should.equal_to(6).and.be_greater_than(3));
        /* false || false*/CHECK_THAT(!value(iv).should.equal_to(8).and.equal_to(6));
    }

    TEST(BothOperatorsShouldBeCombined)
    {
        //First come first serves, so 
        //a.and.b.or.c is equal to [(a && b) || c]
        /* true  && false || true */CHECK_THAT(value(iv).should.be_less_than(8).and.equal_to(9).or.be_less_than(9));
    }

};
