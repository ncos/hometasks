/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* =======================================================================
     This file contains classes for parallel rendering of "image blocks".
 * ======================================================================= */

#pragma once

#include <nori/color.h>
#include <nori/vector.h>
#include <nori/block.h>
#include <tbb/mutex.h>

#define NORI_BLOCK_SIZE 32 /* Block size used for parallelization */

NORI_NAMESPACE_BEGIN

/**
 * \brief Weighted pixel storage for a rectangular subregion of an image
 *
 * This class implements storage for a rectangular subregion of a
 * larger image that is being rendered. For each pixel, it records color
 * values along with a weight that specifies the accumulated influence of
 * nearby samples on the pixel (according to the used reconstruction filter).
 *
 * When rendering with filters, the samples in a rectangular
 * region will generally also contribute to pixels just outside of 
 * this region. For that reason, this class also stores information about
 * a small border region around the rectangle, whose size depends on the
 * properties of the reconstruction filter.
 */
class ImageBlockPlain : public ImageBlock {
public:
    ImageBlockPlain(const Vector2i &size) : ImageBlock(size, nullptr) {}

    virtual void put(const Point2f &pos, const Color3f &value);
    virtual void put_special(const Point2f &pos, const Color3f &value);
    virtual void put(ImageBlock &b);
    virtual Bitmap *toBitmap() const;
};

NORI_NAMESPACE_END
