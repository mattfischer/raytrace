#include "Math/Pdf.hpp"

namespace Math {
    Pdf::Pdf(float pdf, bool isDelta)
    : mPdf(pdf), mIsDelta(isDelta)
    {
    }

    Pdf::operator float() const
    {
        return mPdf;
    }

    bool Pdf::isDelta() const
    {
        return mIsDelta;
    }
}