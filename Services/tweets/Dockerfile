FROM ekidd/rust-musl-builder as builder

ADD --chown=rust:rust . ./

RUN OPENSSL_LIB_DIR=/usr/local/musl/lib/ OPENSSL_INCLUDE_DIR=/usr/local/musl/include OPENSSL_STATIC=1 cargo build --release

FROM scratch

COPY --from=builder /home/rust/src/target/x86_64-unknown-linux-musl/release/tweet_provider /tweet_provider
COPY --from=builder /etc/ssl/certs/ca-certificates.crt /etc/ssl/certs/ca-certificates.crt
ENV SSL_CERT_DIR=/etc/ssl/certs/


ENTRYPOINT ["/tweet_provider"]
