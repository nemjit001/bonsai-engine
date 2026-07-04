#pragma once
#ifndef BONSAI_RHI_TYPES_HPP
#define BONSAI_RHI_TYPES_HPP

#include "BonsaiRHI.hpp"

namespace RHI
{
    /// @brief The GPUResource class is used as a base class for all RHI resource types.
    class GPUResource
    {
    public:
        explicit constexpr GPUResource(BackendAPI backendAPI)
            : _backendAPI(backendAPI) {}
        virtual ~GPUResource() = default;

        /// @brief Get the backend API that created this GPU resource.
        /// @return 
        BackendAPI GetBackendAPI() const { return _backendAPI; }

    protected:
        BackendAPI _backendAPI;
    };

    class GPUBuffer : public GPUResource
    {
    public:
        explicit constexpr GPUBuffer(BackendAPI backendAPI)
            : GPUResource(backendAPI) {}
    };

    class GPUTexture : public GPUResource
    {
    public:
        explicit constexpr GPUTexture(BackendAPI backendAPI)
            : GPUResource(backendAPI) {}
    };

    class GPUSampler : public GPUResource
    {
    public:
        explicit constexpr GPUSampler(BackendAPI backendAPI)
            : GPUResource(backendAPI) {}
    };

    class GPUPipeline : public GPUResource
    {
    public:
        explicit constexpr GPUPipeline(BackendAPI backendAPI)
            : GPUResource(backendAPI) {}
    };
} // namespace RHI

#endif //BONSAI_RHI_TYPES_HPP
