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

#include <nori/block_plain.h>
#include <nori/bitmap.h>
#include <nori/rfilter.h>
#include <nori/bbox.h>
#include <tbb/tbb.h>


NORI_NAMESPACE_BEGIN


void ImageBlockPlain::put(const Point2f &_pos, const Color3f &value) {
    if (!value.isValid()) {
        /* If this happens, go fix your code instead of removing this warning ;) */
        cerr << "Integrator: computed an invalid radiance value: " << value.toString() << endl;
        return;
    }

    /* Convert to pixel coordinates within the image block */
    Point2f pos(
        _pos.x() - 0.5f - (m_offset.x() - m_borderSize),
        _pos.y() - 0.5f - (m_offset.y() - m_borderSize)
    );

    if (pos.y() < 0 || pos.x() < 0 || pos.y() >= this->m_size.y() || pos.x() >= this->m_size.x())
        return;

    this->lock();
    coeffRef(pos.y(), pos.x()) += Color4f(value);
    this->unlock();
}


void ImageBlockPlain::put_special(const Point2f &_pos, const Color3f &value) {
    if (!value.isValid()) {
        /* If this happens, go fix your code instead of removing this warning ;) */
        cerr << "Integrator: computed an invalid radiance value: " << value.toString() << endl;
        return;
    }

    /* Convert to pixel coordinates within the image block */
    Point2f pos(
        _pos.x() - 0.5f - (m_offset.x() - m_borderSize),
        _pos.y() - 0.5f - (m_offset.y() - m_borderSize)
    );

    if (pos.y() < 0 || pos.x() < 0 || pos.y() >= this->m_size.y() || pos.x() >= this->m_size.x())
        return;

    Color4f new_value = Color4f(value);

    this->lock();
    coeffRef(pos.y(), pos.x())[3] = 0;
    coeffRef(pos.y(), pos.x()) += new_value;
    this->unlock();
}


void ImageBlockPlain::put(ImageBlock &b) {
    Vector2i offset = b.getOffset() - m_offset +
        Vector2i::Constant(m_borderSize - b.getBorderSize());
    Vector2i size   = b.getSize()   + Vector2i(2*b.getBorderSize());

    tbb::mutex::scoped_lock lock(m_mutex);

    block(offset.y(), offset.x(), size.y(), size.x()) 
        += b.topLeftCorner(size.y(), size.x());
}

Bitmap *ImageBlockPlain::toBitmap() const {
    Bitmap *result = new Bitmap(m_size);
    for (int y=0; y<m_size.y(); ++y)
        for (int x=0; x<m_size.x(); ++x) {
            //result->coeffRef(y, x) = coeff(y + m_borderSize, x + m_borderSize).divideByFilterWeight();
            result->coeffRef(y, x) = coeff(y + m_borderSize, x + m_borderSize).head<3>();
        }
    return result;
}


NORI_NAMESPACE_END
