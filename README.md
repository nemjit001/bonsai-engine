# Bonsai Renderer
[![CI Tests](https://github.com/nemjit001/bonsai-renderer/actions/workflows/ci_tests.yml/badge.svg?branch=main)](https://github.com/nemjit001/bonsai-renderer/actions/workflows/ci_tests.yml)

Flexible application & rendering framework with a focus on performance.
Bonsai aims to be a flexible software package that allows easy implementation of
custom rendering applications.

## Supported platforms

Currently Bonsai supports 64-bit Windows and Linux. Cross-platform compatability is kept
in mind during development, ensuring similar performance across the supported platforms.

## Building Bonsai

Bonsai should build out of the box when using CMake 3.25 or higher. Dependencies are managed through vcpkg,
making the build as straightforward as these two commands:

```
$ cmake . --preset <preset name>
```
```
$ cmake --build --preset <preset name>
```

## Dependencies

Bonsai aims to have minimal dependencies, including only what is needed. Applications can choose to pull in additional
dependencies if they so require.

## License

Bonsai is licensed under the MIT license.
