use log::{error, info};
use pack3d::algorithm::Algorithm;
use pack3d::tool::parse_input;
use std::env;
use std::fs;
use std::path::Path;

fn main() {
    // 初始化日志，默认显示 info 级别
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    // 解析命令行参数
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        let program_name = Path::new(&args[0])
            .file_name()
            .and_then(|n| n.to_str())
            .unwrap_or("pack3d");
        error!("Usage: {} <input_json>", program_name);
        std::process::exit(1);
    }

    let input_file = &args[1];

    // 算法处理
    let input_data = parse_input(input_file);
    let output_data = Algorithm::new(input_data).run();

    // 输出结果
    let input_path = Path::new(input_file);
    let filename = input_path
        .file_name()
        .and_then(|n| n.to_str())
        .unwrap_or("result.json");
    let output_path = Path::new("result").join(format!("result-{}", filename));

    // 创建输出目录
    if let Some(parent) = output_path.parent() {
        fs::create_dir_all(parent).unwrap_or_else(|e| {
            error!("Failed to create output directory: {}", e);
            std::process::exit(1);
        });
    }

    // 写入输出文件
    let output_json = serde_json::to_string_pretty(&output_data).unwrap_or_else(|e| {
        error!("Failed to serialize output: {}", e);
        std::process::exit(1);
    });

    fs::write(&output_path, output_json).unwrap_or_else(|e| {
        error!("Failed to write output file: {}", e);
        std::process::exit(1);
    });

    info!("Results written to \"{}\".", output_path.display());
}
