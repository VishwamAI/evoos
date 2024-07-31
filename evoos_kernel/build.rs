use std::process::Command;

fn main() {
    Command::new("cargo")
        .args(&["bootimage", "--target", "x86_64-evoos"])
        .status()
        .expect("Failed to build boot image");
}
