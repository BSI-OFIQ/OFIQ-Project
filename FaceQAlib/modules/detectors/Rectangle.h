#pragma once

namespace FACEQA_LIB::modules::detectors
{
    struct Rectangle
    {
        float centerX = 0;
        float centerY = 0;
        float height = 0;
        float width = 0;

        float Top() const { return centerY - (height / 2); }

        float Bottom() const { return centerY + (height / 2); }

        float Left() const { return centerX - (width / 2); }

        float Right() const { return centerX + (width / 2); }

        float Area() const { return height * width; }

        Rectangle Intersection(const Rectangle& other) const
        {
            Rectangle output;
            auto left = std::max(Left(), other.Left());
            auto right = std::min(Right(), other.Right());
            auto top = std::max(Top(), other.Top());
            auto bottom = std::min(Bottom(), other.Bottom());
            if (left < right && top < bottom)
            {
                output.centerX = (top + bottom) / 2;
                output.centerY = (left + right) / 2;
                output.height = bottom - top;
                output.width = right - left;
            }

            return output;
        }
    };
}