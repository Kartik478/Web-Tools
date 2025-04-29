# CrossDev Toolkit Documentation

This documentation provides information about using and extending the CrossDev Toolkit, a comprehensive suite of cross-platform developer tools for Windows, Linux, and macOS.

## Table of Contents

1. [Installation](#installation)
2. [Filesystem Module](#filesystem-module)
   - [C++ API](#c-api)
   - [JavaScript API](#javascript-api)
3. [Building from Source](#building-from-source)
4. [Examples](#examples)

## Installation

### C++ Library

To install the C++ library:

```bash
# Clone the repository
git clone https://github.com/yourusername/crossdev-toolkit.git
cd crossdev-toolkit

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Install
sudo cmake --install .
```

### JavaScript Package

To install the JavaScript package:

```bash
npm install crossdev-toolkit
```

## Filesystem Module

The filesystem module provides a consistent API for working with files and directories across different operating systems.

### C++ API

#### Path Class

```cpp
namespace crossdev {
namespace fs {

class Path {
public:
    Path(const std::string& path);
    
    std::string toString() const;
    std::string getNative() const;
    bool exists() const;
    bool isDirectory() const;
    bool isFile() const;
    Path parent() const;
    std::string filename() const;
    std::string extension() const;
    
    static Path tempDirectory();
    static Path homeDirectory();
    static Path currentDirectory();
    static char separator();
};

} // namespace fs
} // namespace crossdev
```

#### File Class

```cpp
namespace crossdev {
namespace fs {

class File {
public:
    explicit File(const Path& path);
    
    bool exists() const;
    size_t size() const;
    std::string readAsText() const;
    std::vector<uint8_t> readAsBinary() const;
    void writeText(const std::string& content);
    void writeBinary(const std::vector<uint8_t>& content);
    void copy(const Path& destination);
    void move(const Path& destination);
    void remove();
};

} // namespace fs
} // namespace crossdev
```

#### Directory Class

```cpp
namespace crossdev {
namespace fs {

class Directory {
public:
    explicit Directory(const Path& path);
    
    bool exists() const;
    void create();
    void remove(bool recursive = false);
    std::vector<Path> list(bool recursive = false) const;
};

} // namespace fs
} // namespace crossdev
```

### JavaScript API

#### Path Class

```javascript
const { Path } = require('crossdev-toolkit');

// Create a path object
const path = new Path('/path/to/file.txt');

// Path operations
const exists = await path.exists();
const isDir = await path.isDirectory();
const isFile = await path.isFile();
const parent = path.parent();
const filename = path.filename();
const ext = path.extension();

// Static methods
const tempDir = Path.tempDirectory();
const homeDir = Path.homeDirectory();
const currentDir = Path.currentDirectory();
const separator = Path.separator(); // '/' on Unix, '\' on Windows
```

#### File Class

```javascript
const { File } = require('crossdev-toolkit');

// Create a file object
const file = new File('/path/to/file.txt');

// File operations
const exists = await file.exists();
const size = await file.size();
const textContent = await file.readAsText();
const binaryContent = await file.readAsBinary();

// Write to a file
await file.writeText('Hello, world!');
await file.writeBinary(Buffer.from([0x48, 0x65, 0x6c, 0x6c, 0x6f]));

// Move, copy, remove
await file.copy('/path/to/copy.txt');
await file.move('/path/to/moved.txt');
await file.remove();
```

#### Directory Class

```javascript
const { Directory } = require('crossdev-toolkit');

// Create a directory object
const dir = new Directory('/path/to/directory');

// Directory operations
const exists = await dir.exists();
await dir.create();

// List contents
const contents = await dir.list();
for (const path of contents) {
    console.log(path.toString());
}

// Recursive listing
const allContents = await dir.list(true);

// Remove directory
await dir.remove(true); // true for recursive deletion
```

## Building from Source

### Prerequisites

- C++17 compatible compiler
- CMake 3.12 or higher
- Node.js 12 or higher (for JavaScript bindings)

### Building the C++ Library

```bash
# Clone the repository
git clone https://github.com/yourusername/crossdev-toolkit.git
cd crossdev-toolkit

# Create a build directory
mkdir build
cd build

# Configure the build
cmake ..

# Build the library
cmake --build .
```

### Building with Node.js Bindings

```bash
# Configure with Node.js bindings enabled
cmake .. -DBUILD_NODE_MODULES=ON

# Build everything
cmake --build .
```

## Examples

Check out the examples in the `docs/examples` directory:

- `js_filesystem_example.js` - JavaScript filesystem example
- `filesystem_example.cpp` - C++ filesystem example

To run the JavaScript example:

```bash
npm run example
```

To run the C++ example:

```bash
cd build
./bin/filesystem_example
``` 