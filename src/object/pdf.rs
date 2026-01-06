pub struct Pdf {
    pdf: f32,
    pub is_delta: bool
}

impl Pdf {
    pub fn new(pdf: f32) -> Pdf {
        return Pdf { pdf, is_delta: false };
    }

    pub fn delta() -> Pdf {
        return Pdf { pdf: 1.0, is_delta: true };
    }

    pub fn as_f32(&self) -> f32 {
        return self.pdf;
    }
}
