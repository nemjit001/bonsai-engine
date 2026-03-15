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
#include <dxcapi.h>
#include "Renderer.hpp"

/// @brief Available target shader langs for the ShaderCompiler.
enum ShaderLang : uint32_t
{
	ShaderLang_HLSL,
};

/// @brief Available compiler bytecode output types.
enum CompilerOutput : uint32_t
{
	CompilerOutput_SPIRV,
	CompilerOutput_DXIL,
};

/// @brief The ShaderCompiler static class is used to compile shader source into a binary representation.
class BShaderCompiler
{
public:
	/// @brief Compiler a shader source blob.
	/// @param InputLanguage Input shader language.
	/// @param Output Output representation.
	/// @param ShaderSource Input shader source.
	/// @param OutBlob Output shader blob containing bytecode.
	/// @return A boolean indicating successfull compilation.
	static bool Compile(ShaderLang InputLanguage, CompilerOutput Output, BShaderSource const& ShaderSource, IDxcBlob** OutBlob);
};

#endif //SHADER_COMPILER_HPP
