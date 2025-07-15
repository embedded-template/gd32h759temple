use calamine::{Reader, Xlsx, open_workbook};
use std::fs::File;

#[derive(Clone)]
pub struct MetaData {
    pub cmd: String,
    pub byte_start: String,
    pub byte_end: String,
    pub bit_start: String,
    pub bit_end: String,
    pub data_type: String,
    pub data_name: String,
    pub enum_name: String,
}

impl MetaData {
    pub fn new() -> Self {
        Self {
            cmd: String::new(),
            byte_start: String::new(),
            byte_end: String::new(),
            bit_start: String::new(),
            bit_end: String::new(),
            data_type: String::new(),
            data_name: String::new(),
            enum_name: String::new(),
        }
    }

    pub fn set_cmd(&mut self, cmd: &str) {
        self.cmd = cmd.to_string();
    }

    pub fn set_byte_start(&mut self, byte_start: &str) {
        self.byte_start = byte_start.to_string();
    }

    pub fn set_byte_end(&mut self, byte_end: &str) {
        self.byte_end = byte_end.to_string();
    }

    pub fn set_bit_start(&mut self, bit_start: &str) {
        self.bit_start = bit_start.to_string();
    }

    pub fn set_bit_end(&mut self, bit_end: &str) {
        self.bit_end = bit_end.to_string();
    }

    pub fn set_data_type(&mut self, data_type: &str) {
        self.data_type = data_type.to_string();
    }

    pub fn set_data_name(&mut self, data_name: &str) {
        self.data_name = data_name.to_string();
        // 将data_name的首字母变大写，并在最前面追加一个e，然后赋值给enum_name
        if let Some(first_char) = data_name.chars().next() {
            self.enum_name = format!("e{}{}", 
                first_char.to_uppercase(), 
                &data_name[first_char.len_utf8()..]);
        } else {
            self.enum_name.clear();
        }
    }

    pub fn set_enum_name(&mut self, enum_name: &str) {
        self.enum_name = enum_name.to_string();
    }
}

#[derive(Clone)]
pub struct ItemDatas {
    pub data: Vec<MetaData>,
}

impl ItemDatas {
    pub fn new() -> Self {
        Self { data: Vec::new() }
    }

    pub fn add_item(&mut self, data: MetaData) {
        self.data.push(data);
    }
}

// 实现可消耗的迭代器
impl IntoIterator for ItemDatas {
    type Item = MetaData;
    type IntoIter = std::vec::IntoIter<MetaData>;

    fn into_iter(self) -> Self::IntoIter {
        self.data.into_iter()
    }
}

// 实现借用的迭代器
impl<'a> IntoIterator for &'a ItemDatas {
    type Item = &'a MetaData;
    type IntoIter = std::slice::Iter<'a, MetaData>;

    fn into_iter(self) -> Self::IntoIter {
        self.data.iter()
    }
}

// 实现可变借用的迭代器
impl<'a> IntoIterator for &'a mut ItemDatas {
    type Item = &'a mut MetaData;
    type IntoIter = std::slice::IterMut<'a, MetaData>;

    fn into_iter(self) -> Self::IntoIter {
        self.data.iter_mut()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_enum_name_generation() {
        let mut meta = MetaData::new();
        
        // 测试普通英文小写
        meta.set_data_name("voltage");
        assert_eq!(meta.enum_name, "eVoltage");
        
        // 测试普通英文大写开头
        meta.set_data_name("Current");
        assert_eq!(meta.enum_name, "eCurrent");
        
        // 测试全大写
        meta.set_data_name("POWER");
        assert_eq!(meta.enum_name, "ePOWER");
        
        // 测试带下划线
        meta.set_data_name("max_voltage");
        assert_eq!(meta.enum_name, "eMax_voltage");
        
        // 测试数字开头
        meta.set_data_name("1st_channel");
        assert_eq!(meta.enum_name, "e1st_channel");
        
        // 测试中文
        meta.set_data_name("电压");
        assert_eq!(meta.enum_name, "e电压");
        
        // 测试中英文混合
        meta.set_data_name("温度sensor");
        assert_eq!(meta.enum_name, "e温度sensor");
        
        // 测试空字符串
        meta.set_data_name("");
        assert_eq!(meta.enum_name, "");
        
        // 测试单个字符
        meta.set_data_name("a");
        assert_eq!(meta.enum_name, "eA");
        
        // 测试特殊字符开头
        meta.set_data_name("_private");
        assert_eq!(meta.enum_name, "e_private");
    }

    #[test] 
    fn test_clone_and_iterator() {
        let mut item_datas = ItemDatas::new();
        
        let mut meta1 = MetaData::new();
        meta1.set_data_name("voltage");
        meta1.set_cmd("0x01");
        
        let mut meta2 = MetaData::new();
        meta2.set_data_name("current");
        meta2.set_cmd("0x02");
        
        item_datas.add_item(meta1);
        item_datas.add_item(meta2);
        
        // 测试克隆
        let cloned_datas = item_datas.clone();
        assert_eq!(cloned_datas.data.len(), 2);
        assert_eq!(cloned_datas.data[0].enum_name, "eVoltage");
        assert_eq!(cloned_datas.data[1].enum_name, "eCurrent");
        
        // 测试可消耗迭代器
        let mut count = 0;
        for meta in cloned_datas {
            count += 1;
            assert!(!meta.enum_name.is_empty());
        }
        assert_eq!(count, 2);
        
        // 测试借用迭代器
        count = 0;
        for meta in &item_datas {
            count += 1;
            assert!(!meta.enum_name.is_empty());
        }
        assert_eq!(count, 2);
    }
}

