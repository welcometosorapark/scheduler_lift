#include "core.h"

using namespace ThorTower;
int main()
{
    core* scheduler = new (nothrow) core;
    try
    {
        if (scheduler)
        {
            int8_t* t = (int8_t*)calloc(1, sizeof(int8_t));
            if (t)
            {
                while ((*t = scheduler->getInput()) != TERMINATE_CODE)
                {

                }
                delete t;
            }
            delete scheduler;
        }
    }
    catch (const exception & e)
    {
        if (scheduler)
        {
            scheduler->func.msgError(e, "main");
        }
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
