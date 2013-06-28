/**
 * Ozgur Yuksel, June, 2010
 * Numeric value testing. It doesn't use MACRO definitions from value.h to benefit
 * the template mechanism
 */

#pragma once

#include "value.h"

namespace esintiler
{

template<class VT> struct NumericTester;

template<class VT>
class _NumericTester : public Should<NumericTester<VT>>
{
};

template<class VT>
struct _BaseNumericTester: public TesterBase<_NumericTester<VT>> 
{
    typedef Result<_NumericTester<VT>> ResultType;
    typedef VT ValueType;

    inline ValueType& value()
    {
        Base *pRoot = root();
        NumericValue<ValueType>* pNV = (NumericValue<ValueType>*)pRoot;
        ValueType& val = (ValueType&) (*pNV) ;
        return val;
    }
    inline ValueType* valuePtr()
    {
        Base *pRoot = root();
        NumericValue<ValueType>* pNV = (NumericValue<ValueType>*)pRoot;
        ValueType *pVal = (ValueType*) (*pNV) ;
        return pVal;
    } 

};

template<class VT>
struct NumericTester: _BaseNumericTester<VT>
{
    NumericTester()
        : _BaseNumericTester<VT>()
    {
    }

    TESTER_METHOD(equal_to, (const ValueType &iVal))
    {
        return result(value() == iVal);
    }
    
    TESTER_METHOD(be_less_than, (const ValueType &iVal))
    {
        return result(value() < iVal);
    }
    
    TESTER_METHOD(be_greater_than, (const ValueType &iVal))
    {
        return result(value() > iVal);
    }  

    //TODO: 
    //ResultType be_in(const ValueType* iVal, int iNum)



};

template <class VT>
class NumericValue : public Value<VT, _NumericTester<VT>>
{
public:
    NumericValue(VT& iVal)
        : Value<VT, _NumericTester<VT>> (iVal)
    {
    }
};

#define NUMERIC_TESTER(VT) \
    typedef NumericValue<VT> Value##VT;    \
    Value##VT value(VT& iVal) { return Value##VT(iVal); }

NUMERIC_TESTER(char);
NUMERIC_TESTER(short);
NUMERIC_TESTER(int);
NUMERIC_TESTER(float);
NUMERIC_TESTER(double);

}; 