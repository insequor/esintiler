/**
 * Ozgur Yuksel, June, 2010
 * A simple natural language imitation for value testing
 * Highly influenced by igloo
 *
 * Usage Example
 *

    DECLARE_TESTER(int, IntTester)
    {
        //tester method without any input parameter
        TESTER_METHOD(check, ())
        {
            return result(true);
        }

        //tester method can accepts parameters
        TESTER_METHOD(compare, (int iVal))
        {
            return result(value() == iVal);
        }
    };
    DECLARE_TESTER_VALUE(IntTester);


 *
 * above class now can be used like:
 *
    int a = 4;
    bool res = value(a).should.compare(4);

 */
#pragma once

namespace esintiler
{

typedef enum Operators
{
    OpNone,
    OpNot,
    OpAnd,
    OpOr,
    OpTrue,
    OpFalse
};

/**
 *
 */
struct Base
{
    Base(Base *ipParent = 0, Operators iOp = OpNone) 
        : parent(ipParent), op(iOp)
    {
    }

    Base *root() 
    {
        Base *pRoot = this;
        while(pRoot->parent)
            pRoot = pRoot->parent;
        return pRoot;
    }
    
    Base *parent;
    Operators op;
};

/**
 * Result class is used to return the test result. It needs the ValueType 
 * and the ShouldType so it can provide "and" and "or" operators
 */
template<class ShouldType>
class Result : public Base
{
public:
    const int verbose;  //dev time variable for easy testing

    Result(Base *ipParent, bool iResult) 
        : Base(ipParent, iResult ? OpTrue : OpFalse)
        , verbose(false)
    {
        or.op = OpOr;
        or.parent = this;

        and.op = OpAnd;
        and.parent = this;
    }
    
    /**
     * Bool cast operator so result itself can be used directly in checking
     */
    virtual operator bool()
    {
        //Find the parent first
        std::vector<Base*> parents; //we will not keep the value object
        for(Base *pParent = this; pParent->parent != 0; pParent = pParent->parent)
        {
            if(pParent->op == OpNone)
                continue;
            parents.push_back(pParent);
        }
        //start from top and evaluate the result
        bool result = true;
        std::vector<Base*>::iterator iEnd = parents.end();
        do
        {
            result = GetResultValue(result, --iEnd);
        } while(iEnd != parents.begin() );
        if(verbose)
            printf (" = %s\n", result ? "true" : "false");
        return result;
    }

    ShouldType or;

    ShouldType and;

private:
    inline bool GetResultValue(bool iStartResult, std::vector<Base*>::iterator &iObj)
    {

        Base *pObj = *iObj;
        bool result = true;
        switch(pObj->op)
        {
        case OpNot:
            if(verbose)
                printf(".not");
            result = !GetResultValue(iStartResult, --iObj);
            break;
        case OpAnd:
            if(verbose)
                printf(".and");
            result = GetResultValue(iStartResult, --iObj);
            result &= iStartResult; 
            break;
        case OpOr:
            if(verbose)
                printf(".or");
            result = GetResultValue(iStartResult, --iObj);
            result |= iStartResult;
            break;
        case OpTrue:
            if(verbose)
                printf(".true");
            result = true;
            break;
        case OpFalse:
            if(verbose)
                printf(".false");
            result = false;
            break;
        default:
            break;
        }
        return result;
    }
};

/**
 * Base class for all user provided tester classes
 * It needs to know the ValueType since it will store it and ShouldType 
 * since it needs to return dedicated Result instances
 */
template<class ShouldType>
struct TesterBase : public Base
{
    inline Result<ShouldType> result(bool iResult)
    {
        return Result<ShouldType>(this, iResult);
    }
};

/** 
 *
 */
template <class TT>
class Should : public TT
{
public:
    Should()
    {
        not.op = OpNot;
        not.parent = this;
    }
    TT not;
};

/**
 * 
 */
template<class VT, class ShouldType>
struct Value : public Base
{
public:
    Value(VT& iVal)
        : m_pVal(&iVal)
    {
        should.parent = this;
    }
    
    Value(VT *iVal)
        : m_pVal(iVal)
    {
        should.parent = this;
    }

    virtual operator VT&() 
    { 
        return *m_pVal; 
    }
    
    virtual operator VT*()
    { 
        return m_pVal; 
    }

    ShouldType should;

private:
    VT* m_pVal;
};




/**
 *
 */
#define DECLARE_TESTER(VT, TT)                                      \
    struct TT;                                                      \
    typedef Should<TT> _##TT;                                       \
    typedef Value <VT, _##TT> Value##TT;                            \
    struct _Base##TT: public TesterBase<_##TT>                      \
    {                                                               \
        typedef Result<_##TT> ResultType;                           \
        typedef VT ValueType;                                       \
        ValueType& value();                                         \
        ValueType* valuePtr();                                      \
        _Base##TT () { CurrentTester() = this; } \
        static _Base##TT*& CurrentTester() {static _Base##TT *pTester = 0; return pTester; }\
    };                                                              \
    struct TT: public _Base##TT


#define DECLARE_TESTER_VALUE(TT)                                    \
    inline _Base##TT::ValueType& _Base##TT::value()                 \
    {                                                               \
        Base *pRoot = root();                                       \
        return  (ValueType&) (*(Value##TT*)pRoot) ;                 \
    }                                                               \
    inline _Base##TT::ValueType* _Base##TT::valuePtr()              \
    {                                                               \
        Base *pRoot = root();                                       \
        return  (ValueType*) (*(Value##TT*)pRoot) ;                 \
    }                                                               \
    Value##TT value(TT::ValueType& iVal) { return Value##TT(iVal); }\
    Value##TT value(TT::ValueType* iVal) { return Value##TT(iVal); }


/**
 * MACRO to define methods of the tester object. This is not a mandatory 
 * macro but provided for two reasons:
 * - Hide ResultType from user, he doesn't need to know what result() 
 *   returns
 * - Provide markers for static analysis/documentation 
 * - Provide an extension mechanism for future improvements, if we need/plan
 *   to extend the test mechanism we can actually replace the methods with 
 *   tester objects
 */
#define TESTER_METHOD(method, params) \
    ResultType method params

}; //namespace