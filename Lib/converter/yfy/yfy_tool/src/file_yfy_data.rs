use log::info;
use std::fs::File;
use std::io::Write;

pub struct YfyData {
    pub file_c: File,
    pub file_h: File,
}

impl YfyData {
    pub fn new(file_name: &str) -> Self {
        let file_c = File::create(format!("{}.c", file_name)).unwrap();
        let file_h = File::create(format!("{}.h", file_name)).unwrap();
        info!("创建文件: {}.c", file_name);
        info!("创建文件: {}.h", file_name);
        Self { file_c, file_h }
    }

    pub fn write_file_c(mut self, content: &str) {
        self.file_c.write_all(content.as_bytes()).unwrap();
    }

    pub fn write_file_h(mut self, content: &str) {
        self.file_h.write_all(content.as_bytes()).unwrap();
    }
}
