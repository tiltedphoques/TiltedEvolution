use std::collections::HashMap;
use std::time::SystemTime;
use std::cmp;

#[derive(Debug, Copy, Clone)]
pub struct Statistics{
    pub server_count: u32,
    pub max_server_count: u32,
    pub player_count: u32,
    pub max_player_count: u32 
}

struct Server {
    pub last_update: SystemTime,
    pub player_count: u32,
    pub name: String,
    pub ip: String,
    pub port: u16
}

pub struct Context {
    pub stats: Statistics,
    pub server_list: String,
    servers: HashMap::<String, Server>
}

impl Context {
    pub fn new() -> Context {
        Context {
            stats: Statistics{ 
                server_count: 0, 
                max_server_count: 0, 
                player_count: 0, 
                max_player_count: 0
            },
            server_list: String::from("{\"servers\": []}"),
            servers: HashMap::<String, Server>::new()
        }
    }

    pub fn clean(&mut self){
        self.servers.retain(|_, value| {
            match value.last_update.elapsed() {
                Ok(n) => n.as_secs() < 120,
                Err(_) => false,
            }
        });

        self.generate_list();
    }

    pub fn update(&mut self, endpoint: String, name: String, port: u16, player_count: u32) {
        let key = format!("{}:{}", endpoint, port);

        let server = Server{
            last_update: SystemTime::now(),
            player_count,
            name: name.clone(),
            ip: endpoint,
            port
        };

        self.servers.insert(key, server);

        self.generate_list();
    }

    fn generate_list(&mut self) {
        self.stats.player_count = 0;
        self.stats.server_count = self.servers.len() as u32;

        let mut list : String = String::from("{\"servers\": [");

        let mut first = true;

        for (_, value) in &self.servers {
            self.stats.player_count += value.player_count;

            if value.name.len() > 0 {
                if !first {
                    list.push_str(",");
                }
                list.push_str(&format!("{{\"name\": {}, \"ip\": \"{}\", \"port\": {}, \"player_count\": {} }}", json::stringify(value.name.clone()), value.ip, value.port, value.player_count));

                first = false;
            }
        }

        self.stats.max_player_count = cmp::max(self.stats.max_player_count, self.stats.player_count);
        self.stats.max_server_count = cmp::max(self.stats.max_server_count, self.stats.server_count);

        list.push_str("]}");

        self.server_list = list;
    }

    pub fn generate_stats(&self) -> String {
        format!("{{\"current\": {{\"servers\": {}, \"players\": {} }}, \"max\": {{\"servers\": {}, \"players\": {} }} }}", 
            self.stats.server_count, self.stats.player_count, self.stats.max_server_count, self.stats.max_player_count)
    }
}