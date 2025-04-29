#include "filesystem.hpp"

#ifdef _WIN32

#include <Windows.h>
#include <ShlObj.h>
#include <direct.h>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace crossdev {
namespace fs {

// Path implementation
Path::Path(const std::string& path) : m_path(path) {
    // Normalize path separators to Windows style
    std::replace(m_path.begin(), m_path.end(), '/', '\\');
}

std::string Path::toString() const {
    return m_path;
}

std::string Path::getNative() const {
    return m_path;
}

bool Path::exists() const {
    DWORD attr = GetFileAttributesA(m_path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES);
}

bool Path::isDirectory() const {
    DWORD attr = GetFileAttributesA(m_path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool Path::isFile() const {
    DWORD attr = GetFileAttributesA(m_path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

Path Path::parent() const {
    size_t pos = m_path.find_last_of("\\/");
    if (pos == std::string::npos) {
        return Path("");
    }
    return Path(m_path.substr(0, pos));
}

std::string Path::filename() const {
    size_t pos = m_path.find_last_of("\\/");
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
    char buffer[MAX_PATH];
    DWORD length = GetTempPathA(MAX_PATH, buffer);
    if (length == 0) {
        throw FileSystemException("Could not get temp directory");
    }
    return Path(std::string(buffer));
}

Path Path::homeDirectory() {
    char buffer[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, buffer))) {
        return Path(std::string(buffer));
    }
    throw FileSystemException("Could not get home directory");
}

Path Path::currentDirectory() {
    char buffer[MAX_PATH];
    if (_getcwd(buffer, MAX_PATH) != nullptr) {
        return Path(std::string(buffer));
    }
    throw FileSystemException("Could not get current directory");
}

char Path::separator() {
    return '\\';
}

// File implementation
File::File(const Path& path) : m_path(path) {}

bool File::exists() const {
    return m_path.exists() && m_path.isFile();
}

size_t File::size() const {
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if (!GetFileAttributesExA(m_path.toString().c_str(), GetFileExInfoStandard, &fileData)) {
        throw FileSystemException("Could not get file size");
    }
    LARGE_INTEGER size;
    size.HighPart = fileData.nFileSizeHigh;
    size.LowPart = fileData.nFileSizeLow;
    return static_cast<size_t>(size.QuadPart);
}

std::string File::readAsText() const {
    std::ifstream file(m_path.toString(), std::ios::in);
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
    if (!CopyFileA(m_path.toString().c_str(), destination.toString().c_str(), FALSE)) {
        throw FileSystemException("Could not copy file");
    }
}

void File::move(const Path& destination) {
    if (!MoveFileA(m_path.toString().c_str(), destination.toString().c_str())) {
        throw FileSystemException("Could not move file");
    }
}

void File::remove() {
    if (!DeleteFileA(m_path.toString().c_str())) {
        throw FileSystemException("Could not delete file");
    }
}

// Directory implementation
Directory::Directory(const Path& path) : m_path(path) {}

bool Directory::exists() const {
    return m_path.exists() && m_path.isDirectory();
}

void Directory::create() {
    if (!CreateDirectoryA(m_path.toString().c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
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
    
    if (!RemoveDirectoryA(m_path.toString().c_str())) {
        throw FileSystemException("Could not remove directory");
    }
}

std::vector<Path> Directory::list(bool recursive) const {
    std::vector<Path> result;
    
    std::string pattern = m_path.toString() + "\\*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(pattern.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::string name = findData.cFileName;
            
            // Skip . and .. entries
            if (name == "." || name == "..") {
                continue;
            }
            
            Path fullPath(m_path.toString() + "\\" + name);
            result.push_back(fullPath);
            
            // Recursively process directories if requested
            if (recursive && (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::vector<Path> subDirContents = Directory(fullPath).list(true);
                result.insert(result.end(), subDirContents.begin(), subDirContents.end());
            }
        } while (FindNextFileA(hFind, &findData));
        
        FindClose(hFind);
    }
    
    return result;
}

} // namespace fs
} // namespace crossdev

#endif // _WIN32 