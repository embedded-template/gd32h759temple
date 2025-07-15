use clap::{Parser, Subcommand};

#[derive(Parser)]
#[command(name = "yfy-tool")]
#[command(about = "生成逆变器通讯代码与测试代码")]
#[command(version = "1.0")]
pub struct Cli {
    #[command(subcommand)]
    pub command: Commands,
}

#[derive(Subcommand)]
pub enum Commands {
    /// 生成模块通讯代码
    Code,
    /// 生成测试代码
    Test,
}

impl Cli {
    pub fn run(&self) {
        match &self.command {
            Commands::Code => {
                println!("生成模块通讯代码");
            }
            Commands::Test => {
                println!("生成测试代码");
            }
        }
    }
}

pub fn parse_args() -> Cli {
    Cli::parse()
}

fn gen_code(file_name: &str) {
    let yfy_data = YfyData::new(file_name);
}
