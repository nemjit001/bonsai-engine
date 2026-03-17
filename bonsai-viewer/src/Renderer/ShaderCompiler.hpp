#pragma once
#ifndef SHADER_COMPILER_HPP
#define SHADER_COMPILER_HPP

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <atlbase.h>
#endif

#include <cstdint>
#include <string>
#include <filesystem>
#include <dxcapi.h>

#define DXC_ARG_GENERATE_SPIRV 						L"-spirv"
#define DXC_ARG_GENERATE_METAL 						L"-metal"
#define DXC_ARG_VK_USE_DX_LAYOUT 					L"-fvk-use-dx-layout"
#define DXC_ARG_VK_USE_GL_LAYOUT 					L"-fvk-use-gl-layout"
#define DXC_ARG_VK_USE_SCALAR_LAYOUT 				L"-fvk-use-scalar-layout"
#define DXC_ARG_VK_USE_DX_POSITION_W 				L"-fvk-use-dx-position-w"
#define DXC_ARG_VK_INVERT_Y 						L"-fvk-invert-y"
#define DXC_ARG_VK_SUPPORT_NONZERO_BASE_INSTANCE 	L"-fvk-support-nonzero-base-instance"
#define DXC_ARG_VK_SUPPORT_NONZERO_BASE_VERTEX 		L"-fvk-support-nonzero-base-vertex"
#define DXC_ARG_SPIRV_REFLECT						L"-spv-reflect"
#define DXC_ARG_SPIRV_TARGET_ENV(env)				L"-spv-target-env=" env
#define DXC_ARG_SPIRV_USE_EXTENSION(ext)			L"-fspv-extension=<value>" ext
#define DXC_ARG_SPIRV_ENABLE_MAXIMAL_RECONVERGENCE	L"-fspv-enable-maximal-reconvergence"
#define DXC_ARG_SPIRV_FLATTEN_RESOURCE_ARRAYS 		L"-fspv-flatten-resource-arrays"
#define DXC_ARG_SPIRV_PRESERVE_BINDINGS		 		L"-fspv-preserve-bindings"
#define DXC_ARG_SPIRV_PRESERVE_INTERFACE 			L"-fspv-preserve-interface"
#define DXC_ARG_SPIRV_REDUCE_LOAD_SIZE 				L"-fspv-reduce-load-size"
#define DXC_ARG_SPIRV_USE_LEGACY_BUFFER_MTX_ORDER	L"-fspv-use-legacy-buffer-matrix-order"
#define DXC_ARG_SPIRV_USE_UNKNOWN_IMAGE_FORMAT 		L"-fspv-use-unknown-image-format"
#define DXC_ARG_SPIRV_USE_VULKAN_MEMORY_MODEL 		L"-fspv-use-vulkan-memory-model"

/// @brief ShaderCompiler return codes.
enum ShaderCompilerResult : uint32_t
{
	SHADER_COMPILER_RESULT_OK					= 0,
	SHADER_COMPILER_RESULT_BAD_INIT				= 1,
	SHADER_COMPILER_RESULT_COMPILATION_FAILED	= 2,
};

/// @brief Shader compilation arguments.
struct BShaderCompilerArgs
{
	LPCWSTR const* Arguments;
	size_t ArgumentCount;
	LPCWSTR const* IncludePaths;
	size_t IncludePathCount;
	DxcDefine const* Defines;
	size_t DefineCount;
};

/// @brief The ShaderCompiler static class can used to compile shader source code into a binary representation using the DirectX Shader Compiler.
/// HLSL target profiles are used to specify shader type and capabilities.
class BShaderCompiler
{
public:
	/// @brief Compile a shader source file.
	/// @param FilePath 
	/// @param Entrypoint 
	/// @param TargetProfile  
	/// @param CompilerArgs 
	/// @param OutBlob 
	/// @return 
	static ShaderCompilerResult CompileFile(
		char const* FilePath,
		char const* Entrypoint,
		char const* TargetProfile,
		BShaderCompilerArgs const& CompilerArgs,
		IDxcBlob** OutBlob
	);

	/// @brief Compile a shader source string.
	/// This function does not support using #include directives.
	/// @param ShaderSource 
	/// @param Entrypoint 
	/// @param TargetProfile 
	/// @param CompilerArgs 
	/// @param OutBlob 
	/// @return 
	static ShaderCompilerResult CompileShaderSource(
		char const* ShaderSource,
		char const* Entrypoint,
		char const* TargetProfile,
		BShaderCompilerArgs const& CompilerArgs,
		IDxcBlob** OutBlob
	);

private:
	/// @brief Lazily initialize compiler infrastructure.
	/// @return 
	static bool LazyInit();

	/// @brief Convert a char string to a wide char string.
	/// @param str 
	/// @return 
	static std::wstring ToWideString(std::string const& str);

	/// @brief Compile a source buffer.
	/// @param SourceBuffer 
	/// @param CompilerArgs 
	/// @param OutBlob 
	/// @return 
	static ShaderCompilerResult Compile(
		DxcBuffer* SourceBuffer,
		IDxcCompilerArgs* CompilerArgs,
    	IDxcBlob** OutBlob
	);

private:
	static CComPtr<IDxcUtils> Utils;
	static CComPtr<IDxcCompiler3> Compiler;
	static CComPtr<IDxcIncludeHandler> IncludeHandler;
};

#endif //SHADER_COMPILER_HPP
