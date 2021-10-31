use hyper::client::HttpConnector;
use hyper::{Body, Client, Request, Response};
use hyper_tls::HttpsConnector;

pub struct Context {
    pub client: Client<HttpsConnector<HttpConnector>>,
    pub latest_tweets: String,
}

pub async fn body_to_string(req: Response<Body>) -> String {
    let body_bytes = hyper::body::to_bytes(req.into_body()).await.unwrap();
    return String::from_utf8(body_bytes.to_vec()).unwrap();
}

impl Context {
    pub fn new() -> Context {
        Context { 
            client: Client::builder().build::<_, hyper::Body>(HttpsConnector::new()),
            latest_tweets: String::from("[{}]") 
        }
    }

    pub async fn update(&mut self) {
        // kindly ask twitter to provide some tweets for us
        let req = Request::builder()
            .uri("https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=Skyrim_Together&count=2")
            .header("authorization", "Bearer AAAAAAAAAAAAAAAAAAAAAOtDUwEAAAAAbBRyDZVZ8vgvvDUa6RrxJcQNbTs%3DOXZXUbDqn0VvpSBHEnDt0TlOgrZ6sAr08jki8THWA2iwHcT4DV")
            .body(Body::empty())
            .unwrap();

        let maybe_response = self.client.request(req);

        let response_body = maybe_response.await.unwrap();
        self.latest_tweets = body_to_string(response_body).await;
    }
}
