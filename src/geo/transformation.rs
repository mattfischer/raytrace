use crate::geo;

use geo::Matrix4;
use geo::Vec3;

#[derive(Copy, Clone)]
pub struct Transformation {
    pub matrix : Matrix4,
    pub inverse_matrix : Matrix4
}

impl Transformation {
    pub fn transform(&self, xform : Transformation) -> Transformation {
        return Transformation {matrix: self.matrix * xform.matrix, inverse_matrix: xform.inverse_matrix * self.inverse_matrix};
    }

    pub fn inverse_transform(&self, xform : Transformation) -> Transformation {
        return Transformation {matrix: xform.matrix * self.matrix, inverse_matrix: self.inverse_matrix * xform.inverse_matrix};
    }
    
    pub fn identity() -> Transformation {
        return Transformation {matrix: Matrix4::IDENTITY, inverse_matrix: Matrix4::IDENTITY};
    }

    pub fn translate(v : Vec3) -> Transformation {
        let matrix = Matrix4::new(
            [
                1.0, 0.0, 0.0, v.x,
                0.0, 1.0, 0.0, v.y,
                0.0, 0.0, 1.0, v.z,
                0.0, 0.0, 0.0, 1.0
            ]
        );

        let inverse_matrix = Matrix4::new(
            [
                1.0, 0.0, 0.0, -v.x,
                0.0, 1.0, 0.0, -v.y,
                0.0, 0.0, 1.0, -v.z,
                0.0, 0.0, 0.0, 1.0
            ]
        );

        return Transformation {matrix, inverse_matrix}
    }

    pub fn scale(v : Vec3) -> Transformation {
        let matrix = Matrix4::new(
            [
                v.x, 0.0, 0.0, 0.0,
                0.0, v.y, 0.0, 0.0,
                0.0, 0.0, v.z, 0.0,
                0.0, 0.0, 0.0, 1.0
            ]
        );
    
        let inverse_matrix = Matrix4::new(
            [
                1.0 / v.x, 0.0, 0.0, 0.0,
                0.0, 1.0 / v.y, 0.0, 0.0,
                0.0, 0.0, 1.0 / v.z, 0.0,
                0.0, 0.0, 0.0, 1.0
            ]
        );

        return Transformation {matrix, inverse_matrix}
    }

    pub fn uniform_scale(f : f32) -> Transformation {
        return Self::scale(Vec3::new(f, f, f));
    }

    pub fn rotate(v : Vec3) -> Transformation {
        let rx = v.x.to_radians();
        let ry = v.y.to_radians();
        let rz = v.z.to_radians();
        
        let matrix_x = Matrix4::new(
            [
                1.0, 0.0, 0.0, 0.0,
                0.0, rx.cos(), -rx.sin(), 0.0,
                0.0, rx.sin(), rx.cos(), 0.0,
                0.0, 0.0, 0.0, 1.0
            ]
        );

        let matrix_xi = Matrix4::new(
            [
                1.0, 0.0, 0.0, 0.0,
                0.0, rx.cos(), rx.sin(), 0.0,
                0.0, -rx.sin(), rx.cos(), 0.0,
                0.0, 0.0, 0.0, 1.0
            ]
        );
    
        let matrix_y = Matrix4::new(
            [
                ry.cos(), 0.0, -ry.sin(), 0.0,
                0.0, 1.0, 0.0, 0.0,
                ry.sin(), 0.0, ry.cos(), 0.0,
                0.0, 0.0, 0.0, 1.0
            ]
        );

        let matrix_yi = Matrix4::new(
            [
                ry.cos(), 0.0, ry.sin(), 0.0,
                0.0, 1.0, 0.0, 0.0,
                -ry.sin(), 0.0, ry.cos(), 0.0,
                0.0, 0.0, 0.0, 1.0
            ]
        );
    
        let matrix_z = Matrix4::new(
            [
                rz.cos(), rz.sin(), 0.0, 0.0,
                -rz.sin(), rz.cos(), 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0
            ]
        );

        let matrix_zi = Matrix4::new(
            [
                rz.cos(), -rz.sin(), 0.0, 0.0,
                rz.sin(), rz.cos(), 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0
            ]
        );

        let matrix = matrix_x * matrix_y * matrix_z;
        let inverse_matrix = matrix_zi * matrix_yi * matrix_xi;
        
        return Transformation {matrix, inverse_matrix}
    }
}