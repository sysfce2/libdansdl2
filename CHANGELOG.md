# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog] and this project attempts to adhere to [Semantic Versioning].

Changes will be documented under Added, Changed, Deprecated, Removed, Fixed or Security headers.

## Known bugs:

## Unreleased
### Pending

## [V3.2.5]: 2026-09-12
### Added:
- Added epsilon to box collision methods.

## [V3.2.5]: 2025-08-03
### Added:
- Added from_angle method for vectors.
- Added angle between methods for points.

## [V3.2.4]: 2025-06-12
### Added:
- New "grow" method for the rect class.
- Missing point operators.

## [V3.2.3]: 2025-05-22
### Added:
- New "draw" class for quick and dirty geometry
- New aabb_collision supporting ray casting against aabb boxes.
- Add example with draw and aabb_collision example.

## [V3.2.2]: 2025-04-11
### Added:
- Audio controller has_free_channels()
- New audio exception to separate it from the std::runtime_error.

## [V3.2.1]: 2024-08-28
### Added:
- Added trim method to group representation.
### Changed:
- Group representations take part visibility into account when centering and drawing.

### Added:
## [V3.2.0]: 2024-08-23
### Added:
- Add filter class to substitute for the get_X_event of the input class.
- get_opengl_version function.
### Changed:
- Moves joystick, keyboard and mouse to its own files.
### Removed:
- Removes get_X_event methods of the input class. This breaks compatibility.

## [V3.1.10]: 2024-07-02
### Changed:
- Adds more information to exception when sound fails to load.

## [V3.1.9]: 2024-06-29
### Changed:
- Changes defaults for some audio methods.

## [V3.1.8]: 2024-05-03
### Added:
- Added simplified event querying system.
### Fixed:
- Fixes repeated internal details on keyboard input.

## [V3.1.7]: 2024-03-19
### Fixed:
- Alpha on representations now works as expected.

## [V3.1.6]: 2024-03-11
### Added:
- Color indexed surfaces can now be used as textures.
- Added "center_rotation_center" method, acting as an "auto center".

## [V3.1.5]: 08-02-2024
### Added:
- added get_keydown_control_text_filter to input.

## [V3.1.4]: 08-02-2024
### Added:
- Adds version type (debug or not) to lib version type.
### Changed:
- Changes build process so it defaults to shared.

## [V3.1.3]: 11-01-2023
### Added:
- Adds ability to add and remove screen borders.

## [V3.1.2]: 06-8-2022
### Added
- Adds ldt::get_lib_version to retrieve the version number and build type
- Builds up to three libraries (shared, static and debug) selectable from Cmake Options

## [V3.1.1]: 06-8-2022
### Added
- Adds "debug" mode to camera, can be turned on and off when building in debug mode.

## [v3.1.0]: Jul 2022
### Added
- Adds LIBDANSDL2_DEBUG mode which logs things.
- Compatibility with log library changes.

## [v3.0.0]: January 2020
### Added
- File reestructuration with major breaking changes.

## [v2.0.0]: July 2016
### Added
- Uses OpenGL for rendering.
- Transitions into english.
- Retooling interfaces.

## [v0.0.1]: Somewhen before 2006
### Added
- First version.


