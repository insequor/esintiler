// unit_test.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include <map>
#include <vector>

#include "../include/suite.h"
using namespace esintiler;


int main(int argc, char* argv[])
{
    int retVal = TestManager::ExecuteAllSuites();
    //int retVal = TestManager::ExecuteSuite("SpecObject");
    //int retVal = TestManager::ExecuteSuite("Numeric");
    printf("%i\n", retVal);
}

