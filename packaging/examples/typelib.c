/* Loads the type library midl made from smoke.odl, and uses the header. */
#include <stdio.h>
#include <windows.h>
#include <oleauto.h>
#include "smoke.h"

int main(void)
{
    ITypeLib *tl = NULL;
    ITypeInfo *ti = NULL;
    TYPEATTR *ta = NULL;
    BSTR name = NULL;
    if (FAILED(LoadTypeLibEx(L"smoke.tlb", REGKIND_NONE, &tl))) {
        printf("LoadTypeLibEx failed\n");
        return 1;
    }
    tl->lpVtbl->GetDocumentation(tl, -1, &name, NULL, NULL, NULL);
    if (SUCCEEDED(tl->lpVtbl->GetTypeInfoOfGuid(tl, &DIID_ISmoke, &ti)) &&
        SUCCEEDED(ti->lpVtbl->GetTypeAttr(ti, &ta))) {
        printf("typelib %ls: %u types, ISmoke %d functions %d properties\n", name,
               tl->lpVtbl->GetTypeInfoCount(tl), ta->cFuncs, ta->cVars);
        ti->lpVtbl->ReleaseTypeAttr(ti, ta);
        ti->lpVtbl->Release(ti);
    }
    SysFreeString(name);
    tl->lpVtbl->Release(tl);
    return 0;
}
