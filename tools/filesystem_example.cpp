#include "core/filesystem.hpp"
#include <iostream>

using namespace crossdev::fs;

int main() {
    try {
        std::cout << "CrossDev Filesystem Example\n";
        std::cout << "===========================\n";
        
        // Get system directories
        std::cout << "\nSystem Directories:\n";
        std::cout << "Current directory: " << Path::currentDirectory().toString() << "\n";
        std::cout << "Home directory: " << Path::homeDirectory().toString() << "\n";
        std::cout << "Temp directory: " << Path::tempDirectory().toString() << "\n";
        std::cout << "Path separator: '" << Path::separator() << "'\n";
        
        // Create a test directory in the temp folder
        Path testDir = Path::tempDirectory();
        testDir = Path(testDir.toString() + Path::separator() + "crossdev-test-cpp");
        std::cout << "\nCreating test directory: " << testDir.toString() << "\n";
        
        Directory dir(testDir);
        if (dir.exists()) {
            std::cout << "Directory already exists, removing it first...\n";
            dir.remove(true);
        }
        
        dir.create();
        std::cout << "Directory created.\n";
        
        // Create some files
        Path testFile1(testDir.toString() + Path::separator() + "test1.txt");
        Path testFile2(testDir.toString() + Path::separator() + "test2.txt");
        
        std::cout << "\nCreating test files:\n";
        std::cout << "- " << testFile1.toString() << "\n";
        std::cout << "- " << testFile2.toString() << "\n";
        
        File file1(testFile1);
        file1.writeText("Hello from CrossDev Toolkit!\nThis is a test file.");
        
        File file2(testFile2);
        file2.writeText("Another test file.\nWith multiple lines.");
        
        // Read file content
        std::cout << "\nReading file content:\n";
        std::cout << "- " << testFile1.toString() << ":\n";
        std::string content = file1.readAsText();
        std::cout << content << "\n";
        
        // Get file info
        size_t size = file1.size();
        std::cout << "File size: " << size << " bytes\n";
        
        // List directory contents
        std::cout << "\nDirectory contents:\n";
        std::vector<Path> contents = dir.list();
        for (const Path& entry : contents) {
            std::string type = entry.isDirectory() ? "Directory" : "File";
            std::cout << "- " << entry.filename() << " [" << type << "]\n";
        }
        
        // Copy a file
        Path copiedFile(testDir.toString() + Path::separator() + "test1-copy.txt");
        std::cout << "\nCopying " << testFile1.filename() << " to " << copiedFile.filename() << "\n";
        file1.copy(copiedFile);
        
        // Check if the copy exists
        bool copyExists = File(copiedFile).exists();
        std::cout << "Copy exists: " << (copyExists ? "yes" : "no") << "\n";
        
        // Create a subdirectory
        Path subDir(testDir.toString() + Path::separator() + "subdir");
        std::cout << "\nCreating subdirectory: " << subDir.toString() << "\n";
        Directory(subDir).create();
        
        // Create a file in the subdirectory
        Path subFile(subDir.toString() + Path::separator() + "subfile.txt");
        File(subFile).writeText("This is a file in a subdirectory.");
        
        // List directory contents recursively
        std::cout << "\nDirectory contents (recursive):\n";
        std::vector<Path> recursiveContents = dir.list(true);
        for (const Path& entry : recursiveContents) {
            std::string type = entry.isDirectory() ? "Directory" : "File";
            
            // Create a relative path for display
            std::string entryStr = entry.toString();
            std::string testDirStr = testDir.toString();
            
            std::string relativePath;
            if (entryStr.length() > testDirStr.length()) {
                // Remove the test directory prefix and the separator
                relativePath = entryStr.substr(testDirStr.length() + 1);
            } else {
                relativePath = entry.filename();
            }
            
            std::cout << "- " << relativePath << " [" << type << "]\n";
        }
        
        // Clean up
        std::cout << "\nCleaning up test directory...\n";
        dir.remove(true);
        
        bool exists = dir.exists();
        std::cout << "Test directory exists: " << (exists ? "yes" : "no") << "\n";
        
        std::cout << "\nExample completed successfully.\n";
        return 0;
    } catch (const FileSystemException& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "UNHANDLED EXCEPTION: " << e.what() << "\n";
        return 2;
    }
} 