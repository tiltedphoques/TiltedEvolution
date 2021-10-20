use hyper::{Body, Request, Response};
use std::sync::Arc;
use async_std::sync::Mutex;
use std::convert::Infallible;

use crate::context::{Context};

pub async fn handle(_req: Request<Body>, ctx: Arc::<Mutex::<Context>>) -> Result<Response<Body>, Infallible> {

    let stats = ctx.lock().await.stats;

    let result = format!("# HELP st_player_count Online player count
# TYPE st_player_count gauge
st_player_count {}
# HELP st_server_count Server running count
# TYPE st_server_count gauge
st_server_count {}", stats.player_count, stats.server_count);
    
    Ok(Response::new(Body::from(result)))
}