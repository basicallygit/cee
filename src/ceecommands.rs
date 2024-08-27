use anyhow::{self, Context, Result};
use std::env::{current_dir, set_current_dir};
use std::fs::{create_dir, create_dir_all, remove_dir_all, remove_file, File};
use std::io::Write;
use std::path::{Path, PathBuf};
use std::process::{Command, Stdio};
use std::time::Instant;

use crate::ceeconfig::{
    self, Config, CEE_CONFIG_FILE, DEFAULT_MAIN_FILE, DEFAULT_MAIN_FILE_CONTENTS,
};

pub fn cee_new(project_name: String, blank_project: bool) -> Result<()> {
    if Path::new(&project_name).exists() {
        anyhow::bail!(
            "Destination `{}` already exists.\n\n\
            Try using 'init' to initialize the directory",
            project_name
        );
    }

    create_dir(&project_name)
        .with_context(|| format!("Failed to create directory `{}`", project_name))?;

    // cd into the project directory
    set_current_dir(&project_name).unwrap();

    let mut config = if blank_project {
        Config::blank_config()
    } else {
        Config::default()
    };

    // Set the [project] 'name' field
    config.project.name = project_name.clone();

    // Write the default config file to cee.toml
    ceeconfig::write_config(CEE_CONFIG_FILE, config).unwrap();

    // If --blank was specified, we only care about creating the config file.
    if blank_project {
        println!("Created new cee project '{}'", project_name);
        return Ok(());
    }

    // Create the src directory
    create_dir("src").unwrap();

    let main_file: PathBuf = ["src", DEFAULT_MAIN_FILE].iter().collect();

    // Create the main file
    File::create(main_file)
        .unwrap()
        .write_all(DEFAULT_MAIN_FILE_CONTENTS.as_bytes())
        .unwrap();

    println!("Created new cee project '{}'", project_name);

    Ok(())
}

pub fn cee_init(blank_project: bool) -> Result<()> {
    // Check that cee.toml doesn't already exist
    if Path::new(CEE_CONFIG_FILE).exists() {
        anyhow::bail!(
            "`{}`: Path already exists in current directory",
            CEE_CONFIG_FILE
        );
    }

    let main_file: PathBuf = ["src", DEFAULT_MAIN_FILE].iter().collect();

    // Blank project doesnt care about if src/ exists
    if !blank_project && main_file.exists() {
        anyhow::bail!("`{}`: Path already exists!", main_file.display());
    }

    let mut config = if blank_project {
        Config::blank_config()
    } else {
        Config::default()
    };

    // Set the [project] 'name' field
    let project_name = current_dir()
        .unwrap()
        .file_name()
        .unwrap()
        .to_string_lossy()
        .to_string();
    config.project.name = project_name.clone();

    // Write the default config file to cee.toml
    ceeconfig::write_config(CEE_CONFIG_FILE, config)
        .with_context(|| format!("Failed to create `{}`", CEE_CONFIG_FILE))?;

    // If --blank was specified, we only care about creating the config file.
    if blank_project {
        println!("Created new cee project '{}'", project_name);
        return Ok(());
    }

    if !Path::new("src").exists() {
        // At this point we have already created cee.toml successfully
        // so no other write operations should fail in this directory
        create_dir("src").unwrap();
        File::create(&main_file)
            .unwrap()
            .write_all(DEFAULT_MAIN_FILE_CONTENTS.as_bytes())
            .unwrap();
    } else {
        let mut f = File::create(&main_file)
            .with_context(|| format!("Failed to create `{}`", main_file.display()))?;

        f.write_all(DEFAULT_MAIN_FILE_CONTENTS.as_bytes()).unwrap();
    }

    println!("Created new cee project '{}'", project_name);

    Ok(())
}

