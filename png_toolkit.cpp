#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <array>
#include "stb_image_write.h"
#include "png_toolkit.h"

png_toolkit::png_toolkit()
{
}

png_toolkit::~png_toolkit()
{
    stbi_image_free(m_imgData.pixels);
}

bool png_toolkit::load( const std::string &pictureName )
{
    m_imgData.pixels = stbi_load(pictureName.c_str(),
                                 &m_imgData.w, &m_imgData.h,
                                 &m_imgData.compPerPixel, 0);
    return m_imgData.pixels != nullptr;
}

bool png_toolkit::save( const std::string &pictureName )
{
    return stbi_write_png(pictureName.c_str(),
                          m_imgData.w, m_imgData.h,
                          m_imgData.compPerPixel,
                          m_imgData.pixels, 0) != 0;
}


image_data png_toolkit::getPixelData( void ) const
{
    return m_imgData;
}
