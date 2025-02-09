#ifndef __prog_Image_hpp__
#define __prog_Image_hpp__

#include "Color.hpp"
#include <vector>


namespace prog
{
    class Image
    {
    private:
        int width_;
        int height_;
        std::vector<std::vector<Color>> colors_;    // vetor 2D em que cada elemento é uma cor (representando, assim, uma imagem em forma de matriz).
    public:
        Image(int w, int h, const Color &fill = {255, 255, 255});
        ~Image();

        int width() const;
        int height() const;

        Color &at(int x, int y);
        const Color &at(int x, int y) const;
    };
}


#endif
