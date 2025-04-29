/**
 * CrossDev Toolkit - Filesystem Module
 * JavaScript bindings for the C++ filesystem core
 */

// This is a simplified version - in a real implementation,
// this would use Node.js native module bindings or WebAssembly

class Path {
  /**
   * Create a new path instance
   * @param {string} pathString - The path string
   */
  constructor(pathString) {
    this._path = pathString;
    this._normalizedPath = this._normalizePath(pathString);
  }

  /**
   * Normalize path based on current platform
   * @private
   */
  _normalizePath(path) {
    // Simple normalization for cross-platform paths
    const isWindows = process.platform === 'win32';
    const separator = isWindows ? '\\' : '/';
    
    // Replace all path separators with the platform-specific one
    let normalized = path.replace(/[\\\/]/g, separator);
    
    // Remove trailing slashes except for root
    if (normalized.length > 1 && normalized.endsWith(separator)) {
      normalized = normalized.slice(0, -1);
    }
    
    return normalized;
  }

  /**
   * Get string representation of the path
   * @returns {string} The path as a string
   */
  toString() {
    return this._normalizedPath;
  }

  /**
   * Check if path exists
   * @returns {Promise<boolean>} True if path exists
   */
  async exists() {
    const fs = require('fs').promises;
    try {
      await fs.access(this._normalizedPath);
      return true;
    } catch {
      return false;
    }
  }

  /**
   * Check if path is a directory
   * @returns {Promise<boolean>} True if path is a directory
   */
  async isDirectory() {
    const fs = require('fs').promises;
    try {
      const stat = await fs.stat(this._normalizedPath);
      return stat.isDirectory();
    } catch {
      return false;
    }
  }

  /**
   * Check if path is a file
   * @returns {Promise<boolean>} True if path is a file
   */
  async isFile() {
    const fs = require('fs').promises;
    try {
      const stat = await fs.stat(this._normalizedPath);
      return stat.isFile();
    } catch {
      return false;
    }
  }

  /**
   * Get parent directory path
   * @returns {Path} Parent directory path
   */
  parent() {
    const path = require('path');
    return new Path(path.dirname(this._normalizedPath));
  }

  /**
   * Get filename component of path
   * @returns {string} Filename
   */
  filename() {
    const path = require('path');
    return path.basename(this._normalizedPath);
  }

  /**
   * Get file extension
   * @returns {string} File extension with dot
   */
  extension() {
    const path = require('path');
    return path.extname(this._normalizedPath);
  }

  /**
   * Get system temp directory
   * @returns {Path} System temp directory
   */
  static tempDirectory() {
    const os = require('os');
    return new Path(os.tmpdir());
  }

  /**
   * Get user home directory
   * @returns {Path} User home directory
   */
  static homeDirectory() {
    const os = require('os');
    return new Path(os.homedir());
  }

  /**
   * Get current working directory
   * @returns {Path} Current working directory
   */
  static currentDirectory() {
    return new Path(process.cwd());
  }

  /**
   * Get platform-specific path separator
   * @returns {string} Path separator
   */
  static separator() {
    return process.platform === 'win32' ? '\\' : '/';
  }
}

class File {
  /**
   * Create a new file instance
   * @param {Path|string} path - Path to the file
   */
  constructor(path) {
    this._path = path instanceof Path ? path : new Path(path);
  }

  /**
   * Check if file exists
   * @returns {Promise<boolean>} True if file exists
   */
  async exists() {
    return this._path.exists() && this._path.isFile();
  }

  /**
   * Get file size in bytes
   * @returns {Promise<number>} File size in bytes
   */
  async size() {
    const fs = require('fs').promises;
    try {
      const stat = await fs.stat(this._path.toString());
      return stat.size;
    } catch (err) {
      throw new Error(`Could not get file size: ${err.message}`);
    }
  }

  /**
   * Read file as text
   * @returns {Promise<string>} File contents as string
   */
  async readAsText() {
    const fs = require('fs').promises;
    try {
      return await fs.readFile(this._path.toString(), 'utf8');
    } catch (err) {
      throw new Error(`Could not read file: ${err.message}`);
    }
  }

  /**
   * Read file as binary
   * @returns {Promise<Buffer>} File contents as buffer
   */
  async readAsBinary() {
    const fs = require('fs').promises;
    try {
      return await fs.readFile(this._path.toString());
    } catch (err) {
      throw new Error(`Could not read file: ${err.message}`);
    }
  }

