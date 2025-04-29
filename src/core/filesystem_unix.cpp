#include "filesystem.hpp"

#if defined(__unix__) || defined(__APPLE__)

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <pwd.h>

namespace crossdev {
namespace fs {

// Path implementation
Path::Path(const std::string& path) : m_path(path) {
    // Replace Windows backslashes with Unix forward slashes
    std::replace(m_path.begin(), m_path.end(), '\\', '/');
    
    // Remove trailing slash if not root
    if (m_path.size() > 1 && m_path.back() == '/') {
        m_path.pop_back();
    }
}

std::string Path::toString() const {
    return m_path;
}

std::string Path::getNative() const {
    return m_path;
}

bool Path::exists() const {
    struct stat buffer;
    return (stat(m_path.c_str(), &buffer) == 0);
}

bool Path::isDirectory() const {
    struct stat buffer;
    if (stat(m_path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

bool Path::isFile() const {
    struct stat buffer;
    if (stat(m_path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISREG(buffer.st_mode);
}

Path Path::parent() const {
    size_t pos = m_path.find_last_of('/');
    if (pos == std::string::npos) {
        return Path(".");
    }
    if (pos == 0) {
        return Path("/");
    }
    return Path(m_path.substr(0, pos));
}

std::string Path::filename() const {
    size_t pos = m_path.find_last_of('/');
    if (pos == std::string::npos) {
        return m_path;
    }
    return m_path.substr(pos + 1);
}

std::string Path::extension() const {
    std::string filename = this->filename();
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filename.substr(pos);
}

Path Path::tempDirectory() {
    const char* tmpdir = getenv("TMPDIR");
    if (tmpdir == nullptr) {
        tmpdir = "/tmp";
    }
    return Path(tmpdir);
}

Path Path::homeDirectory() {
    const char* home = getenv("HOME");
    if (home != nullptr) {
        return Path(home);
    }
    
    struct passwd* pw = getpwuid(getuid());
    if (pw != nullptr) {
        return Path(pw->pw_dir);
    }
    
    throw FileSystemException("Could not get home directory");
}

Path Path::currentDirectory() {
    char buffer[PATH_MAX];
    if (getcwd(buffer, PATH_MAX) != nullptr) {
        return Path(buffer);
    }
    throw FileSystemException("Could not get current directory");
}

char Path::separator() {
    return '/';
}

// File implementation
File::File(const Path& path) : m_path(path) {}

bool File::exists() const {
    return m_path.exists() && m_path.isFile();
}

size_t File::size() const {
    struct stat st;
    if (stat(m_path.toString().c_str(), &st) != 0) {
        throw FileSystemException("Could not get file size");
    }
    return static_cast<size_t>(st.st_size);
}

std::string File::readAsText() const {
    std::ifstream file(m_path.toString());
    if (!file.is_open()) {
        throw FileSystemException("Could not open file for reading");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<uint8_t> File::readAsBinary() const {
    std::ifstream file(m_path.toString(), std::ios::binary);
    if (!file.is_open()) {
        throw FileSystemException("Could not open file for reading");
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    
    return buffer;
}

void File::writeText(const std::string& content) {
    std::ofstream file(m_path.toString());
    if (!file.is_open()) {
        throw FileSystemException("Could not open file for writing");
    }
    file << content;
}

void File::writeBinary(const std::vector<uint8_t>& content) {
    std::ofstream file(m_path.toString(), std::ios::binary);
    if (!file.is_open()) {
        throw FileSystemException("Could not open file for writing");
    }
    file.write(reinterpret_cast<const char*>(content.data()), content.size());
}

void File::copy(const Path& destination) {
    std::ifstream src(m_path.toString(), std::ios::binary);
    if (!src) {
        throw FileSystemException("Could not open source file for copying");
    }
    
    std::ofstream dst(destination.toString(), std::ios::binary);
    if (!dst) {
        throw FileSystemException("Could not open destination file for copying");
    }
    
    dst << src.rdbuf();
}

void File::move(const Path& destination) {
    if (rename(m_path.toString().c_str(), destination.toString().c_str()) != 0) {
        // If rename fails (possibly due to cross-device link), try copy and delete
        copy(destination);
        remove();
    }
}

void File::remove() {
    if (::unlink(m_path.toString().c_str()) != 0) {
        throw FileSystemException("Could not delete file");
    }
}

// Directory implementation
Directory::Directory(const Path& path) : m_path(path) {}

bool Directory::exists() const {
    return m_path.exists() && m_path.isDirectory();
}

void Directory::create() {
    if (mkdir(m_path.toString().c_str(), 0755) != 0 && errno != EEXIST) {
        throw FileSystemException("Could not create directory");
    }
}

void Directory::remove(bool recursive) {
    if (recursive) {
        // First remove contents
        std::vector<Path> contents = list(false);
        for (const Path& entry : contents) {
            if (entry.isDirectory()) {
                Directory(entry).remove(true);
            } else {
                File(entry).remove();
            }
        }
    }
    
    if (rmdir(m_path.toString().c_str()) != 0) {
        throw FileSystemException("Could not remove directory");
    }
}

std::vector<Path> Directory::list(bool recursive) const {
    std::vector<Path> result;
    
    DIR* dir = opendir(m_path.toString().c_str());
    if (!dir) {
        return result;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        
        // Skip . and .. entries
        if (name == "." || name == "..") {
            continue;
        }
        
        Path fullPath(m_path.toString() + "/" + name);
        result.push_back(fullPath);
        
        // Recursively process directories if requested
        if (recursive && fullPath.isDirectory()) {
            std::vector<Path> subDirContents = Directory(fullPath).list(true);
            result.insert(result.end(), subDirContents.begin(), subDirContents.end());
        }
    }
    
    closedir(dir);
    return result;
}

} // namespace fs
} // namespace crossdev

#endif // __unix__ || __APPLE__ 