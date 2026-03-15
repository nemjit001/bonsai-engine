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
#include <filesystem>
#include <dxcapi.h>

#define TARGET_PROFILE_VERTEX_SHADER(version)			(L"vs_" version)
#define TARGET_PROFILE_HULL_SHADER(version)				(L"hs_" version)
#define TARGET_PROFILE_DOMAIN_SHADER(version)			(L"ds_" version)
#define TARGET_PROFILE_GEOMETRY_SHADER(version)			(L"gs_" version)
#define TARGET_PROFILE_PIXEL_SHADER(version)			(L"ps_" version)
#define TARGET_PROFILE_COMPUTE_SHADER(version)			(L"cs_" version)
#define TARGET_PROFILE_SHADER_LIBRARY(version)			(L"lib_" version)
#define TARGET_PROFILE_MESH_SHADER(version)				(L"ms_" version)
#define TARGET_PROFILE_AMPLIFICATION_SHADER(version)	(L"as_" version)

/// @brief ShaderCompiler return codes.
enum ShaderCompileResult : uint32_t
{
	SHADER_COMPILE_RESULT_OK					= 0,
	SHADER_COMPILE_RESULT_BAD_INIT				= 1,
	SHADER_COMPILE_RESULT_COMPILATION_FAILED	= 2,
};

/// @brief Available target shader langs for the ShaderCompiler.
enum ShaderLang : uint32_t
{
	ShaderLang_HLSL	= 0,
};

/// @brief Available compiler bytecode output types.
enum CompilerOutput : uint32_t
{
	CompilerOutput_DXIL		= 0,
	CompilerOutput_SPIRV	= 1,
};

/// @brief The ShaderCompiler static class can used to compile shader source code into a binary representation.
class BShaderCompiler
{
public:
	/// @brief Compile a shader source string into a binary representation.
	/// @param InputLanguage Input shader language.
	/// @param OutputType Compiler output representation.
	/// @param TargetProfile Shader target profile.
	/// @param Entrypoint Shader entrypoint name.
	/// @param ShaderSource Input shader source code.
	/// @param OutBlob Output shader blob containing bytecode.
	/// @return A ShaderCompileResult return code.
	static ShaderCompileResult Compile(
		ShaderLang InputLanguage,
		CompilerOutput OutputType,
		LPCWSTR TargetProfile,
		char const* Entrypoint,
		char const* ShaderSource,
		IDxcBlob** OutBlob
	);

private:
	static CComPtr<IDxcUtils> Utils;
	static CComPtr<IDxcCompiler3> Compiler;
	static CComPtr<IDxcIncludeHandler> IncludeHandler;
};

#endif //SHADER_COMPILER_HPP
