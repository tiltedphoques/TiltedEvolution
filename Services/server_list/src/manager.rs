use hyper::{Body, Request, Response, StatusCode, Method};
use std::sync::Arc;
use async_std::sync::Mutex;
use std::convert::Infallible;
use url::form_urlencoded;
use std::collections::HashMap;

use crate::context::Context;

fn bad_request() -> Result<Response<Body>, Infallible> {
    Ok(Response::builder().status(StatusCode::BAD_REQUEST).body(Body::empty()).unwrap_or_default())
}

async fn handle_announce(req: Request<Body>, ctx: Arc::<Mutex::<Context>>) -> Result<Response<Body>, Infallible> {

    let forwarded = req.headers().get("X-Forwarded-For")
        .and_then(|n| match n.to_str() {
            Ok(n) => Some(n),
            Err(_) => None
        })
        .map_or(String::from("127.0.0.1"), |n| String::from(n) );

    let host = forwarded.split(",").next().map_or(String::from("127.0.0.1"), |n| String::from(n));

    let b = hyper::body::to_bytes(req).await;
    let b = match b {
        Ok(bytes) => bytes,
        Err(_) => return bad_request()
    };

    let params = form_urlencoded::parse(b.as_ref())
        .into_owned()
        .collect::<HashMap<String, String>>();

    let mut name = match params.get("name") {
        Some(n) => String::from(n),
        _ => String::from("")
    };
    name.truncate(64);

    // Optional
    let mut desc = match params.get("desc") {
        Some(n) => String::from(n),
        _ => String::from("")
    };
    desc.truncate(256);

    // Optional
    let mut icon_url = match params.get("icon_url") {
        Some(n) => String::from(n),
        _ => String::from("")
    };
    icon_url.truncate(512);

    let port = params.get("port").and_then(|n| match n.parse::<u16>() {
        Ok(n) => Some(n),
        Err(_) => None,
    });

    let tick = params.get("tick").and_then(|n| match n.parse::<u16>() {
        Ok(n) => Some(n),
        Err(_) => None,
    });

    let player_count = params.get("player_count").and_then(|n| match n.parse::<u16>() {
        Ok(n) => Some(n),
        Err(_) => None,
    });

    let max_player_count = params.get("max_player_count").and_then(|n| match n.parse::<u16>() {
        Ok(n) => Some(n),
        Err(_) => None,
    });

    let port = match port {
        Some(port) => port,
        None => return bad_request()
    };

    let player_count = match player_count {
        Some(player_count) => player_count,
        None => return bad_request()
    };

    let max_player_count = match max_player_count {
        Some(max_player_count) => max_player_count,
        None => return bad_request()
    };

    let tick = match tick {
        Some(tick) => tick,
        None => return bad_request()
    };

    if port < 1 {
        return bad_request()
    }

    ctx.lock().await.update(
        host,
        name,
        desc,
        icon_url,
        port,
        tick,
        player_count,
        max_player_count);

    Ok(Response::builder()
    .status(StatusCode::OK)
    .body(Body::empty())
    .unwrap_or_default())
}

async fn handle_list(_req: Request<Body>, ctx: Arc::<Mutex::<Context>>) -> Result<Response<Body>, Infallible> {

    Ok(Response::builder()
    .status(StatusCode::OK)
    .header("Content-Type", "application/json")
    .body(Body::from(ctx.lock().await.server_list.clone()))
    .unwrap_or_default())
}

async fn handle_stat(_req: Request<Body>, ctx: Arc::<Mutex::<Context>>) -> Result<Response<Body>, Infallible> {

    Ok(Response::builder()
    .status(StatusCode::OK)
    .header("Content-Type", "application/json")
    .body(Body::from(ctx.lock().await.generate_stats())).unwrap_or_default())
}

pub async fn handle(req: Request<Body>, ctx: Arc::<Mutex::<Context>>) -> Result<Response<Body>, Infallible> {

    match (req.method(), req.uri().path()) {
        (&Method::POST, "/announce") => handle_announce(req, ctx).await,
        (&Method::GET, "/list") => handle_list(req, ctx).await,
        (&Method::GET, "/stat") => handle_stat(req, ctx).await,
        _ => Ok(Response::builder()
        .status(StatusCode::NOT_FOUND)
        .body(Body::empty())
        .unwrap_or_default())
    }
}