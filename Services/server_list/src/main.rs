use std::convert::Infallible;
use std::sync::Arc;
use async_std::sync::Mutex;
use futures_util::future::join;

use hyper::service::{make_service_fn, service_fn};
use hyper::Server;

use tokio::task;
use tokio::time::{sleep, Duration};

mod context;
mod metrics;
mod manager;

use context::Context;

#[tokio::main]
pub async fn main() {
    let ctx = Arc::<Mutex::<Context>>::new(Mutex::new(Context::new()));

    let task_ctx = Arc::clone(&ctx);
    task::spawn(async move {
        loop {
            sleep(Duration::from_secs(10)).await;
            task_ctx.lock().await.clean();
        }
    });
    
    let service_ctx = ctx.clone();
    let make_service_svc = make_service_fn(move |_socket| { 
        let ctx = service_ctx.clone();
        async move {
            Ok::<_, Infallible>(service_fn(move |req| {
                let ctx = ctx.clone();
                return manager::handle(req, ctx);
            }))
        }
    });

    let metrics_ctx = ctx.clone();
    let make_metrics_svc = make_service_fn(move |_socket| { 
        let ctx = metrics_ctx.clone();
        async move {
            Ok::<_, Infallible>(service_fn(move |req| {
                let ctx = ctx.clone();
                return metrics::handle(req, ctx);
            }))
        }
    });

    let service_addr = ([0, 0, 0, 0], 80).into();
    let metrics_addr = ([0, 0, 0, 0], 9100).into();

    let service_server = Server::bind(&service_addr).serve(make_service_svc);
    let metrics_server = Server::bind(&metrics_addr).serve(make_metrics_svc);

    println!("Listening on http://{} and metrics http://{}", service_addr, metrics_addr);

    let _ret = join(service_server, metrics_server).await;
}