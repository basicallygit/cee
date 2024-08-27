# cee project manager

cee is a highly customizable project manager primarily for C/C++, however it can easily by adapted to other programming languages through the config file.

## Usage
```
Usage: cee <COMMAND>

Commands:
  new    Creates a new cee project
  init   Initializes a cee project in the current directory
  build  Build the current project
  run    Compile & Run the current project
  clean  Purge all build files
  help   Print this message or the help of the given subcommand(s)

Options:
  -h, --help     Print help
  -V, --version  Print version
```

## cee.toml configuration
|Key|Description|
|---|---|
|<b>[project]</b>||
|name|The name of the project|
|version|The version of the project|
|<b>[build]</b>||
|compiler|The compiler (or interpreter) to invoke on build|
|debug_flags|The flags to pass to {compiler} when building in debug mode|
|release_flags|The flags to pass to {compiler} when building in release mode|
|source_files|A list of source files to compile|
|bin_directory|The directory to place build files (leave empty for current directory)|

## Optional cee.toml configuration options
|Key|Description|
|---|---|
|<b>[build]<b>||
|command_format|The format of the build command (see #command_format options)|
|finish_on_build|A boolean representing whether or not to finish after building, even if the command was <kbd>run</kbd> (useful for interpreted languages)|

## command_format options
All of the required cee.toml are valid command_format options, with the exception of debug_flags and release_flags just being replaced by [flags]<br>
There is also an added [win_exe_extension] which is encouraged to be added where there would be a ".exe" extension, on non-windows systems this option is simply ignored, however on windows it will be replaced with the string ".exe"

The default command_format is:
```
[compiler] [source_files] [flags] -o [bin_directory]/[project_name][win_exe_extension]
```

## cee.toml examples:
<b>C project</b>
```
[project]
name = "my_c_project"
version = "0.1.0"

[build]
compiler = "cc"
debug_flags = "-Wall -Wpedantic"
release_flags = "-Wall -Wpedantic -O2"
source_files = "src/main.c"
bin_directory = "bin"
```

<b>Python project</b>
```
[project]
name = "my_python_project"
version = "0.1.0"

[build]
compiler = "python3"
debug_flags = ""
release_flags = ""
source_files = "main.py"
bin_directory = ""
command_format = "[compiler] [source_files]"
finish_on_build = true
```