pub fn cee_build(is_release: bool, verbose: bool) -> Result<()> {
    let config = ceeconfig::parse_config(CEE_CONFIG_FILE)
        .with_context(|| format!("Failed to read `{}`", CEE_CONFIG_FILE))?;

    let bin_dir = Path::new(&config.build.bin_directory);

    if bin_dir.exists() {
        if bin_dir.is_file() {
            anyhow::bail!(
                "Bin path `{}` already exists and is not a directory",
                config.build.bin_directory
            );
        }
    } else {
        if verbose {
            println!("[INFO] Creating directory `{}`", bin_dir.display());
        }

        create_dir_all(bin_dir)
            .with_context(|| format!("Failed to create directory `{}`", bin_dir.display()))?;
    }

    let build_command_string = config.create_build_command(is_release)?;

    println!(
        "Building '{}' v{}{}",
        config.project.name,
        config.project.version,
        if is_release { " (release)" } else { "" }
    );

    if verbose {
        println!("[INFO] [EXEC] {}", build_command_string);
    }

    let build_command: Vec<&str> = build_command_string.split(' ').collect();

    let build_start_time = Instant::now();

    let status = Command::new(build_command[0])
        .stdin(Stdio::inherit())
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .args(build_command[1..].iter())
        .status()
        .with_context(|| format!("Failed to execute `{}`", build_command[0]))?;

    if !status.success() {
        anyhow::bail!("Failed to build '{}', exiting..", config.project.name);
    }

    if !config.build.finish_on_build.is_some_and(|f| f) {
        println!(
            "Finished building '{}'{} in {:.2?}",
            config.project.name,
            if is_release { " (release)" } else { "" },
            build_start_time.elapsed()
        );
    }

    Ok(())
}

pub fn cee_run(is_release: bool, verbose: bool) -> Result<()> {
    cee_build(is_release, verbose)?;

    let config = ceeconfig::parse_config(CEE_CONFIG_FILE)
        .with_context(|| format!("Failed to read `{}`", CEE_CONFIG_FILE))?;

    if config.build.finish_on_build.is_some_and(|f| f) {
        // If `finish_on_build` was set to true, everything has already been done in build
        // and there is no need to execute any binary
        return Ok(());
    }

    let bin_dir = if config.build.bin_directory.trim().is_empty() {
        "."
    } else {
        &config.build.bin_directory
    };

    let mut output_binary: PathBuf = [bin_dir, &config.project.name].iter().collect();

    if cfg!(windows) {
        output_binary = output_binary.with_extension("exe");
    }

    println!("Running `{}`", output_binary.display());

    if verbose {
        println!("[INFO] [EXEC] {}", output_binary.display());
    }

    Command::new(&output_binary)
        .stdin(Stdio::inherit())
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .status()
        .with_context(|| format!("Failed to execute `{}`", output_binary.display()))?;

    Ok(())
}

pub fn cee_clean(verbose: bool) -> Result<()> {
    let config = ceeconfig::parse_config(CEE_CONFIG_FILE)
        .with_context(|| format!("Failed to read `{}`", CEE_CONFIG_FILE))?;

    if config.build.bin_directory.trim().is_empty() || config.build.bin_directory == "." {
        // Only try to remove any built executable file, since we cant
        // just remove the entire current directory
        let mut output_binary: PathBuf = [&config.build.bin_directory, &config.project.name]
            .iter()
            .collect();

        if cfg!(windows) {
            output_binary = output_binary.with_extension("exe");
        }

        if Path::new(&output_binary).exists() {
            if verbose {
                println!("[INFO] Removing file `{}`", output_binary.display());
            }

            remove_file(&output_binary)
                .with_context(|| format!("Failed to remove `{}`", output_binary.display()))?;
        }
    } else {
        // Remove the entire bin directory
        if Path::new(&config.build.bin_directory).exists() {
            if verbose {
                println!("[INFO] Removing directory `{}`", config.build.bin_directory);
            }

            remove_dir_all(&config.build.bin_directory)
                .with_context(|| format!("Failed to remove `{}`", config.build.bin_directory))?;
        }
    }

    Ok(())
}
