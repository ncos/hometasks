#include <nori/octree.h>


NORI_NAMESPACE_BEGIN


Mesh *OctreeNd::m_mesh = NULL;
unsigned int OctreeNd::inner_cnt = 0;
unsigned int OctreeNd::leaf_cnt = 0;
unsigned long long int OctreeNd::leaf_triangle_cnt = 0;


OctreeNd::OctreeNd (Mesh *mesh) {
    OctreeNd::m_mesh = mesh; 
    this->bbox = mesh->getBoundingBox();
    this->depth = 0;
    
    for (uint32_t i = 0; i < OctreeNd::m_mesh->getTriangleCount(); ++i) {
        this->triangles.push_back(i);
    }
}


OctreeNd::OctreeNd (BoundingBox3f& box) {
    this->bbox = box;
    this->depth = 0;
    for (uint32_t i = 0; i < OctreeNd::m_mesh->getTriangleCount(); ++i) {
        const BoundingBox3f box_ = OctreeNd::m_mesh->getBoundingBox(i);
        if (this->bbox.overlaps(box_)) {
            this->triangles.push_back(i);
        }   
    }
}


OctreeNd::OctreeNd (BoundingBox3f& box, std::vector<uint32_t> &mask, unsigned int depth_) {
    this->bbox = box;
    this->depth = depth_;
    for (uint32_t i = 0; i < mask.size(); ++i) {
        const BoundingBox3f box_ = OctreeNd::m_mesh->getBoundingBox(mask[i]);
        if (this->bbox.overlaps(box_)) {
            this->triangles.push_back(mask[i]);
        }
    }
}


void OctreeNdInner::instantiate_children () {
    if (this->triangles.size() == 0) {
        for (int i = 0; i < 8; ++i) this->children[i] = NULL;
        return;
    }

    Point3f min = this->bbox.min;
    Point3f max = this->bbox.max;
    Point3f center = this->bbox.getCenter();
    bool leaf = this->triangles.size() < 80;
    if (std::ceil(std::log(OctreeNd::m_mesh->getTriangleCount() / 10) / std::log(8.0f)) < (this->depth + 1))
        leaf = true;

    BoundingBox3f boxes[8];
    boxes[0] = BoundingBox3f(min, center);
    boxes[1] = BoundingBox3f(Point3f(center.x(), min.y(), min.z()), Point3f(max.x(), center.y(), center.z()));
    boxes[2] = BoundingBox3f(Point3f(center.x(), center.y(), min.z()), Point3f(max.x(), max.y(), center.z()));
    boxes[3] = BoundingBox3f(Point3f(min.x(), center.y(), min.z()), Point3f(center.x(), max.y(), center.z()));
    boxes[4] = BoundingBox3f(Point3f(min.x(), min.y(), center.z()), Point3f(center.x(), center.y(), max.z()));
    boxes[5] = BoundingBox3f(Point3f(center.x(), min.y(), center.z()), Point3f(max.x(), center.y(), max.z()));
    boxes[6] = BoundingBox3f(Point3f(center.x(), center.y(), center.z()), Point3f(max.x(), max.y(), max.z()));
    boxes[7] = BoundingBox3f(Point3f(min.x(), center.y(), center.z()), Point3f(center.x(), max.y(), max.z()));

    
    tbb::parallel_for(tbb::blocked_range<int>(0, 8), [&](const tbb::blocked_range<int>& r) {
        for (int i = r.begin(); i < r.end(); ++i) {
            this->children[i] = OctreeNdFactory::get(leaf, boxes[i], this->triangles, this->depth + 1);
        }
    });
    

    //for (unsigned int i = 0; i < 8; ++i)
    //    this->children[i] = OctreeNdFactory::get(leaf, boxes[i], this->triangles, this->depth + 1);
    //this->triangles.clear(); // Save us some memory
}


uint32_t OctreeNdInner::intersect (Ray3f &ray, Intersection &its, bool shadowRay) {
    uint32_t idx = (uint32_t) -1;
    if (this->triangles.size() == 0) return idx;

    /* 
    for (int i = 0; i < 8; ++i) {
        if (!this->children[i]->get_bbox()->rayIntersect(ray)) continue;
        uint32_t idx_ = this->children[i]->intersect(ray, its, shadowRay);
        if (idx_ < (uint32_t) -1) {idx = idx_; }
    }
    return idx;
    */

    std::vector<std::pair<int, float>> distances;
    for (int i = 0; i < 8; ++i) {
        float nearT, farT;
        if (this->children[i]->get_bbox()->rayIntersect(ray, nearT, farT))
            distances.push_back(std::make_pair(i, nearT));
    }

    std::sort(distances.begin(), distances.end(), 
          [](std::pair<int, float> const &a, std::pair<int, float> const &b) 
          {return a.second < b.second; });

    for (auto &pair : distances) {
        uint32_t idx_ = this->children[pair.first]->intersect(ray, its, shadowRay);
        if (idx_ < (uint32_t) -1) return idx_;
    }

    return idx;
}


uint32_t OctreeNdLeaf::intersect (Ray3f &ray, Intersection &its, bool shadowRay) {
    bool foundIntrsection = false;
    uint32_t idx = (uint32_t) -1;

    for (uint32_t i = 0; i < this->triangles.size(); ++i) {
        float u, v, t;
        uint32_t idx_ = this->triangles[i];
        if (OctreeNd::m_mesh->rayIntersect(idx_, ray, u, v, t)) {
            ray.maxt = its.t = t;
            its.uv = Point2f(u, v);
            its.mesh = OctreeNd::m_mesh;
            foundIntrsection = true;
            idx = idx_;
            if (shadowRay) return idx;
        }
    }
    if (foundIntrsection) return idx;
    return (uint32_t) -1;
}


NORI_NAMESPACE_END
