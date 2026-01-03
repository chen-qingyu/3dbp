#[cfg(test)]
mod test_output {
    use jsonschema::JSONSchema;
    use pack3d::algorithm::Algorithm;
    use pack3d::constraint::Constraint;
    use pack3d::tool::parse_input;
    use std::collections::HashSet;
    use std::fs;
    use std::path::Path;

    #[test]
    fn test_output() {
        // 创建输出目录
        fs::create_dir_all("result").unwrap();

        // 遍历所有测试文件
        let test_dir = Path::new("data/tests");
        if !test_dir.exists() {
            return;
        }

        for entry in fs::read_dir(test_dir).unwrap() {
            let entry = entry.unwrap();
            let input_path = entry.path();

            if input_path.extension().and_then(|s| s.to_str()) != Some("json") {
                continue;
            }

            println!("Testing: {}", input_path.display());

            // 运行算法并输出结果
            let input = parse_input(input_path.to_str().unwrap());
            let output = Algorithm::new(input.clone()).run();

            // 1. 确保输入箱子与输出箱子数量相等
            let mut out_box_cnt = output.unpacked_boxes.len();
            for container in &output.containers {
                out_box_cnt += container.boxes.len();
            }
            assert_eq!(input.boxes.len(), out_box_cnt);

            // 2. 确保输入箱子与输出箱子实体相等
            let input_box_ids: HashSet<_> = input.boxes.iter().map(|b| &b.id).collect();
            let mut output_box_ids: HashSet<_> =
                output.unpacked_boxes.iter().map(|b| &b.id).collect();
            for container in &output.containers {
                for box_item in &container.boxes {
                    output_box_ids.insert(&box_item.id);
                }
            }
            assert_eq!(input_box_ids, output_box_ids);

            // 3. 确保输出的每个已装载箱子都符合约束
            for container in &output.containers {
                for (i, box_item) in container.boxes.iter().enumerate() {
                    let mut boxes = container.boxes.clone();
                    boxes.remove(i);
                    let constraint = Constraint::new(container, &boxes);
                    assert!(
                        constraint.check_constraints(box_item),
                        "Box {} violates constraints",
                        box_item.id
                    );
                }
            }

            // 4. 校验输出数据JSON格式
            let filename = input_path.file_name().unwrap().to_str().unwrap();
            let output_file = format!("result/result-{}", filename);
            let output_json = serde_json::to_string_pretty(&output).unwrap();
            fs::write(&output_file, output_json).unwrap();

            let result: serde_json::Value =
                serde_json::from_str(&fs::read_to_string(&output_file).unwrap()).unwrap();
            let schema: serde_json::Value =
                serde_json::from_str(&fs::read_to_string("tests/output_schema.json").unwrap())
                    .unwrap();

            let compiled = JSONSchema::compile(&schema).unwrap();
            let validation_result = compiled.validate(&result);
            if let Err(errors) = validation_result {
                let error_messages: Vec<String> = errors.map(|e| e.to_string()).collect();
                panic!(
                    "Output schema validation failed: {}",
                    error_messages.join(", ")
                );
            }
        }
    }
}
