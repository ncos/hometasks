__kernel void My_kernel(__global float * ps)
{
    size_t i = 3*get_global_id(0);

    float4 pos = (float4)(ps[i], ps[i+1], ps[i+2], 1);


	float dx = pos.x - 0;
	float dy = pos.y - 0;

	float4 V = (float4)(pos.x + dy, pos.y - dx, 0, 1);

    //pos = pos + V*0.009;

    ps[i] = pos.x;
    ps[i+1] = pos.y;
    ps[i+2] = pos.z;
}