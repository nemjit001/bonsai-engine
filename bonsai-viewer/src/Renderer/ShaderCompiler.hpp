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

/// @brief ShaderCompiler return codes.
enum ShaderCompilerResult : uint32_t
{
	SHADER_COMPILER_RESULT_OK					= 0,
	SHADER_COMPILER_RESULT_BAD_INIT				= 1,
	SHADER_COMPILER_RESULT_COMPILATION_FAILED	= 2,
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
	/// @param OutBlob 
	/// @return 
	static ShaderCompilerResult CompileFile(
		char const* FilePath,
		char const* Entrypoint,
		char const* TargetProfile,
		IDxcBlob** OutBlob
	);

	/// @brief Compile a shader source string.
	/// @param ShaderSource 
	/// @param Entrypoint 
	/// @param TargetProfile 
	/// @param OutBlob 
	/// @return 
	static ShaderCompilerResult CompileShaderSource(
		char const* ShaderSource,
		char const* Entrypoint,
		char const* TargetProfile,
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
