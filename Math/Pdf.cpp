#include "Math/Pdf.hpp"

namespace Math {
    Pdf::Pdf()
    : mPdf(0.0f), mIsDelta(false)
    {
    }

    Pdf::Pdf(float pdf, bool isDelta)
    : mPdf(pdf), mIsDelta(isDelta)
    {
    }

    Pdf::Pdf(float pdf)
    : mPdf(pdf), mIsDelta(false)
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

    Pdf Pdf::delta()
    {
        return Pdf(1.0f, true);
    }
}