#ifndef MATH_PDF_HPP
#define MATH_PDF_HPP

namespace Math {
    class Pdf {
    public:
        Pdf();
        Pdf(float pdf);
    
        operator float() const;

        bool isDelta() const;

        static Pdf delta();

    private:
        Pdf(float pdf, bool isDelta);
    
        float mPdf;
        bool mIsDelta;
    };
}
#endif