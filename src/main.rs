use anyhow::Result;
use clap::{Parser, Subcommand};

mod ceecommands;
mod ceeconfig;

#[derive(Debug, Parser)]
#[command(name = "cee", version, about, long_about = None)]
struct Cli {
    #[command(subcommand)]
    command: Command,
}

#[derive(Debug, Subcommand)]
enum Command {
    /// Creates a new cee project
    #[command(arg_required_else_help = true)]
    New {
        /// The name of the project to create
        project_name: String,

        /// Creates a language-agnostic blank cee project
        #[arg(long)]
        blank: bool,
    },

    /// Initializes a cee project in the current directory
    Init {
        /// Creates a language-agnostic blank cee project
        #[arg(long)]
        blank: bool,
    },

    /// Build the current project
    Build {
        /// Builds the project in release mode
        #[arg(long, short)]
        release: bool,

        /// Show verbose output of the build process
        #[arg(long)]
        verbose: bool,
    },

    /// Compile & Run the current project
    Run {
        /// Runs the project in release mode
        #[arg(long, short)]
        release: bool,

        /// Show verbose output of the build process
        #[arg(long)]
        verbose: bool,
    },

    /// Purge all build files
    Clean {
        /// Show verbose output of the clean process
        #[arg(long)]
        verbose: bool,
    },
}

fn main() -> Result<()> {
    let args = Cli::parse();

    match args.command {
        #[rustfmt::skip]
        Command::New { project_name, blank } => ceecommands::cee_new(project_name, blank)?,

        Command::Init { blank } => ceecommands::cee_init(blank)?,

        Command::Build { release, verbose } => ceecommands::cee_build(release, verbose)?,

        Command::Run { release, verbose } => ceecommands::cee_run(release, verbose)?,

        Command::Clean { verbose } => ceecommands::cee_clean(verbose)?,
    };

    Ok(())
}
