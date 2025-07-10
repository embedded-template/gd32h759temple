mod cmd;

fn main() {
    let cli = cmd::parse_args();
    cli.run();
}
