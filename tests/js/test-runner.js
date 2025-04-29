/**
 * CrossDev Toolkit - JavaScript Test Runner
 */

const jest = require('jest');

// Run the tests
jest.run([
  '--config', JSON.stringify({
    verbose: true,
    testEnvironment: 'node',
    testMatch: ['**/tests/js/**/*.test.js'],
    testPathIgnorePatterns: ['/node_modules/']
  })
]); 