#include "OVAFTAllocator.h"

using namespace Csm;

void *OVAFTAllocator::Allocate(csmSizeType size) {
    return malloc(size);
}

void *OVAFTAllocator::AllocateAligned(csmSizeType size, csmUint32 alignment) {
    size_t offset, shift, alignedAddress;
    void *allocation;
    void **preamble;

    offset = alignment - 1 + sizeof(void *);

    allocation = Allocate(size + static_cast<csmUint32>(offset));

    alignedAddress = reinterpret_cast<size_t>(allocation) + sizeof(void *);

    shift = alignedAddress % alignment;

    if (shift) {
        alignedAddress += (alignment - shift);
    }

    preamble = reinterpret_cast<void **>(alignedAddress);
    preamble[-1] = allocation;

    return reinterpret_cast<void *>(alignedAddress);
}

void OVAFTAllocator::Deallocate(void *memory) {
    free(memory);
}

void OVAFTAllocator::DeallocateAligned(void *alignedMemory) {
    void **preamble;

    preamble = static_cast<void **>(alignedMemory);

    Deallocate(preamble[-1]);
}
