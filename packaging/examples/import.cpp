// Uses the headers #import makes of the type library midl made from
// smoke.odl: the smart pointer, the GUID behind __uuidof and the wrapper
// methods, which call the COM support library.
#include <stdio.h>
#include <windows.h>
#include "smoke.h"
#include "smoke.tlh"

int main()
{
    SmokeLib::ISmokePtr p;
    // The wrappers are linked, with the dispatch call they make.
    long (SmokeLib::ISmoke::*add)(long) = &SmokeLib::ISmoke::Add;
    bool same = IsEqualGUID(__uuidof(SmokeLib::ISmoke), DIID_ISmoke) != 0;
    printf("import SmokeLib::ISmoke %s\n", same && add && !p ? "ok" : "wrong");
    return same ? 0 : 1;
}
