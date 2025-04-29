/**
 * CrossDev Toolkit - Filesystem Module Tests
 */

const { Path, File, Directory } = require('../../src/bindings/js/filesystem');

describe('Path', () => {
  test('construction and basic operations', async () => {
    // Basic path creation
    const path = new Path('test/path/file.txt');
    
    // Path representation
    const expectedSeparator = process.platform === 'win32' ? '\\' : '/';
    expect(path.toString()).toContain('test');
    expect(path.toString()).toContain('path');
    expect(path.toString()).toContain('file.txt');
    
    // Path components
    expect(path.filename()).toBe('file.txt');
    expect(path.extension()).toBe('.txt');
    
    const parent = path.parent();
    expect(parent.toString()).toContain('test');
    expect(parent.toString()).toContain('path');
    expect(parent.toString()).not.toContain('file.txt');
  });
  
  test('static methods', async () => {
    // Current directory
    const currentDir = Path.currentDirectory();
    expect(await currentDir.exists()).toBe(true);
    expect(await currentDir.isDirectory()).toBe(true);
    
    // Temp directory
    const tempDir = Path.tempDirectory();
    expect(await tempDir.exists()).toBe(true);
    expect(await tempDir.isDirectory()).toBe(true);
    
    // Home directory
    const homeDir = Path.homeDirectory();
    expect(await homeDir.exists()).toBe(true);
    expect(await homeDir.isDirectory()).toBe(true);
    
    // Path separator
    const separator = Path.separator();
    expect(separator === '/' || separator === '\\').toBe(true);
  });
});

describe('File', () => {
  // Setup a test file
  let tempDir;
  let testFile;
  
  beforeEach(async () => {
    tempDir = Path.tempDirectory();
    testFile = new Path(tempDir.toString() + Path.separator() + 'crossdev-test-js-file.txt');
    
    // Clean up any previous test file
    if (await new File(testFile).exists()) {
      await new File(testFile).remove();
    }
  });
  
  afterEach(async () => {
    // Clean up
    if (await new File(testFile).exists()) {
      await new File(testFile).remove();
    }
  });
  
  test('file creation and read/write', async () => {
    const file = new File(testFile);
    const testContent = 'Hello, CrossDev Toolkit!';
    
    await file.writeText(testContent);
    
    expect(await file.exists()).toBe(true);
    expect(await testFile.exists()).toBe(true);
    expect(await testFile.isFile()).toBe(true);
    
    const readContent = await file.readAsText();
    expect(readContent).toBe(testContent);
    
    const size = await file.size();
    expect(size).toBe(testContent.length);
    
    await file.remove();
    expect(await file.exists()).toBe(false);
  });
  
  test('file copy and move', async () => {
    const file = new File(testFile);
    const testContent = 'Test content for copy and move operations';
    await file.writeText(testContent);
    
    // Copy the file
    const copyPath = new Path(tempDir.toString() + Path.separator() + 'crossdev-test-js-copy.txt');
    if (await new File(copyPath).exists()) {
      await new File(copyPath).remove();
    }
    
    await file.copy(copyPath);
    expect(await new File(copyPath).exists()).toBe(true);
    expect(await new File(copyPath).readAsText()).toBe(testContent);
    
    // Move the file
    const movePath = new Path(tempDir.toString() + Path.separator() + 'crossdev-test-js-moved.txt');
    if (await new File(movePath).exists()) {
      await new File(movePath).remove();
    }
    
    await new File(copyPath).move(movePath);
    expect(await new File(movePath).exists()).toBe(true);
    expect(await new File(copyPath).exists()).toBe(false);
    expect(await new File(movePath).readAsText()).toBe(testContent);
    
    // Clean up
    await new File(testFile).remove();
    await new File(movePath).remove();
  });
  
  test('binary file operations', async () => {
    const file = new File(testFile);
    const binaryContent = Buffer.from([
      0x48, 0x65, 0x6c, 0x6c, 0x6f, // "Hello" in ASCII
      0x00, 0x01, 0x02, 0x03, 0x04  // Some binary data
    ]);
    
    await file.writeBinary(binaryContent);
    
    const readContent = await file.readAsBinary();
    expect(readContent.length).toBe(binaryContent.length);
    expect(Buffer.compare(readContent, binaryContent)).toBe(0);
    
    await file.remove();
  });
});

