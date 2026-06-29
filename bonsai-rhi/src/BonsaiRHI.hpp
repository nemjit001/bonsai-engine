#pragma once
#ifndef BONSAI_RHI_HPP
#define BONSAI_RHI_HPP

#include <BonsaiBase.hpp>
#include "BonsaiRHIExport.hpp"

class BONSAI_RHI_EXPORT IGraphicsDevice
{
public:
    virtual ~IGraphicsDevice() = default;
};

#endif //BONSAI_RHI_HPP
