use log::{error, info};
use pack3d::algorithm::Algorithm;
use pack3d::tool::parse_input;
use std::env;
use std::fs;
use std::path::Path;

fn main() {
    colog::init();

    // 解析输入
    let args = env::args().collect::<Vec<String>>();
    if args.len() != 2 {
        error!("Usage: pack3d <input_json>");
        return;
    }
    let input_file = &args[1];

    // 算法处理
    let input_data = parse_input(input_file);
    let output_data = Algorithm::new(input_data).run();

    // 输出结果
    let filename = Path::new(input_file).file_name().unwrap().to_string_lossy();
    let output_path = Path::new("result").join(format!("result-{}", filename));
    fs::create_dir_all(output_path.parent().unwrap()).unwrap();
    let output_json = serde_json::to_string_pretty(&output_data).unwrap();
    fs::write(&output_path, output_json).unwrap();
    info!("Results written to \"{}\".", output_path.display());
}