describe('Directory', () => {
  // Setup a test directory
  let tempDir;
  let testDir;
  
  beforeEach(async () => {
    tempDir = Path.tempDirectory();
    testDir = new Path(tempDir.toString() + Path.separator() + 'crossdev-test-js-dir');
    
    // Clean up any previous test directory
    if (await new Directory(testDir).exists()) {
      await new Directory(testDir).remove(true);
    }
  });
  
  afterEach(async () => {
    // Clean up
    if (await new Directory(testDir).exists()) {
      await new Directory(testDir).remove(true);
    }
  });
  
  test('directory creation and existence', async () => {
    const dir = new Directory(testDir);
    await dir.create();
    
    expect(await dir.exists()).toBe(true);
    expect(await testDir.exists()).toBe(true);
    expect(await testDir.isDirectory()).toBe(true);
    
    await dir.remove();
    expect(await dir.exists()).toBe(false);
  });
  
  test('directory listing', async () => {
    // Create test directory with files
    const dir = new Directory(testDir);
    await dir.create();
    
    // Create some files in the test directory
    await new File(new Path(testDir.toString() + Path.separator() + 'file1.txt'))
      .writeText('File 1');
      
    await new File(new Path(testDir.toString() + Path.separator() + 'file2.txt'))
      .writeText('File 2');
    
    // Create a subdirectory
    const subDir = new Path(testDir.toString() + Path.separator() + 'subdir');
    await new Directory(subDir).create();
    
    await new File(new Path(subDir.toString() + Path.separator() + 'subfile.txt'))
      .writeText('Subfile');
    
    // List without recursion
    const contents = await dir.list(false);
    expect(contents.length).toBe(3); // file1.txt, file2.txt, subdir
    
    // List with recursion
    const recursiveContents = await dir.list(true);
    expect(recursiveContents.length).toBe(4); // file1.txt, file2.txt, subdir, subdir/subfile.txt
    
    // Clean up
    await dir.remove(true);
    expect(await dir.exists()).toBe(false);
  });
  
  test('recursive directory removal', async () => {
    // Create test directory with nested content
    const dir = new Directory(testDir);
    await dir.create();
    
    // Create a complex directory structure
    await new File(new Path(testDir.toString() + Path.separator() + 'file1.txt'))
      .writeText('File 1');
    
    const subDir1 = new Path(testDir.toString() + Path.separator() + 'subdir1');
    await new Directory(subDir1).create();
    
    await new File(new Path(subDir1.toString() + Path.separator() + 'subfile1.txt'))
      .writeText('Subfile 1');
    
    const subDir2 = new Path(testDir.toString() + Path.separator() + 'subdir2');
    await new Directory(subDir2).create();
    
    await new File(new Path(subDir2.toString() + Path.separator() + 'subfile2.txt'))
      .writeText('Subfile 2');
    
    const nestedDir = new Path(subDir1.toString() + Path.separator() + 'nested');
    await new Directory(nestedDir).create();
    
    await new File(new Path(nestedDir.toString() + Path.separator() + 'nested.txt'))
      .writeText('Nested file');
    
    // Verify structure exists
    expect(await dir.exists()).toBe(true);
    expect(await new Directory(subDir1).exists()).toBe(true);
    expect(await new Directory(subDir2).exists()).toBe(true);
    expect(await new Directory(nestedDir).exists()).toBe(true);
    
    // Remove recursively
    await dir.remove(true);
    
    // Verify all removed
    expect(await dir.exists()).toBe(false);
    expect(await new Directory(subDir1).exists()).toBe(false);
    expect(await new Directory(subDir2).exists()).toBe(false);
    expect(await new Directory(nestedDir).exists()).toBe(false);
  });
}); 