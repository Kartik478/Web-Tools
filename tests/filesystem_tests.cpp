#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "core/filesystem.hpp"

using namespace crossdev::fs;

TEST_CASE("Path construction and basic operations", "[path]") {
    // Basic path construction
    Path path1("test/path/file.txt");
    
    SECTION("Path representation") {
        // These tests are platform-dependent, so we check both options
        const char separator = Path::separator();
        const std::string expectedPath = separator == '/' 
            ? "test/path/file.txt" 
            : "test\\path\\file.txt";
            
        REQUIRE(path1.toString().find("test") != std::string::npos);
        REQUIRE(path1.toString().find("path") != std::string::npos);
        REQUIRE(path1.toString().find("file.txt") != std::string::npos);
    }
    
    SECTION("Path components") {
        REQUIRE(path1.filename() == "file.txt");
        REQUIRE(path1.extension() == ".txt");
        
        Path parent = path1.parent();
        std::string parentStr = parent.toString();
        REQUIRE((parentStr.find("test") != std::string::npos && parentStr.find("path") != std::string::npos));
        REQUIRE(parentStr.find("file.txt") == std::string::npos);
    }
}

TEST_CASE("Path static methods", "[path]") {
    SECTION("Current directory") {
        Path currentDir = Path::currentDirectory();
        REQUIRE(currentDir.exists());
        REQUIRE(currentDir.isDirectory());
    }
    
    SECTION("Temp directory") {
        Path tempDir = Path::tempDirectory();
        REQUIRE(tempDir.exists());
        REQUIRE(tempDir.isDirectory());
    }
    
    SECTION("Home directory") {
        Path homeDir = Path::homeDirectory();
        REQUIRE(homeDir.exists());
        REQUIRE(homeDir.isDirectory());
    }
    
    SECTION("Path separator") {
        char separator = Path::separator();
        REQUIRE((separator == '/' || separator == '\\'));
    }
}

TEST_CASE("File operations", "[file]") {
    // Set up a test file in the temp directory
    Path tempDir = Path::tempDirectory();
    Path testFile = Path(tempDir.toString() + Path::separator() + "crossdev-test-file.txt");
    
    // Clean up any previous test file
    if (File(testFile).exists()) {
        File(testFile).remove();
    }
    
    SECTION("File creation and read/write") {
        // Write content
        File file(testFile);
        std::string testContent = "Hello, CrossDev Toolkit!";
        file.writeText(testContent);
        
        // Verify file exists
        REQUIRE(file.exists());
        REQUIRE(testFile.exists());
        REQUIRE(testFile.isFile());
        
        // Read and verify content
        std::string readContent = file.readAsText();
        REQUIRE(readContent == testContent);
        
        // Check file size
        REQUIRE(file.size() == testContent.size());
        
        // Clean up
        file.remove();
        REQUIRE_FALSE(file.exists());
    }
    
    SECTION("File copy and move") {
        // Create a test file
        File file(testFile);
        std::string testContent = "Test content for copy and move operations";
        file.writeText(testContent);
        
        // Copy the file
        Path copyPath = Path(tempDir.toString() + Path::separator() + "crossdev-test-copy.txt");
        if (File(copyPath).exists()) {
            File(copyPath).remove();
        }
        
        file.copy(copyPath);
        REQUIRE(File(copyPath).exists());
        REQUIRE(File(copyPath).readAsText() == testContent);
        
        // Move the file
        Path movePath = Path(tempDir.toString() + Path::separator() + "crossdev-test-moved.txt");
        if (File(movePath).exists()) {
            File(movePath).remove();
        }
        
        File(copyPath).move(movePath);
        REQUIRE(File(movePath).exists());
        REQUIRE_FALSE(File(copyPath).exists());
        REQUIRE(File(movePath).readAsText() == testContent);
        
        // Clean up
        File(testFile).remove();
        File(movePath).remove();
    }
    
    SECTION("Binary file operations") {
        // Create binary content
        std::vector<uint8_t> binaryContent = {
            0x48, 0x65, 0x6c, 0x6c, 0x6f, // "Hello" in ASCII
            0x00, 0x01, 0x02, 0x03, 0x04  // Some binary data
        };
        
        // Write binary content
        File file(testFile);
        file.writeBinary(binaryContent);
        
        // Read and verify
        std::vector<uint8_t> readContent = file.readAsBinary();
        REQUIRE(readContent.size() == binaryContent.size());
        REQUIRE(readContent == binaryContent);
        
        // Clean up
        file.remove();
    }
}

