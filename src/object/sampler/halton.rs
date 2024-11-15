use crate::geo;
use crate::object;

use object::Sampler;

use rand::prelude::*;
use rand::thread_rng;

#[derive(Debug)]
pub struct Halton {
    index : u64,
    next_dimension : usize,

    width_exponent : usize,
    width_aligned : usize,
    height_exponent : usize,
    height_aligned : usize,
    sample_stride : usize,

    euclid_x : i32,
    euclid_y : i32,

    prime_indices : Vec<usize>,
    scrambled_digits_start : Vec<usize>,
    scrambled_digits : Vec<usize>
}

const PRIMES : [usize; 168] = [
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 
    53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109,
    113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179,
    181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241,
    251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313,
    317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389,
    397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461,
    463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547,
    557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617,
    619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691,
    701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773,
    787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859,
    863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947,
    953, 967, 971, 977, 983, 991, 997
];

fn euclid(a : i32, b : i32) -> (i32, i32)
{
    let mut r0 = a; let mut r1 = b;
    let mut s0 = 1; let mut s1 = 0;
    let mut t0 = 0; let mut t1 = 1;

    while r1 > 0 {
        let q = r0 / r1;
        let r2 = r0 - q * r1; r0 = r1; r1 = r2;
        let s2 = s0 - q * s1; s0 = s1; s1 = s2;
        let t2 = t0 - q * t1; t0 = t1; t1 = t2;
    }

    return (s0, t0);
}

impl Halton {  
    pub fn new(width : i32, height : i32) -> Halton {
        let mut rng = rand::thread_rng();
    
        let index = 0;
        let next_dimension = 0;

        let mut width_exponent = 0;
        let mut width_aligned = 1;
        while width_aligned < width {
            width_exponent += 1;
            width_aligned *= 2;
        }

        let mut height_exponent = 0;
        let mut height_aligned = 1;
        while height_aligned < height {
            height_exponent += 1;
            height_aligned *= 3;
        }

        let (euclid_x, euclid_y) = euclid(width_aligned, height_aligned);
        let sample_stride = (width_aligned * height_aligned) as usize;
        
        let mut prime_indices = vec![0; PRIMES.len()];
        let mut scrambled_digits = Vec::<usize>::new();
        let mut scrambled_digits_start = Vec::<usize>::new();
        for i in 0..PRIMES.len() {
            prime_indices[i] = i;
            let mut digits = Vec::<usize>::with_capacity(PRIMES[i]);
            digits.resize(PRIMES[i], 0);
            for j in 0..PRIMES[i] {
                digits[j] = j;
            }
    
            if i > 1 {
                digits.shuffle(&mut rng);
            }
            scrambled_digits_start.push(scrambled_digits.len());
            scrambled_digits.extend(digits);
        }
        prime_indices[2..].shuffle(&mut rng);

        return Halton {index, next_dimension, width_exponent, width_aligned: width_aligned as usize, height_exponent, height_aligned: height_aligned as usize, sample_stride, euclid_x, euclid_y, prime_indices, scrambled_digits_start, scrambled_digits};
    }
}

impl Sampler for Halton {
    fn start_sample_with_index(&mut self, index : usize) {
        self.index = index as u64;
        self.next_dimension = 0;
    }

    fn start_sample_with_xys(&mut self, x : usize, y : usize, sample : usize) {
        let mut xm = x as i32;
        let mut ym = y as i32;

        let mut xr = 0 as i32;
        for i in 0..self.width_exponent {
            xr = 2 * xr + xm % 2;
            xm /= 2;
        }

        let mut yr = 0 as i32;
        for i in 0..self.height_exponent {
            yr = 3 * yr + ym % 3;
            ym /= 3;
        }

        let mut idx = xr * self.euclid_y * (self.height_aligned as i32) + yr * self.euclid_x * (self.width_aligned as i32);
        if idx < 0 {
            idx = self.sample_stride as i32 - (-idx % self.sample_stride as i32);
        }

        self.index = idx as u64 + (sample * self.sample_stride) as u64;
        self.next_dimension = 0;
    }

    fn get_value(&mut self) -> f32 {
        let prime_index = self.prime_indices[self.next_dimension];
        let b = PRIMES[prime_index] as u64;

        let mut n = 0 as u64;
        let mut d = 1 as u64;

        let mut x = self.index;
        let mut i = 0;
        let scrambled_digits_start = self.scrambled_digits_start[prime_index];
        while x > 0 {
            if self.next_dimension == 0 && i < self.width_exponent || self.next_dimension == 1 && i < self.height_exponent {
                // ...
            } else {
                n = n * b + self.scrambled_digits[scrambled_digits_start + (x % b) as usize] as u64;
                d *= b;
            }

            x /= b;
            i += 1;
        }

        let nd = (self.scrambled_digits[scrambled_digits_start] as f32) / ((b - 1) as f32);
        let mut f = (n as f32 + nd) / (d as f32);
        if f == 1.0 {
            f = 0.0;
        }
        self.next_dimension += 1;

        if self.next_dimension == PRIMES.len() {
            self.next_dimension = 0;
        }

        return f;
    }
}