# Contributing to CrossDev Toolkit

Thank you for your interest in contributing to CrossDev Toolkit! This document provides guidelines and instructions for contributing to this project.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Workflow](#development-workflow)
4. [Coding Standards](#coding-standards)
5. [Testing](#testing)
6. [Pull Requests](#pull-requests)
7. [Release Process](#release-process)

## Code of Conduct

By participating in this project, you agree to abide by the [Code of Conduct](CODE_OF_CONDUCT.md).

## Getting Started

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/your-username/crossdev-toolkit.git
   cd crossdev-toolkit
   ```
3. Set up the development environment:
   ```bash
   # Create a build directory for C++
   mkdir build
   cd build
   cmake .. -DBUILD_TESTS=ON
   cmake --build .
   
   # For JavaScript development
   cd .. # back to project root
   npm install
   ```

## Development Workflow

1. Create a new branch for your feature or bugfix:
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/issue-description
   ```

2. Make your changes

3. Run tests to ensure your changes don't break existing functionality:
   ```bash
   # C++ tests
   cd build
   ctest
   
   # JavaScript tests
   npm test
   ```

4. Commit your changes with a descriptive commit message:
   ```bash
   git commit -m "Add feature: your feature description"
   ```

5. Push your branch to your fork:
   ```bash
   git push origin feature/your-feature-name
   ```

6. Create a Pull Request from your fork to the original repository

## Coding Standards

### C++

- Follow the C++17 standard
- Use the existing code style (indentation, braces, etc.)
- Document all public APIs with comments
- Avoid platform-specific code outside of platform-specific files
- Use strong typing and avoid raw pointers when possible

### JavaScript

- Follow modern ES6+ syntax
- Document functions and classes with JSDoc comments
- Use async/await for asynchronous code
- Follow the existing project structure

## Testing

All new features and bug fixes should include tests. We use:

- For C++: Native testing framework integrated with CTest
- For JavaScript: Jest testing framework

## Pull Requests

When submitting a pull request:

1. Update the README.md with details of changes to the interface, if applicable
2. Update the documentation with details of any changes
3. The pull request should work for all supported platforms (Windows, macOS, Linux)
4. Include appropriate tests for your changes

## Release Process

The project maintainers will handle the release process, including:

1. Versioning (using Semantic Versioning)
2. Changelog updates
3. Building and publishing releases

## Questions?

If you have any questions or need help, please open an issue in the repository. 