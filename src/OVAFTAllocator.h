#pragma once

#include <CubismFramework.hpp>
#include <ICubismAllocator.hpp>

class OVAFTAllocator : public Csm::ICubismAllocator {
    void *Allocate(Csm::csmSizeType size) override;

    void *AllocateAligned(Csm::csmSizeType size, Csm::csmUint32 alignment) override;

    void Deallocate(void *memory) override;

    void DeallocateAligned(void *alignedMemory) override;
};
