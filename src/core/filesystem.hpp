#ifndef CROSSDEV_FILESYSTEM_HPP
#define CROSSDEV_FILESYSTEM_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

namespace crossdev {
namespace fs {

/**
 * Exception class for filesystem operations
 */
class FileSystemException : public std::runtime_error {
public:
    explicit FileSystemException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * Path representation with platform-specific handling
 */
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
    
private:
    std::string m_path;
};

/**
 * File operations
 */
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
    
private:
    Path m_path;
};

/**
 * Directory operations
 */
class Directory {
public:
    explicit Directory(const Path& path);
    
    bool exists() const;
    void create();
    void remove(bool recursive = false);
    std::vector<Path> list(bool recursive = false) const;
    
private:
    Path m_path;
};

} // namespace fs
} // namespace crossdev

#endif // CROSSDEV_FILESYSTEM_HPP 