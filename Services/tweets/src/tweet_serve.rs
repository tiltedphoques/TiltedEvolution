use async_std::sync::Mutex;
use hyper::{Body, Request, Response};
use std::convert::Infallible;
use std::sync::Arc;

use crate::context::Context;

pub async fn handle(
    _req: Request<Body>,
    ctx: Arc<Mutex<Context>>,
) -> Result<Response<Body>, Infallible> {
    // fetch and serve the latest tweets collection from memory
    let result = &ctx.lock().await.latest_tweets;

    Ok(Response::new(Body::from(result.clone())))
}
