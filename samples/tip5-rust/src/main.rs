/**
 *
 * Copyright (c) 2025 Maxim [maxirmx] Samsonov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is a part of tip5xx library
 *
 */

use clap::{Parser, ValueEnum};
use std::error::Error;
use twenty_first::{tip5::Tip5, prelude::{Digest, BFieldElement}};

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

    /// Input digests in format (n1,n2,n3,n4,n5) for each digest
    #[arg(required = true, help = "Input digests in format (n1,n2,n3,n4,n5).\nFor pair mode: provide exactly 2 digests\nFor varlen mode: provide 2 or more digests\nEach number can be in formats:\n- Hexadecimal: 0x01020304 (must use 0x prefix)\n- Decimal: 16909060 (numbers starting with 0 like 0123 are treated as decimal)")]
    inputs: Vec<String>,
}

fn parse_number(input: &str) -> Result<BFieldElement, Box<dyn Error>> {
    let trimmed = input.trim();
    let value = if trimmed.starts_with("0x") || trimmed.starts_with("0X") {
        // Handle hex format
        let hex_str = &trimmed[2..];
        u64::from_str_radix(hex_str, 16)?
    } else {
        // Handle decimal
        trimmed.parse::<u64>()?
    };
    Ok(BFieldElement::new(value))
}

fn parse_digest(input: &str) -> Result<Digest, Box<dyn Error>> {
    // Remove outer parentheses and split by comma
    let content = input.trim()
        .strip_prefix('(')
        .ok_or("Missing opening parenthesis")?
        .strip_suffix(')')
        .ok_or("Missing closing parenthesis")?;

    let numbers: Vec<&str> = content.split(',').collect();
    if numbers.len() != 5 {
        return Err("Each digest must contain exactly 5 numbers".into());
    }

    let elements: Result<Vec<BFieldElement>, _> = numbers
        .iter()
        .map(|n| parse_number(n))
        .collect();

    let elements = elements?;
    Ok(Digest::new([
        elements[0],
        elements[1],
        elements[2],
        elements[3],
        elements[4],
    ]))
}

fn main() -> Result<(), Box<dyn Error>> {
    let args = Args::parse();

    match args.mode {
        Mode::Pair => {
            if args.inputs.len() != 2 {
                return Err("pair mode requires exactly 2 digests".into());
            }

            let digest1 = parse_digest(&args.inputs[0])?;
            let digest2 = parse_digest(&args.inputs[1])?;

            println!("Hash pair mode Digest{}, Digest{}", args.inputs[0], args.inputs[1]);
            let result = Tip5::hash_pair(digest1, digest2);
            print!("Result: ");
            println!("Digest({})", result.to_string());
        }
        Mode::Varlen => {
            if args.inputs.len() < 2 {
                return Err("varlen mode requires at least 2 digests".into());
            }

            let mut digests = Vec::new();
            for input in &args.inputs {
                let digest = parse_digest(input)?;
                digests.extend_from_slice(&digest.values());
            }

            print!("Hash varlen mode Digest");
            for (i, input) in args.inputs.iter().enumerate() {
                if i > 0 {
                    print!(", ");
                }
                print!("{}", input);
            }
            println!("");

            let result = Tip5::hash_varlen(&digests);
            print!("Result: ");
            println!("Digest({})", result.to_string());
        }
    }

    Ok(())
}
