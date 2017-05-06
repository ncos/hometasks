__kernel void oclp_kernel(__global float * ps, __global float * vl, __global float * r, __global float * m, __global float * q, float dt)
{
    size_t i = 3*get_global_id(0);

    float4 pos = (float4)(ps[i], ps[i+1], ps[i+2], 1);
    float4 vel = (float4)(vl[i], vl[i+1], vl[i+2], 1);

    // If we collide on the next step, invert the velocity:
    float4 new_pos = pos + vel * dt;
    if (fabs(new_pos.x) >= 75)
        vel.x = -vel.x;
    if (fabs(new_pos.y) >= 75)
        vel.y = -vel.y;
    if (fabs(new_pos.z) >= 75)
        vel.z = -vel.z;

    pos = pos + vel * dt;

    ps[i]   = pos.x;
    ps[i+1] = pos.y;
    ps[i+2] = pos.z;
    vl[i]   = vel.x;
    vl[i+1] = vel.y;
    vl[i+2] = vel.z;
}
