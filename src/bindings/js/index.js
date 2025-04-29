/**
 * CrossDev Toolkit - JavaScript Bindings
 * Main entry point for the JavaScript interface
 */

const fs = require('./filesystem');

// Export all modules
module.exports = {
  // Filesystem module
  fs,
  
  // Re-export for convenience
  Path: fs.Path,
  File: fs.File,
  Directory: fs.Directory
}; 