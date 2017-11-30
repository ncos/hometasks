#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/camera.h>
#include <nori/block.h>
#include <nori/sampler.h>
#include <nori/warp.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>


extern nori::ImageBlock *globalLightBlock;

NORI_NAMESPACE_BEGIN


struct BDPT_Vertex
{
	Vector3f wi;
	Vector3f wo;
	float rr = 0.0f;
	Color3f throughput;
	Intersection its;
	float vc = 0.0f;
	float vcm = 0.0f;
    int	depth = 0;
    bool specular = false;
    bool from_specular = false;
};


class BDPTBetterIntegrator : public Integrator {
protected:
    bool naive;

public:
    BDPTBetterIntegrator (const PropertyList &props) {
        this->naive = false;
    }

    virtual void preprocess(const Scene *scene_) { 
        this->camera = scene_->getCamera();
        this->scene = scene_;
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        Ray3f ray_{ray}; // Eye ray
        Color3f li{0.0f};

        // Initialize emitters
        std::vector<const Mesh*> emitters;
        for (auto &m : scene->getMeshes())
            if (m->isEmitter()) emitters.push_back(m);

        DiscretePDF emitter_pdf;
        for (auto &e : emitters)
            emitter_pdf.append(1);    
        emitter_pdf.normalize();

        // Light ray
        int idx = emitter_pdf.sample(sampler->next1D());
        const Mesh* emitter = emitters[idx];

        // Sample a point on the emitter
        Point3f p;
        Normal3f n;
        emitter->samplePosition(sampler->next2D(), p, n);

        Intersection lits;
        if (!scene->rayIntersect(Ray3f{p + n * 0.01, -n}, lits) || (lits.p - p).dot(lits.p - p) > 0.01) {
            std::cout << "ERROR! points do not match...\n"
                      << lits.p << " " << p << "\n" << std::flush;
        }
		
        // Sample a ray
        BSDFQueryRecord bsdf_record{Normal3f{0, 0, 1}}; // wi does not matter for diffuse light
        emitter->getBSDF()->sample(bsdf_record, sampler->next2D());
        float pdf = emitter_pdf[idx];
        float light_emission_pdf = emitter->PDF();
	    float light_pdfa = emitter->getBSDF()->pdf(bsdf_record) * light_emission_pdf;
	    Ray3f   light_ray = Ray3f{p, lits.shFrame.toWorld(bsdf_record.wo)};
        float   cosAtLight = Frame::cosTheta(bsdf_record.wo);
        Color3f le = emitter->getEmitter()->eval(bsdf_record.wo, bsdf_record.wo);	
	 
    //-----------------------------------------------------------------------------------------------------
	// Trace light path from light source
	std::vector<BDPT_Vertex> light_path;
    bool from_specular = false;
	Ray3f wi = light_ray;
	float vc = MIS(cosAtLight / light_emission_pdf);
	float vcm = MIS(light_pdfa / light_emission_pdf);
	Color3f throughput = le * cosAtLight / (light_emission_pdf * pdf);
	float rr = 1.0f;
	while ((int)light_path.size() < 10)
	{
		BDPT_Vertex vert;
        if (!scene->rayIntersect(wi, vert.its))
            break;

		const float distSqr = (wi.o - vert.its.p).dot(wi.o - vert.its.p);
		const float cosIn = vert.its.shFrame.cosTheta(vert.its.shFrame.toLocal(-wi.d));
        
		if( light_path.size() > 0 )
			vcm *= MIS(distSqr);
		vcm /= MIS( cosIn );
		vc /= MIS( cosIn );

		rr = 1.0f;
		if (light_path.size() > 4)
			rr = 0.5f;

		vert.wi = vert.its.shFrame.toLocal(-wi.d);
		vert.throughput = throughput;
		vert.vcm = vcm;
		vert.vc = vc;
		vert.rr = rr;
		vert.depth = (unsigned)(light_path.size() + 1);
		light_path.push_back(vert);


		//-----------------------------------------------------------------------------------------------------
		// Path evaluation: light tracing
		Point3f cam_origin = ray.o;
        this->_ConnectCamera( vert , (unsigned)light_path.size(), cam_origin, sampler);

		// russian roulette
		if (sampler->next1D() > rr)
			break;

        BSDFQueryRecord bsdf_record{vert.wi};
        throughput *= vert.its.mesh->getBSDF()->sample(bsdf_record, sampler->next2D()) / rr;
        float bsdf_pdf = vert.its.mesh->getBSDF()->pdf(bsdf_record) * rr;
        vert.wo = bsdf_record.wo;
		const float cosOut = vert.its.shFrame.cosTheta(vert.wo);


        if (!vert.its.mesh->getBSDF()->isDiffuse()) {
            vert.specular = true;
            from_specular = true;
        }

        vert.from_specular = from_specular;
        from_specular = vert.specular;

		if (throughput.isZero())
			break;

        BSDFQueryRecord bsdf_record_rev{bsdf_record.wo, bsdf_record.wi, ESolidAngle};
		const float rev_bsdf_pdfw = vert.its.mesh->getBSDF()->pdf(bsdf_record_rev) * rr;
        if (!vert.specular) {
		    vc = MIS(cosOut/bsdf_pdf) * ( MIS(rev_bsdf_pdfw) * vc + vcm ) ;
		    vcm = MIS(1.0f/bsdf_pdf);
        } else {
            vc = vc;//MIS( cosOut / 1 ) * ( MIS( rev_bsdf_pdfw ) * vc + vcm );
            vcm = vcm;
        }
        
		wi = Ray3f{vert.its.p, vert.its.shFrame.toWorld(bsdf_record.wo)};
	}


	//-----------------------------------------------------------------------------------------------------
	// Trace light path from eye point
	const unsigned lps = (const unsigned)light_path.size();
    from_specular = false;
    wi = ray;
	throughput = 1.0f;
	int light_path_len = 0;
	vc = 0.0f;
	vcm = 1.0 / MIS(sampler->getSampleCount());
	rr = 1.0f;
	while (light_path_len <= (int)10)
	{
		BDPT_Vertex vert;
		vert.depth = light_path_len;
        if (!scene->rayIntersect(wi, vert.its))
            break;

		const float distSqr = (wi.o - vert.its.p).dot(wi.o - vert.its.p);
		const float cosIn = vert.its.shFrame.cosTheta(vert.its.shFrame.toLocal(-wi.d));
        
		vcm *= MIS( distSqr );
		vcm /= MIS( cosIn );
		vc /= MIS( cosIn );

		//-----------------------------------------------------------------------------------------------------
		// Path evaluation: it hits a light source
		if (vert.its.mesh == emitter)
		{

            BSDFQueryRecord bsdf_record{Normal3f{0, 0, 1}, vert.its.shFrame.toLocal(-wi.d), ESolidAngle};
			if( vert.depth > 0 && vert.depth <= 10 )
			{
				float emissionPdf = emitter->PDF();
	            float directPdfA = emitter->getBSDF()->pdf(bsdf_record) * emissionPdf;
                Color3f _li = emitter->getEmitter()->eval(vert.its, -wi.d) * throughput / pdf;

				li += _li / ( 1.0f + MIS( directPdfA ) * vcm + MIS( emissionPdf ) * vc );
			}
			else if( vert.depth == 0 )
			    li += emitter->getEmitter()->eval(bsdf_record.wo, bsdf_record.wo) / pdf;
		}

		rr = 1.0f;
		if (light_path_len > 4)
			rr = 0.5f;

		vert.wi = vert.its.shFrame.toLocal(-wi.d);
		vert.throughput = throughput;
		vert.vc = vc;
		vert.vcm = vcm;
		vert.rr = rr;
        vert.from_specular = from_specular;

		//-----------------------------------------------------------------------------------------------------
		// Path evaluation: connect light sample first
        //if (light_path_len < 1)
		li += this->_ConnectLight(vert, emitter, sampler) / pdf;

		//-----------------------------------------------------------------------------------------------------
		// Path evaluation: connect vertices
		for (unsigned j = 0; j < lps; ++j) {
            li += _ConnectVertices(light_path[j], vert);
        }

		++light_path_len;

		if (sampler->next1D() > rr)
			break;

        BSDFQueryRecord bsdf_record{vert.wi};
        throughput *= vert.its.mesh->getBSDF()->sample(bsdf_record, sampler->next2D()) / rr;
        float bsdf_pdf = vert.its.mesh->getBSDF()->pdf(bsdf_record) * rr;
        vert.wo = bsdf_record.wo;
		const float cosOut = vert.its.shFrame.cosTheta(vert.wo);

        if (!vert.its.mesh->getBSDF()->isDiffuse()) {
            vert.specular = true;
            from_specular = true;
        } else {
            from_specular = false;
        }


		if (throughput.isZero())
			break;

        BSDFQueryRecord bsdf_record_rev{bsdf_record.wo, bsdf_record.wi, ESolidAngle};
		const float rev_bsdf_pdfw = vert.its.mesh->getBSDF()->pdf(bsdf_record_rev) * rr;
        
        if (!vert.specular) {
            vc = MIS( cosOut / bsdf_pdf ) * ( MIS( rev_bsdf_pdfw ) * vc + vcm );
		    vcm = MIS( 1.0f / bsdf_pdf );
        } else {
            vc = vc;
            vcm = vcm;
        }
		wi = Ray3f{vert.its.p, vert.its.shFrame.toWorld(bsdf_record.wo)};
	}


        Point2f pixelSample = this->camera->pixelFromVisiblePoint(ray_, its.p);
        globalLightBlock->put_special(pixelSample, li / ((float)sampler->getSampleCount()) );


	    return li;
    }



// connnect vertices
Color3f _ConnectVertices( const BDPT_Vertex& p0 , const BDPT_Vertex& p1) const
{
    if( p0.depth + p1.depth >= 10) {
        return Color3f{0.0f};
    }

	const Vector3f delta = p0.its.p - p1.its.p;
	const float invDistcSqr = 1.0f / delta.dot(delta);
	const Vector3f n_delta = delta * sqrt(invDistcSqr);

    float cosAtP0 = std::max(p0.its.shFrame.cosTheta(p0.its.shFrame.toLocal(-n_delta)), 0.0f);
    float cosAtP1 = std::max(p1.its.shFrame.cosTheta(p1.its.shFrame.toLocal( n_delta)), 0.0f);
	Color3f g = cosAtP0 * cosAtP1 * invDistcSqr;

    BSDFQueryRecord p0_bsdf_record{p0.its.shFrame.toLocal(-n_delta), p0.wi, ESolidAngle};
    g *= p0.its.mesh->getBSDF()->eval(p0_bsdf_record);
    BSDFQueryRecord p1_bsdf_record{p1.wi, p1.its.shFrame.toLocal(n_delta), ESolidAngle};
    g *= p1.its.mesh->getBSDF()->eval(p1_bsdf_record);
	if (g.isZero())
		return Color3f{0.0f};


	const float p0_bsdf_pdfw = p0.its.mesh->getBSDF()->pdf(BSDFQueryRecord{p0.wi, p0.its.shFrame.toLocal(-n_delta), ESolidAngle}) * p0.rr;
	const float p0_bsdf_rev_pdfw = p0.its.mesh->getBSDF()->pdf(BSDFQueryRecord{p0.its.shFrame.toLocal(-n_delta), p0.wi, ESolidAngle}) * p0.rr;
	const float p1_bsdf_pdfw = p1.its.mesh->getBSDF()->pdf(BSDFQueryRecord{p1.wi, p1.its.shFrame.toLocal(n_delta), ESolidAngle}) * p1.rr;
	const float p1_bsdf_rev_pdfw = p1.its.mesh->getBSDF()->pdf(BSDFQueryRecord{p1.its.shFrame.toLocal(n_delta), p1.wi, ESolidAngle}) * p1.rr;

	const float p0_a = p1_bsdf_pdfw * cosAtP0 * invDistcSqr;
	const float p1_a = p0_bsdf_pdfw * cosAtP1 * invDistcSqr;

	const float mis_0 = MIS( p0_a ) * ( p0.vcm + p0.vc * MIS( p0_bsdf_rev_pdfw ) );
	const float mis_1 = MIS( p1_a ) * ( p1.vcm + p1.vc * MIS( p1_bsdf_rev_pdfw ) );

	const float weight = 1.0f / (mis_0 + 1.0f + mis_1);

	const Color3f li = p0.throughput * p1.throughput * g * weight;
	if( li.isZero() )
		return li;

    Intersection v_its;
    if (!(scene->rayIntersect(Ray3f{p1.its.p, n_delta}, v_its)) 
           || (v_its.mesh != p0.its.mesh)) return Color3f{0.0f};

	return li;
}

