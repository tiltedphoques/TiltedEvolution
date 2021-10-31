use std::collections::HashMap;
use std::time::SystemTime;
use std::cmp;

pub struct Context {
    pub server_list: String,
}

impl Context {
    pub fn new() -> Context {
        Context {
            server_list: String::from("{\"servers\": []}"),
        }
    }
    
    pub fn update(&mut self) {
        
    }
}