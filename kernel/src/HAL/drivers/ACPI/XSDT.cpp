#include "XSDT.hpp"

#include <Memory/PagingUtil.hpp>

ACPISDTHeader* g_XSDT;

bool InitAndValidateXSDT(void* XSDT) {
    if (XSDT == nullptr)
        return false;
    if (doChecksum(reinterpret_cast<ACPISDTHeader*>(XSDT))) {
        g_XSDT = (ACPISDTHeader*)to_HHDM(XSDT);
        return true;
    }
    return false;
}

ACPISDTHeader* getOtherSDT(uint64_t index) {
    uint64_t* start = (uint64_t*)((uint64_t)g_XSDT + sizeof(ACPISDTHeader));
    return (ACPISDTHeader*)(start[index]);
}