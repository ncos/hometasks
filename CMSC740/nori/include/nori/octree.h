#pragma once

#include <nori/common.h>
#include <nori/mesh.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

NORI_NAMESPACE_BEGIN


class OctreeNd {
protected:
    static Mesh *m_mesh;
    BoundingBox3f bbox;
    std::vector<uint32_t> triangles;
    unsigned int depth;

    static unsigned int inner_cnt;
    static unsigned int leaf_cnt;
    static unsigned long long int leaf_triangle_cnt;

public:
    OctreeNd (Mesh *mesh); 
    OctreeNd (BoundingBox3f& box);
    OctreeNd (BoundingBox3f& box, std::vector<uint32_t> &mask, unsigned int depth_ = 0);
    virtual ~OctreeNd () {}

    virtual uint32_t intersect (Ray3f &ray, Intersection &its, bool shadowRay) = 0;

    BoundingBox3f *get_bbox () {return &this->bbox; }
};


class OctreeNdInner : public OctreeNd {
protected:
    OctreeNd* children[8];

public:
    OctreeNdInner (Mesh *mesh) : OctreeNd (mesh) {OctreeNd::inner_cnt++; this->instantiate_children (); }
    OctreeNdInner (BoundingBox3f& box) : OctreeNd (box) {OctreeNd::inner_cnt++; this->instantiate_children (); }
    OctreeNdInner (BoundingBox3f& box, std::vector<uint32_t> &mask, unsigned int depth_ = 0) : 
        OctreeNd (box, mask, depth_) {OctreeNd::inner_cnt++; this->instantiate_children (); }

    virtual ~OctreeNdInner () {for (int i = 0; i < 8; ++i) if (this->children[i] != NULL) delete this->children[i]; }
    virtual uint32_t intersect (Ray3f &ray, Intersection &its, bool shadowRay);

private:
    void instantiate_children ();
};


class OctreeNdLeaf : public OctreeNd {
public:
    OctreeNdLeaf (Mesh *mesh) : OctreeNd (mesh) {OctreeNd::leaf_cnt++; OctreeNd::leaf_triangle_cnt += this->triangles.size();}
    OctreeNdLeaf (BoundingBox3f& box) : OctreeNd (box) {OctreeNd::leaf_cnt++; OctreeNd::leaf_triangle_cnt += this->triangles.size();}
    OctreeNdLeaf (BoundingBox3f& box, std::vector<uint32_t> &mask, unsigned int depth_ = 0) : 
        OctreeNd (box, mask, depth_) {OctreeNd::leaf_cnt++; OctreeNd::leaf_triangle_cnt += this->triangles.size();}

    virtual uint32_t intersect (Ray3f &ray, Intersection &its, bool shadowRay);
};


class Octree : public OctreeNdInner {
public:
    Octree (Mesh *mesh) : OctreeNdInner (mesh) {
        std::cout << "\tOctree: " << OctreeNd::leaf_cnt << " leaf nodes" << std::endl
                  << "\t\t" << OctreeNd::inner_cnt << " inner nodes" << std::endl
                  << "\t\t" << OctreeNd::leaf_triangle_cnt / OctreeNd::leaf_cnt << " triangles per node" << std::endl
                  << "\t\t" << sizeof(OctreeNdInner) << " b - size of inner node" << std::endl
                  << "\t\t" << sizeof(OctreeNdLeaf) + sizeof(uint32_t) * OctreeNd::leaf_triangle_cnt / OctreeNd::leaf_cnt  
                  << " b - average size of leaf node" << std::endl 
                  << "sizeof bbox / triangles / depth: " << sizeof(BoundingBox3f) << " " << sizeof(std::vector<uint32_t>) 
                  << " " << sizeof(unsigned int) << std::endl;
    }
};


class OctreeNdFactory {
public:
    static OctreeNd *get (bool leaf, BoundingBox3f& box) {
        if (leaf) return new OctreeNdLeaf (box);
        return new OctreeNdInner (box);
    }

    static OctreeNd *get (bool leaf, BoundingBox3f& box, std::vector<uint32_t> &mask, unsigned int depth_ = 0) {
        if (leaf) return new OctreeNdLeaf (box, mask, depth_);
        return new OctreeNdInner (box, mask, depth_);
    }
};



NORI_NAMESPACE_END
