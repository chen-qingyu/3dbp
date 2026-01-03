use criterion::{Criterion, criterion_group, criterion_main};
use pack3d::algorithm::Algorithm;
use pack3d::tool::parse_input;
use std::fs;

fn benchmark_algorithm(c: &mut Criterion) {
    for entry in fs::read_dir("data/br_json").unwrap() {
        let path = entry.unwrap().path();

        if !path.to_str().unwrap().contains("001") {
            continue;
        }

        let filename = path.file_name().unwrap().to_str().unwrap().to_string();
        let path_str = path.to_str().unwrap().to_string();

        c.bench_function(&filename, |b| {
            b.iter(|| {
                let input = parse_input(&path_str);
                let output = Algorithm::new(input).run();
                std::hint::black_box(output.containers.len())
            });
        });
    }
}

criterion_group!(benches, benchmark_algorithm);
criterion_main!(benches);
