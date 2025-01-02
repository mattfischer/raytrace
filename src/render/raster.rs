pub struct Raster<T> {
    width: usize,
    height: usize,
    elements: Vec<T>,
}

impl<T: Default + Copy> Raster<T> {
    pub fn new(width: usize, height: usize) -> Raster<T> {
        let mut elements = Vec::new();
        elements.resize(width * height, T::default());
        return Raster {
            width,
            height,
            elements,
        };
    }

    pub fn set(&mut self, x: usize, y: usize, value: T) {
        if x < self.width && y < self.height {
            self.elements[y * self.width + x] = value;
        }
    }

    pub fn get(&self, x: usize, y: usize) -> T {
        if x < self.width && y < self.height {
            return self.elements[y * self.width + x];
        } else {
            return T::default();
        }
    }

    pub fn get_mut(&mut self, x: usize, y: usize) -> &mut T {
        return &mut self.elements[y * self.width + x];
    }
}