    Color3f _ConnectLight(const BDPT_Vertex& eye_vertex , const Mesh *light, Sampler *sampler) const
    {
	    if( eye_vertex.depth >= 10)
		    return Color3f{0.0f};

        if (eye_vertex.specular)
            return Color3f{0.0f};

        // Sample a point on the emitter
        Point3f p;
        Normal3f n;
        light->samplePosition(sampler->next2D(), p, n);

        Intersection lits;
        if (!scene->rayIntersect(Ray3f{p + n * 0.01, -n}, lits) || (lits.p - p).dot(lits.p - p) > 0.01) {
            std::cout << "ERROR! points do not match...\n"
                      << lits.p << " " << p << "\n" << std::flush;
        }

        Vector3f wi = (p - eye_vertex.its.p).normalized();
	    Ray3f   light_ray = Ray3f{eye_vertex.its.p, wi};
        float distSqr = (eye_vertex.its.p - p).dot(eye_vertex.its.p - p);
        Vector3f l2px_local = eye_vertex.its.shFrame.toLocal(wi);

        Intersection em_its;
        if (!(scene->rayIntersect(light_ray, em_its)) || (em_its.mesh != light)) 
            return Color3f{0.0f};
		
        BSDFQueryRecord bsdf_record{Normal3f{0, 0, 1}, em_its.shFrame.toLocal(-wi), ESolidAngle};
        float directPdfW = light->PDF();
	    float emissionPdfW = light->getBSDF()->pdf(bsdf_record) * directPdfW;
        float cosAtLight = std::max((-l2px_local).dot(eye_vertex.its.shFrame.toLocal(n)), 0.0f);
        Color3f li = light->getEmitter()->eval(bsdf_record.wo, bsdf_record.wo);	
        const float cosAtEyeVertex = std::max(eye_vertex.its.shFrame.cosTheta(l2px_local), 0.0f);

        if (distSqr < 0.05 || cosAtLight <= 0.0)
            return Color3f{0.0f};

        float geo = cosAtEyeVertex * cosAtLight / distSqr;
        //float geo = cosAtLight / distSqr;
        //float geo = cosAtEyeVertex / distSqr;

        BSDFQueryRecord bsdf_record_eye{eye_vertex.wi, l2px_local, ESolidAngle};
	    li *= eye_vertex.throughput * eye_vertex.its.mesh->getBSDF()->eval(bsdf_record_eye) * geo / directPdfW; 

	    if (li.isZero())
		    return Color3f{0.0f};

	    const float eye_bsdf_pdfw = eye_vertex.its.mesh->getBSDF()->pdf(BSDFQueryRecord{eye_vertex.wi,
        eye_vertex.its.shFrame.toLocal(wi), ESolidAngle}) * eye_vertex.rr;
	    const float eye_bsdf_rev_pdfw =
        eye_vertex.its.mesh->getBSDF()->pdf(BSDFQueryRecord{eye_vertex.its.shFrame.toLocal(wi), eye_vertex.wi, ESolidAngle})
        * eye_vertex.rr;

	    const float mis0 = MIS(eye_bsdf_pdfw / directPdfW);
	    const float mis1 = MIS(cosAtEyeVertex * emissionPdfW / ( cosAtLight * directPdfW ) ) * ( eye_vertex.vcm + eye_vertex.vc * MIS( eye_bsdf_rev_pdfw ) );
	    const float weight = 1.0f / ( mis0 + mis1 + 1.0f );
	    return li * weight;
    }


// connect camera point
void _ConnectCamera(const BDPT_Vertex& light_vertex, int len, Point3f &cam_origin, Sampler *sampler) const
{
	if( light_vertex.depth > 10 )
		return;

    Ray3f newRay{light_vertex.its.p, (cam_origin - light_vertex.its.p).normalized()};

    // Visibility test
    Intersection v_its;
    if (scene->rayIntersect(newRay, v_its))
        return;

    Point2f pixelSample = this->camera->pixelFromVisiblePoint(newRay, light_vertex.its.p);

	float camera_pdfA = 1;
	float camera_pdfW = 1;

	const Vector3f delta = light_vertex.its.p - cam_origin;
	const float invSqrLen = 1.0f / delta.dot(delta);
	const Vector3f n_delta = delta * sqrt(invSqrLen);

    if (delta.dot(delta) < 0.05)
        return;

    Vector3f local_wo = light_vertex.its.shFrame.toLocal(newRay.d);
	const float cosAtLightVertex = std::max(light_vertex.its.shFrame.cosTheta(local_wo), 0.0f);

    BSDFQueryRecord lv_bsdf_record{light_vertex.wi, local_wo, ESolidAngle};
	const Color3f bsdf_value = light_vertex.its.mesh->getBSDF()->eval(lv_bsdf_record);

	if(bsdf_value.isZero())
		return;

    float sample_per_pixel = (float)sampler->getSampleCount();
	const float gterm = cosAtLightVertex * invSqrLen;
	Color3f radiance = light_vertex.throughput * bsdf_value * gterm / sample_per_pixel; // (float)(sample_per_pixel * total_pixel * camera_pdfA);
  
    const float lightvert_pdfA = camera_pdfW * cosAtLightVertex * invSqrLen;
    

    const float bsdf_rev_pdfw = light_vertex.its.mesh->getBSDF()->pdf(BSDFQueryRecord{local_wo, light_vertex.wi, ESolidAngle}) * light_vertex.rr;

            
    //const float mis0 = ( light_vertex.vcm + light_vertex.vc * MIS( bsdf_rev_pdfw ) ) * MIS( lightvert_pdfA / total_pixel );
    const float mis0 = ( light_vertex.vcm + light_vertex.vc * MIS( bsdf_rev_pdfw ) ) * MIS( lightvert_pdfA);
	float weight = 1.0f / ( 1.0f + mis0 );
 
    radiance *= weight;
    globalLightBlock->put_special(pixelSample, radiance);
}



// mis factor
float MIS( float t ) const {
	return t * t;
}


    std::string toString() const {
        return "BDPTIntegrator[]";
    }

protected:
    const Scene *scene;
    const Camera *camera;
};

NORI_REGISTER_CLASS(BDPTBetterIntegrator, "bdpt_better");
NORI_NAMESPACE_END
