#ifndef MATH_PDF_HPP
#define MATH_PDF_HPP

namespace Math {
    class Pdf {
    public:
        Pdf(float pdf = 0.0f, bool isDelta = false);
    
        operator float() const;

        bool isDelta() const;

    private:    
        float mPdf;
        bool mIsDelta;
    };
}
#endif