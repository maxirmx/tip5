use clap::{Parser, ValueEnum};
use std::error::Error;
use twenty_first::{math::tip5::Tip5, prelude::{Digest, BFieldElement}};

#[derive(Debug, Copy, Clone, PartialEq, Eq, ValueEnum)]
enum Mode {
    Pair,
    Varlen,
}

#[derive(Parser)]
#[command(author, version, about = "TIP5 Hash Calculator")]
struct Args {
    /// Hash mode: 'pair' or 'varlen'
    #[arg(short, long, value_enum, default_value_t = Mode::Pair)]
    mode: Mode,

    /// Input numbers (hex with 0x prefix, decimal, or octal with 0 prefix)
    #[arg(required = true, help = "Input numbers.\nFor pair mode: provide exactly 2 numbers\nFor varlen mode: provide 2 or more numbers\nSupported formats:\n- Hexadecimal: 0x01020304 (must use 0x prefix)\n- Decimal: 16909060\n- Octal: 0100402404 (must use 0 prefix)")]
    inputs: Vec<String>,
}

fn parse_number(input: &str) -> Result<BFieldElement, Box<dyn Error>> {
    let value = if input.starts_with("0x") || input.starts_with("0X") {
        // Handle hex format
        let hex_str = &input[2..];
        if hex_str.len() % 2 != 0 {
            return Err("Hex string length must be even (full bytes)".into());
        }
        u64::from_str_radix(hex_str, 16)?
    } else if input.starts_with('0') {
        // Handle octal
        let oct_str = &input[1..];
        u64::from_str_radix(oct_str, 8)?
    } else {
        // Handle decimal
        input.parse::<u64>()?
    };
    Ok(BFieldElement::new(value))
}

fn print_hash(hash: &Digest) {
    println!("{:?}", hash);
}

fn main() -> Result<(), Box<dyn Error>> {
    let args = Args::parse();

    match args.mode {
        Mode::Pair => {
            if args.inputs.len() != 2 {
                return Err("pair mode requires exactly 2 inputs".into());
            }

            let input1 = parse_number(&args.inputs[0])?;
            let input2 = parse_number(&args.inputs[1])?;

            println!("Hash pair mode [{}, {}]:", args.inputs[0], args.inputs[1]);
            let result = Tip5::hash_pair(
                Digest::new([input1, BFieldElement::new(0), BFieldElement::new(0), BFieldElement::new(0), BFieldElement::new(0)]),
                Digest::new([input2, BFieldElement::new(0), BFieldElement::new(0), BFieldElement::new(0), BFieldElement::new(0)])
            );
            print!("Result: ");
            print_hash(&result);
        }
        Mode::Varlen => {
            if args.inputs.len() < 2 {
                return Err("varlen mode requires at least 2 inputs".into());
            }

            let mut inputs = Vec::new();
            for input in &args.inputs {
                inputs.push(parse_number(input)?);
            }

            print!("Hash varlen mode [");
            for (i, input) in args.inputs.iter().enumerate() {
                if i > 0 {
                    print!(", ");
                }
                print!("{}", input);
            }
            println!("]:");

            let result = Tip5::hash_varlen(&inputs);
            print!("Result: ");
            print_hash(&result);
        }
    }

    Ok(())
}
