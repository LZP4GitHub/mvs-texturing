/*
 * Copyright (C) 2015, Nils Moehrle
 * TU Darmstadt - Graphics, Capture and Massively Parallel Computing
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-Clause license. See the LICENSE.txt file for details.
 */

#ifndef TEX_TEXTUREATLAS_HEADER
#define TEX_TEXTUREATLAS_HEADER


#include <vector>

#include <util/exception.h>
#include <math/vector.h>
#include <mve/mesh.h>
#include <mve/image.h>

#include "tri.h"
#include "texture_patch.h"
#include "rectangular_bin.h"

/**
  * Class representing a texture atlas.
  */
class TextureAtlas {
    public:
        typedef std::shared_ptr<TextureAtlas> Ptr;

        typedef std::vector<std::size_t> Faces;
        typedef std::vector<std::size_t> TexcoordIds;
        typedef std::vector<math::Vec2f> Texcoords;

    private:
        unsigned int const size;
        unsigned int const padding;
        bool finalized;

        Faces faces;
        Texcoords texcoords;
        TexcoordIds texcoord_ids;

        mve::ByteImage::Ptr image;
        mve::ByteImage::Ptr validity_mask;

        RectangularBin::Ptr bin;

        void apply_edge_padding(void);
        void merge_texcoords(void);

    public:
        TextureAtlas(unsigned int size);

        static TextureAtlas::Ptr create(unsigned int size);

        Faces const & get_faces(void) const;
        TexcoordIds const & get_texcoord_ids(void) const;
        Texcoords const & get_texcoords(void) const;
        mve::ByteImage::ConstPtr get_image(void) const;

        bool insert(TexturePatch::ConstPtr texture_patch, float mean, float max);

        void finalize(void);
};

inline TextureAtlas::Ptr
TextureAtlas::create(unsigned int size) {
    return Ptr(new TextureAtlas(size));
}

inline TextureAtlas::Faces const &
TextureAtlas::get_faces(void) const {
    return faces;
}

inline TextureAtlas::TexcoordIds const &
TextureAtlas::get_texcoord_ids(void) const {
    return texcoord_ids;
}

inline TextureAtlas::Texcoords const &
TextureAtlas::get_texcoords(void) const {
    return texcoords;
}

inline mve::ByteImage::ConstPtr
TextureAtlas::get_image(void) const {
    if (!finalized) {
        throw util::Exception("Texture atlas not finalized");
    }
    return image;
}

/**
  * Copies the src image into the dest image at the given position,
  * optionally adding a border.
  * @warning asserts that the given src image fits into the given dest image.
  */
template <typename T>
void copy_into(typename mve::Image<T>::ConstPtr src, int x, int y,
    typename mve::Image<T>::Ptr dest, int border = 0) {

    assert(x >= 0 && x + src->width() + 2 * border <= dest->width());
    assert(y >= 0 && y + src->height() + 2 * border <= dest->height());

    for(int j = 0; j < src->height() + 2 * border; j++) {
        for (int i = 0; i < src->width() + 2 * border; ++i) {
            int sx = i - border;
            int sy = j - border;

            if (sx < 0 || sx >= src->width() || sy < 0 || sy >= src->height())
                continue;

            for (int c = 0; c < src->channels(); ++c) {
                dest->at(x + i, y + j, c) = src->at(sx, sy, c);
            }
        }
    }
}
#endif /* TEX_TEXTUREATLAS_HEADER */
