/**
 * CrossDev Toolkit - Filesystem Module Example
 * This example demonstrates how to use the filesystem module in JavaScript
 */

// Import the filesystem module
const { Path, File, Directory } = require('../../src/bindings/js/filesystem');

async function main() {
  try {
    console.log('CrossDev Filesystem Module Example');
    console.log('==================================');
    
    // Get system directories
    console.log(`\nSystem Directories:`);
    console.log(`Current directory: ${Path.currentDirectory().toString()}`);
    console.log(`Home directory: ${Path.homeDirectory().toString()}`);
    console.log(`Temp directory: ${Path.tempDirectory().toString()}`);
    console.log(`Path separator: "${Path.separator()}"`);
    
    // Create a test directory in the temp folder
    const testDir = new Path(Path.tempDirectory().toString() + Path.separator() + 'crossdev-test');
    console.log(`\nCreating test directory: ${testDir.toString()}`);
    
    const dir = new Directory(testDir);
    if (await dir.exists()) {
      console.log('Directory already exists, removing it first...');
      await dir.remove(true);
    }
    
    await dir.create();
    console.log('Directory created.');
    
    // Create some files
    const testFile1 = new Path(testDir.toString() + Path.separator() + 'test1.txt');
    const testFile2 = new Path(testDir.toString() + Path.separator() + 'test2.txt');
    
    console.log(`\nCreating test files:`);
    console.log(`- ${testFile1.toString()}`);
    console.log(`- ${testFile2.toString()}`);
    
    const file1 = new File(testFile1);
    await file1.writeText('Hello from CrossDev Toolkit!\nThis is a test file.');
    
    const file2 = new File(testFile2);
    await file2.writeText('Another test file.\nWith multiple lines.');
    
    // Read file content
    console.log(`\nReading file content:`);
    console.log(`- ${testFile1.toString()}:`);
    const content = await file1.readAsText();
    console.log(content);
    
    // Get file info
    const size = await file1.size();
    console.log(`File size: ${size} bytes`);
    
    // List directory contents
    console.log(`\nDirectory contents:`);
    const contents = await dir.list();
    for (const entry of contents) {
      const isDir = await entry.isDirectory();
      console.log(`- ${entry.filename()} [${isDir ? 'Directory' : 'File'}]`);
    }
    
    // Copy a file
    const copiedFile = new Path(testDir.toString() + Path.separator() + 'test1-copy.txt');
    console.log(`\nCopying ${testFile1.filename()} to ${copiedFile.filename()}`);
    await file1.copy(copiedFile);
    
    // Check if the copy exists
    const copyExists = await new File(copiedFile).exists();
    console.log(`Copy exists: ${copyExists}`);
    
    // Create a subdirectory
    const subDir = new Path(testDir.toString() + Path.separator() + 'subdir');
    console.log(`\nCreating subdirectory: ${subDir.toString()}`);
    await new Directory(subDir).create();
    
    // List directory contents recursively
    console.log(`\nDirectory contents (recursive):`);
    const recursiveContents = await dir.list(true);
    for (const entry of recursiveContents) {
      const isDir = await entry.isDirectory();
      const relativePath = entry.toString().slice(testDir.toString().length + 1);
      console.log(`- ${relativePath} [${isDir ? 'Directory' : 'File'}]`);
    }
    
    // Clean up
    console.log(`\nCleaning up test directory...`);
    await dir.remove(true);
    
    const exists = await dir.exists();
    console.log(`Test directory exists: ${exists}`);
    
    console.log('\nExample completed successfully.');
  } catch (err) {
    console.error(`ERROR: ${err.message}`);
  }
}

// Run the example
main(); 