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

/// @brief ShaderCompiler return codes.
enum ShaderCompilerResult : uint32_t
{
	SHADER_COMPILER_RESULT_OK					= 0,
	SHADER_COMPILER_RESULT_BAD_INIT				= 1,
	SHADER_COMPILER_RESULT_PREPROCESS_FAILED	= 2,
	SHADER_COMPILER_RESULT_COMPILATION_FAILED	= 3,
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
private:
	static bool LazyInit();

public:
	/// @brief Preprocess shader source code using the shader compiler.
	/// @param ShaderSource Source code to preprocess.
	/// @param ShaderSourceLength 
	/// @param IncludeDirs Include directories to use for resolving include directives.
	/// @param IncludeDirCount 
	/// @param OutBlob Preprocessing output, regular string containing preprocessed source code.
	/// @return 
	static ShaderCompilerResult PreprocessShaderSource(
		char const* ShaderSource,
		size_t ShaderSourceLength,
		char const** IncludeDirs,
		size_t IncludeDirCount,
    	IDxcBlob** OutBlob
	);

	/// @brief Compile shader source code using the shader compiler.
	/// @param ShaderSource Source code to compile. For resolving include directives, preprocess source first.
	/// @param ShaderSourceLength 
	/// @param Arguments Additional command line arguments to use during compilation.
	/// @param ArgumentCount 
	/// @param OutBlob Compilation output, binary representation of shader source code that may be consumed by render backends.
	/// @return 
	static ShaderCompilerResult CompileShaderSource(
		char const* ShaderSource,
		size_t ShaderSourceLength,
		char const** Arguments,
		size_t ArgumentCount,
    	IDxcBlob** OutBlob
	);

private:
	static CComPtr<IDxcUtils> Utils;
	static CComPtr<IDxcCompiler3> Compiler;
	static CComPtr<IDxcIncludeHandler> IncludeHandler;
};

#endif //SHADER_COMPILER_HPP
