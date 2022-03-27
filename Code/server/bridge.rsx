#[cxx::bridge]
mod foo {
    extern "Rust" {
        fn say_hello_rs();
    }
}