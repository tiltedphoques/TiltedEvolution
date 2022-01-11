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
    pub name: String,
    pub desc: String,
    pub icon_url: String,
    pub ip: String,
    pub port: u16,
    pub tick: u16,
    pub player_count: u16,
    pub max_player_count: u16,
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

    pub fn update(&mut self, 
        endpoint: String, 
        name: String,
        desc: String,
        icon_url: String,
        port: u16,
        tick: u16,  
        player_count: u16, 
        max_player_count: u16) {

        let key = format!("{}:{}", endpoint, port);

        let server = Server{
            last_update: SystemTime::now(),
            name: name.clone(),
            desc: desc.clone(),
            icon_url: icon_url.clone(),
            ip: endpoint,
            port,
            tick,
            player_count,
            max_player_count,
        };

        self.servers.insert(key, server);

        self.generate_list();
    }

    fn generate_list(&mut self) {
        self.stats.player_count = 0;
        self.stats.server_count = self.servers.len() as u32;

        let mut list : String = String::from("{\"servers\": [");

        let mut first = true;

        for (_, s) in &self.servers {
            self.stats.player_count += s.player_count as u32;

            if s.ip.len() > 0 {
                if !first {
                    list.push_str(",");
                }
                list.push_str(&format!(
                    r#"{{"name": {}, "desc": {}, icon_url: {}, "ip": "{}", "port": {}, "tick": {}, "player_count": {}, "max_player_count": {}}}"#,
                    json::stringify(s.name.clone()), json::stringify(s.desc.clone()), json::stringify(s.icon_url.clone()),
                    s.ip, s.port, s.tick, s.player_count, s.max_player_count));
                first = false;
            }
        }

        self.stats.max_player_count = cmp::max(self.stats.max_player_count, self.stats.player_count);
        self.stats.max_server_count = cmp::max(self.stats.max_server_count, self.stats.server_count);

        list.push_str("]}");

        self.server_list = list;
    }

    pub fn generate_stats(&self) -> String {
        format!(
            r#"{{"current": {{"servers": {}, "players": {} }}, "max": {{"servers": {}, "players": {} }} }}"#,
            self.stats.server_count, self.stats.player_count, self.stats.max_server_count, self.stats.max_player_count)
    }
}