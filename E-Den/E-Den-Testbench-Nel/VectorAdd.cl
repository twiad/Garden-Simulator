__kernel void VectorAdd(__global const float* a, __global const float* b, __global float* c, int iNumElements)
{
    int iGID = get_global_id(0);
    if (iGID >= iNumElements) { return; }
    c[iGID] = a[iGID] + b[iGID];
}
