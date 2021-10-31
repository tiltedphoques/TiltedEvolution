use async_std::sync::Mutex;
use std::convert::Infallible;
use std::sync::Arc;

use hyper::service::{make_service_fn, service_fn};
use hyper::Server;

use tokio::task;
use tokio::time::{sleep, Duration};

mod context;
mod tweet_serve;

use context::Context;

#[tokio::main]
pub async fn main() {
    let ctx = Arc::<Mutex<Context>>::new(Mutex::new(Context::new()));

    let task_ctx = Arc::clone(&ctx);
    //task_ctx.lock().await.MakeRequest();

    task::spawn(async move {
        loop {
            println!("Refreshing tweets!");
            task_ctx.lock().await.update().await;
            // every half hour we will refresh the tweets list
            sleep(Duration::from_secs(60 * 30)).await;
        }
    });

    let service_ctx = ctx.clone();
    let make_service_svc = make_service_fn(move |_socket| {
        let ctx = service_ctx.clone();
        async move {
            Ok::<_, Infallible>(service_fn(move |req| {
                let ctx = ctx.clone();
                return tweet_serve::handle(req, ctx);
            }))
        }
    });

    let service_addr = ([0, 0, 0, 0], 9280).into();
    let server = Server::bind(&service_addr).serve(make_service_svc);

    println!("Serving tweets on http://{}", service_addr);

    let _ret = server.await;
}
