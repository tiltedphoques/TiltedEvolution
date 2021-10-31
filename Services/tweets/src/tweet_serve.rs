use hyper::{Body, Request, Response};
use std::sync::Arc;
use async_std::sync::Mutex;
use std::convert::Infallible;

use crate::context::{Context};

pub async fn handle(_req: Request<Body>, ctx: Arc::<Mutex::<Context>>) -> Result<Response<Body>, Infallible> {
    let result = "Hello there!";
    
    Ok(Response::new(Body::from(result)))
}