use anyhow::{self, Result};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs::{read_to_string, File};
use std::io::Write;
use std::path::Path;
use std::process::exit;
use toml;

pub const CEE_CONFIG_FILE: &str = "cee.toml";
pub const DEFAULT_MAIN_FILE: &str = "main.c";
pub const DEFAULT_MAIN_FILE_CONTENTS: &str = "#include <stdio.h>

int main(void) {
    puts(\"Hello, World!\");
    return 0;
}";

const DEFAULT_COMMAND_FORMAT: &str =
    "[compiler] [source_files] [flags] -o [bin_directory]/[project_name][win_exe_extension]";
const REQUIRED_COMMAND_FORMAT_OPTIONS: [&str; 2] = ["[compiler]", "[source_files]"];

#[derive(Debug, Serialize, Deserialize)]
pub struct Config {
    pub project: Project,
    pub build: Build,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Project {
    pub name: String,
    pub version: String,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Build {
    pub compiler: String,
    pub debug_flags: String,
    pub release_flags: String,
    pub source_files: String,
    pub bin_directory: String,
    pub command_format: Option<String>,
    pub finish_on_build: Option<bool>,
}

impl Default for Config {
    #[rustfmt::skip]
    fn default() -> Self {
        Self {
            project: Project {
                name:    String::new(), // To be replaced with the project name
                version: "0.1.0".into(),
            },
            build: Build {
                compiler:        "cc".into(),
                debug_flags:     "-Wall -Wpedantic".into(),
                release_flags:   "%debug_flags% -O2".into(),
                source_files:    "src/main.c".into(),
                bin_directory:   "bin".into(),
                command_format:  None,
                finish_on_build: None,
            },
        }
    }
}

impl Config {
    #[rustfmt::skip]
    pub fn blank_config() -> Self {
        Self {
            project: Project {
                name:    String::new(), // To be replaced with the project name
                version: "0.1.0".into(),
            },
            build: Build {
                compiler:        "".into(),
                debug_flags:     "".into(),
                release_flags:   "".into(),
                source_files:    "".into(),
                bin_directory:   "".into(),
                command_format:  Some("".into()),
                finish_on_build: Some(false),
            },
        }
    }

    pub fn create_build_command(&self, is_release: bool) -> Result<String> {
        let command_format = if let Some(format) = &self.build.command_format {
            format.as_str()
        } else {
            DEFAULT_COMMAND_FORMAT
        };

        let mut build_command_string = String::from(command_format);

        // Validate that all the required options are present
        for option in REQUIRED_COMMAND_FORMAT_OPTIONS {
            if !command_format.contains(option) {
                anyhow::bail!(
                    "`{}`: Field 'command_format' is missing the '{}' field",
                    CEE_CONFIG_FILE,
                    option
                );
            }
        }

        let format_options_map = HashMap::from([
            ("[compiler]", &self.build.compiler),
            ("[source_files]", &self.build.source_files),
        ]);

        // Replace all option placeholders with their actual values
        for option in REQUIRED_COMMAND_FORMAT_OPTIONS {
            build_command_string = build_command_string.replace(option, format_options_map[option]);
        }

        let flags = if is_release {
            // Allow release flags to copy the flags already written in debug_flags with the %debug_flags% flag
            // (Don't allow %release_flags% in debug_flags to prevent passing the raw format flag to the compiler)
            if self.build.release_flags.contains("%debug_flags%") {
                &self
                    .build
                    .release_flags
                    .replace("%debug_flags%", &self.build.debug_flags)
            } else {
                &self.build.release_flags
            }
        } else {
            &self.build.debug_flags
        };

        let bin_dir = if self.build.bin_directory.trim().is_empty() {
            "."
        } else {
            &self.build.bin_directory
        };

        build_command_string = build_command_string.replace("[bin_directory]", bin_dir);
        build_command_string = build_command_string.replace("[flags]", flags);
        build_command_string = build_command_string.replace("[version]", &self.project.version);
        build_command_string = build_command_string.replace(
            "[win_exe_extension]",
            if cfg!(windows) { ".exe" } else { "" },
        );

        Ok(build_command_string.replace("[project_name]", &self.project.name))
    }
}

pub fn parse_config<P: AsRef<Path> + ToString>(path: P) -> std::io::Result<Config> {
    let config: Config = match toml::from_str(read_to_string(&path)?.as_str()) {
        Ok(c) => c,
        Err(e) => {
            eprintln!("Error in `{}`: {}", path.to_string(), e);
            exit(1);
        }
    };

    Ok(config)
}

pub fn write_config<P: AsRef<Path> + ToString>(path: P, config: Config) -> std::io::Result<()> {
    let toml_config_data = toml::to_string(&config).unwrap();
    File::create(path)?.write_all(toml_config_data.as_bytes())?;

    Ok(())
}
