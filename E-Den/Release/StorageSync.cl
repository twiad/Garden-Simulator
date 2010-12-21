__kernel void StorageSync(__global const float* aMax, __global const float* aCurrent, __global const float* bMax, __global const float* bCurrent, __global float* aCurrentOut, __global float* bCurrentOut, int iNumElements)
{
    int iGID = get_global_id(0);
    if (iGID >= iNumElements) { return; }
    
    if(aMax[iGID] > 0.0f) {
      float maximumQuotient = bMax[iGID]/aMax[iGID];
      float currentSum = bCurrent[iGID]+aCurrent[iGID];
      float newb = currentSum*(maximumQuotient/(maximumQuotient + 1));
      bCurrentOut[iGID] = newb;
      aCurrentOut[iGID] = currentSum - newb;
    }
    else {
      aCurrentOut[iGID] = aCurrent[iGID];
      bCurrentOut[iGID] = bCurrent[iGID]; 
    }
    /*
    // alternative: less mem, more processing, less rounding error
    bCurrentOut[iGID] = (bCurrent[iGID] + aCurrent[iGID])*((bMax[iGID]/aMax[iGID])/((bMax[iGID]/aMax[iGID]) + 1));
    aCurrentOut[iGID] = (bCurrent[iGID] + aCurrent[iGID]) - (bCurrent[iGID] + aCurrent[iGID])*((bMax[iGID]/aMax[iGID])/((bMax[iGID]/aMax[iGID]) + 1));
    */
}