  /**
   * Write text to file
   * @param {string} content - Content to write
   * @returns {Promise<void>}
   */
  async writeText(content) {
    const fs = require('fs').promises;
    try {
      await fs.writeFile(this._path.toString(), content, 'utf8');
    } catch (err) {
      throw new Error(`Could not write to file: ${err.message}`);
    }
  }

  /**
   * Write binary data to file
   * @param {Buffer|Uint8Array} content - Content to write
   * @returns {Promise<void>}
   */
  async writeBinary(content) {
    const fs = require('fs').promises;
    try {
      await fs.writeFile(this._path.toString(), content);
    } catch (err) {
      throw new Error(`Could not write to file: ${err.message}`);
    }
  }

  /**
   * Copy file to destination
   * @param {Path|string} destination - Destination path
   * @returns {Promise<void>}
   */
  async copy(destination) {
    const fs = require('fs').promises;
    const destPath = destination instanceof Path ? destination : new Path(destination);
    
    try {
      await fs.copyFile(this._path.toString(), destPath.toString());
    } catch (err) {
      throw new Error(`Could not copy file: ${err.message}`);
    }
  }

  /**
   * Move file to destination
   * @param {Path|string} destination - Destination path
   * @returns {Promise<void>}
   */
  async move(destination) {
    const fs = require('fs').promises;
    const destPath = destination instanceof Path ? destination : new Path(destination);
    
    try {
      await fs.rename(this._path.toString(), destPath.toString());
    } catch (err) {
      if (err.code === 'EXDEV') {
        // Cross-device link, try copy and delete
        await this.copy(destPath);
        await this.remove();
      } else {
        throw new Error(`Could not move file: ${err.message}`);
      }
    }
  }

  /**
   * Remove file
   * @returns {Promise<void>}
   */
  async remove() {
    const fs = require('fs').promises;
    try {
      await fs.unlink(this._path.toString());
    } catch (err) {
      throw new Error(`Could not remove file: ${err.message}`);
    }
  }
}

class Directory {
  /**
   * Create a new directory instance
   * @param {Path|string} path - Path to the directory
   */
  constructor(path) {
    this._path = path instanceof Path ? path : new Path(path);
  }

  /**
   * Check if directory exists
   * @returns {Promise<boolean>} True if directory exists
   */
  async exists() {
    return this._path.exists() && this._path.isDirectory();
  }

  /**
   * Create directory
   * @param {boolean} [recursive=false] - Create parent directories if they don't exist
   * @returns {Promise<void>}
   */
  async create(recursive = false) {
    const fs = require('fs').promises;
    try {
      await fs.mkdir(this._path.toString(), { recursive });
    } catch (err) {
      if (err.code !== 'EEXIST') {
        throw new Error(`Could not create directory: ${err.message}`);
      }
    }
  }

  /**
   * Remove directory
   * @param {boolean} [recursive=false] - Remove contents recursively
   * @returns {Promise<void>}
   */
  async remove(recursive = false) {
    const fs = require('fs').promises;
    const path = require('path');
    
    if (recursive) {
      try {
        const contents = await this.list(false);
        
        for (const entry of contents) {
          if (await entry.isDirectory()) {
            await new Directory(entry).remove(true);
          } else {
            await new File(entry).remove();
          }
        }
        
        await fs.rmdir(this._path.toString());
      } catch (err) {
        throw new Error(`Could not remove directory recursively: ${err.message}`);
      }
    } else {
      try {
        await fs.rmdir(this._path.toString());
      } catch (err) {
        throw new Error(`Could not remove directory: ${err.message}`);
      }
    }
  }

  /**
   * List directory contents
   * @param {boolean} [recursive=false] - List contents recursively
   * @returns {Promise<Path[]>} Array of paths
   */
  async list(recursive = false) {
    const fs = require('fs').promises;
    const path = require('path');
    
    try {
      const result = [];
      const entries = await fs.readdir(this._path.toString(), { withFileTypes: true });
      
      for (const entry of entries) {
        const fullPath = new Path(path.join(this._path.toString(), entry.name));
        result.push(fullPath);
        
        if (recursive && entry.isDirectory()) {
          const subdir = new Directory(fullPath);
          const subEntries = await subdir.list(true);
          result.push(...subEntries);
        }
      }
      
      return result;
    } catch (err) {
      throw new Error(`Could not list directory: ${err.message}`);
    }
  }
}

// Export the classes
module.exports = {
  Path,
  File,
  Directory
}; 