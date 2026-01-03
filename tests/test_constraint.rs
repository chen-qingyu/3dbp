#[cfg(test)]
mod test_constraint {
    use pack3d::constraint::Constraint;
    use pack3d::entities::{Box, BoxType, Container, ContainerType};
    use rstest::*;
    use std::rc::Rc;

    #[fixture]
    fn test_data() -> (Container, Rc<BoxType>, Vec<Box>, Box, Box, Box) {
        let container = Container {
            container_type: ContainerType {
                id: "ct1".to_string(),
                lx: 10,
                ly: 10,
                lz: 10,
                payload: Some(100.0),
                quantity: Some(1),
            },
            ..Default::default()
        };

        let box_type = Rc::new(BoxType {
            id: "t1".to_string(),
            lx: 5,
            ly: 5,
            lz: 5,
            orients: vec![],
        });

        let origin = make_box("origin", box_type.clone(), 10.0, 0, 0, 0);
        let boxes = vec![origin];

        let box_x = make_box("bx", box_type.clone(), 10.0, 5, 0, 0);
        let box_y = make_box("by", box_type.clone(), 10.0, 0, 5, 0);
        let box_z = make_box("bz", box_type.clone(), 10.0, 0, 0, 5);

        (container, box_type, boxes, box_x, box_y, box_z)
    }

    fn make_box(id: &str, box_type: Rc<BoxType>, weight: f64, x: i32, y: i32, z: i32) -> Box {
        Box {
            id: id.to_string(),
            box_type: Some(box_type.clone()),
            type_id: box_type.id.clone(),
            weight: Some(weight),
            x: Some(x),
            y: Some(y),
            z: Some(z),
            orient: None,
            lx: box_type.lx,
            ly: box_type.ly,
            lz: box_type.lz,
        }
    }

    #[rstest]
    fn test_check_bound(test_data: (Container, Rc<BoxType>, Vec<Box>, Box, Box, Box)) {
        let (container, box_type, boxes, box_x, box_y, box_z) = test_data;
        let box_bad = make_box("bad", Rc::clone(&box_type), 10.0, 0, 0, 6); // Exceeds container height
        let constraint = Constraint::new(&container, &boxes);

        assert!(constraint.check_bound(&box_x));
        assert!(constraint.check_bound(&box_y));
        assert!(constraint.check_bound(&box_z));
        assert!(!constraint.check_bound(&box_bad));
    }

    #[rstest]
    fn test_check_overlap(test_data: (Container, Rc<BoxType>, Vec<Box>, Box, Box, Box)) {
        let (container, box_type, boxes, box_x, box_y, box_z) = test_data;
        let box_bad = make_box("bad", Rc::clone(&box_type), 10.0, 0, 0, 4); // Overlaps with origin box
        let constraint = Constraint::new(&container, &boxes);

        assert!(constraint.check_overlap(&box_x));
        assert!(constraint.check_overlap(&box_y));
        assert!(constraint.check_overlap(&box_z));
        assert!(!constraint.check_overlap(&box_bad));
    }

    #[rstest]
    fn test_check_support(test_data: (Container, Rc<BoxType>, Vec<Box>, Box, Box, Box)) {
        let (container, box_type, boxes, box_x, box_y, box_z) = test_data;
        let box_bad = make_box("bad", Rc::clone(&box_type), 10.0, 5, 0, 1); // Not properly supported
        let constraint = Constraint::new(&container, &boxes);

        assert!(constraint.check_support(&box_x));
        assert!(constraint.check_support(&box_y));
        assert!(constraint.check_support(&box_z));
        assert!(!constraint.check_support(&box_bad));
    }

    #[rstest]
    fn test_check_weight(test_data: (Container, Rc<BoxType>, Vec<Box>, Box, Box, Box)) {
        let (container, box_type, boxes, box_x, box_y, box_z) = test_data;
        let box_bad = make_box("bad", Rc::clone(&box_type), 100.0, 5, 0, 0); // Exceeds weight limit
        let constraint = Constraint::new(&container, &boxes);

        assert!(constraint.check_weight(&box_x));
        assert!(constraint.check_weight(&box_y));
        assert!(constraint.check_weight(&box_z));
        assert!(!constraint.check_weight(&box_bad));
    }
}
