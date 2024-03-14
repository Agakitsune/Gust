# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Entity component removal
- Template Entity creation
- Template Entity removal
- Entity creation with template
- Entity batch creation with template
- Entity relationship creation
- Entity relationship removal

## [1.1.0] - 2024-03-14

### Added

- C++17 bindings

### Fixed

- Type creation did not contain the first component
- Iterator crashed when no table satisfied the query

## [1.0.0] - 2024-03-14

### Added

- SANITIZE Cmake options, allowing for AddressSanitizer, LeakSanitizer, and UndefinedBehaviorSanitizer
- Entity creation
- Entity removal
- Entity batch creation
- Entity batch removal
- Component registration
- Entity component addition
- System registration
- System execution
