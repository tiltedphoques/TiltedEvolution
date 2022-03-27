#[cxx::bridge]
mod foo {
    extern "Rust" {
        fn say_hello_rs();
    }
}

pub fn say_hello_rs() {
    println!("Hello from rust!")
}