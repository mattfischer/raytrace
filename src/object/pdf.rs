pub struct Pdf {
    pdf: f32,
    pub is_delta: bool
}

impl Pdf {
    pub fn new(pdf: f32, is_delta: bool) -> Pdf {
        return Pdf { pdf, is_delta };
    }

    pub fn as_f32(&self) -> f32 {
        return self.pdf;
    }
}
