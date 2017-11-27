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

#include <nori/warp.h>
#include <nori/vector.h>
#include <nori/frame.h>


NORI_NAMESPACE_BEGIN


Point2f Warp::squareToUniformSquare(const Point2f &sample) {
    return sample;
}


float Warp::squareToUniformSquarePdf(const Point2f &sample) {
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}


Point2f Warp::squareToTent(const Point2f &sample) {
    return Point2f((sample[0] < 0.5f) ? 1.0f - std::sqrt(sample[0] * 2.0f) : 
                                        std::sqrt(sample[0] * 2.0f - 1.0f) - 1.0f,
                   (sample[1] < 0.5f) ? 1.0f - std::sqrt(sample[1] * 2.0f) : 
                                        std::sqrt(sample[1] * 2.0f - 1.0f) - 1.0f);
}


float Warp::squareToTentPdf(const Point2f &p) {
	float x = std::fabs(p[0]);
	float y = std::fabs(p[1]);
	return 1.0f - x - y + x * y;
}


Point2f Warp::squareToUniformDisk(const Point2f &sample) {
	float r = std::sqrt(sample[0]);
	float theta = 2.0f * M_PI * sample[1];
	return Point2f(r * std::cos(theta), r * std::sin(theta));
}


float Warp::squareToUniformDiskPdf(const Point2f &p) {
	return (p.dot(p) < 1.0f) ? INV_PI : 0.0f;
}


Vector3f Warp::squareToUniformSphere(const Point2f &sample) {
    float theta = 2.0f * M_PI * sample[0];
    float phi = std::acos(2.0f * sample[1] - 1.0f);
    return Vector3f(std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta), 2.0f * sample[1] - 1.0f);
}


float Warp::squareToUniformSpherePdf(const Vector3f &v) {
    return (std::abs(v.dot(v) - 1.0f) < 1e-5) ? INV_FOURPI : 0.0f;
}


Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) {
    float theta = 2.0f * M_PI * sample[0];
    float phi = std::acos(sample[1]);
    return Vector3f(std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta), sample[1]);
}


float Warp::squareToUniformHemispherePdf(const Vector3f &v) {
    return v[2] > 0.0f ? INV_TWOPI : 0.0f;
}


Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) {
    float theta = 2.0f * M_PI * sample[0];
    float phi = std::acos(2.0f * sample[1] - 1.0f) / 2.0f;
    return Vector3f(std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta), std::cos(phi));
}


float Warp::squareToCosineHemispherePdf(const Vector3f &v) {
    return v[2] > 0.0f ? v[2] * INV_PI : 0.0f;
}


Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    float theta = 2.0f * M_PI * sample[0];
    float tan_2phi = std::max(0.0f, -alpha * alpha * std::log(1 - sample[1]));
    float cos_phi = 1.0f / std::sqrt(1.0f + tan_2phi);
    float sin_phi = std::sqrt(1.0f - cos_phi * cos_phi);
    return Vector3f(sin_phi * std::cos(theta), sin_phi * std::sin(theta), cos_phi);
}


float Warp::squareToBeckmannPdf(const Vector3f &m, float alpha) {
    float cos_th = Frame::cosTheta(m);
    float tan_th2 = Frame::sinTheta2(m) / (cos_th * cos_th);

    if (std::isinf(tan_th2))
        return 0;

    if (m[2] > 0) {
        float alpha2 = alpha * alpha;
        return std::exp(-tan_th2 / alpha2) / (M_PI * alpha2 * std::pow(cos_th, 3));
    }
    
    return 0;  
}


Point2f Warp::squareToUniformTriangle(const Point2f &sample) {
    float a = std::sqrt(1.0f - sample.x());
    return Point2f(1 - a, a * sample.y());
}

NORI_NAMESPACE_END