TEST_CASE("Directory operations", "[directory]") {
    // Set up test directory in the temp folder
    Path tempDir = Path::tempDirectory();
    Path testDir = Path(tempDir.toString() + Path::separator() + "crossdev-test-dir");
    
    // Clean up any previous test directory
    if (Directory(testDir).exists()) {
        Directory(testDir).remove(true);
    }
    
    SECTION("Directory creation and existence") {
        Directory dir(testDir);
        dir.create();
        
        REQUIRE(dir.exists());
        REQUIRE(testDir.exists());
        REQUIRE(testDir.isDirectory());
        
        // Clean up
        dir.remove();
        REQUIRE_FALSE(dir.exists());
    }
    
    SECTION("Directory listing") {
        // Create test directory with files
        Directory dir(testDir);
        dir.create();
        
        // Create some files in the test directory
        File(Path(testDir.toString() + Path::separator() + "file1.txt")).writeText("File 1");
        File(Path(testDir.toString() + Path::separator() + "file2.txt")).writeText("File 2");
        
        // Create a subdirectory
        Path subDir = Path(testDir.toString() + Path::separator() + "subdir");
        Directory(subDir).create();
        File(Path(subDir.toString() + Path::separator() + "subfile.txt")).writeText("Subfile");
        
        // List without recursion
        std::vector<Path> contents = dir.list(false);
        REQUIRE(contents.size() == 3); // file1.txt, file2.txt, subdir
        
        // List with recursion
        std::vector<Path> recursiveContents = dir.list(true);
        REQUIRE(recursiveContents.size() == 4); // file1.txt, file2.txt, subdir, subdir/subfile.txt
        
        // Clean up
        dir.remove(true);
        REQUIRE_FALSE(dir.exists());
    }
    
    SECTION("Recursive directory removal") {
        // Create test directory with nested content
        Directory dir(testDir);
        dir.create();
        
        // Create a complex directory structure
        File(Path(testDir.toString() + Path::separator() + "file1.txt")).writeText("File 1");
        
        Path subDir1 = Path(testDir.toString() + Path::separator() + "subdir1");
        Directory(subDir1).create();
        File(Path(subDir1.toString() + Path::separator() + "subfile1.txt")).writeText("Subfile 1");
        
        Path subDir2 = Path(testDir.toString() + Path::separator() + "subdir2");
        Directory(subDir2).create();
        File(Path(subDir2.toString() + Path::separator() + "subfile2.txt")).writeText("Subfile 2");
        
        Path nestedDir = Path(subDir1.toString() + Path::separator() + "nested");
        Directory(nestedDir).create();
        File(Path(nestedDir.toString() + Path::separator() + "nested.txt")).writeText("Nested file");
        
        // Verify structure exists
        REQUIRE(dir.exists());
        REQUIRE(Directory(subDir1).exists());
        REQUIRE(Directory(subDir2).exists());
        REQUIRE(Directory(nestedDir).exists());
        
        // Remove recursively
        dir.remove(true);
        
        // Verify all removed
        REQUIRE_FALSE(dir.exists());
        REQUIRE_FALSE(Directory(subDir1).exists());
        REQUIRE_FALSE(Directory(subDir2).exists());
        REQUIRE_FALSE(Directory(nestedDir).exists());
    }
} 