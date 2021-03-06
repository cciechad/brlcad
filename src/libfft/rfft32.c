/*
 * BRL-CAD
 *
 * This file is a generated source file.
 * See splitditc.c for license and distribution details.
 */
/*
 * Machine-generated Real Split Radix Decimation in Time FFT
 */

#define INVSQ2 0.70710678118654752440

void
rfft32(register double X[])
{
    register double t0, t1, c3, d3, c2, d2;
    register int i;

    /* bit reverse */
    t0 = X[16];
    X[16] = X[1];
    X[1] = t0;
    t0 = X[8];
    X[8] = X[2];
    X[2] = t0;
    t0 = X[24];
    X[24] = X[3];
    X[3] = t0;
    t0 = X[20];
    X[20] = X[5];
    X[5] = t0;
    t0 = X[12];
    X[12] = X[6];
    X[6] = t0;
    t0 = X[28];
    X[28] = X[7];
    X[7] = t0;
    t0 = X[18];
    X[18] = X[9];
    X[9] = t0;
    t0 = X[26];
    X[26] = X[11];
    X[11] = t0;
    t0 = X[22];
    X[22] = X[13];
    X[13] = t0;
    t0 = X[30];
    X[30] = X[15];
    X[15] = t0;
    t0 = X[25];
    X[25] = X[19];
    X[19] = t0;
    t0 = X[29];
    X[29] = X[23];
    X[23] = t0;

    /* length two xforms */
    t0 = X[0];
    X[0] += X[1];
    X[1] = t0 - X[1];
    t0 = X[4];
    X[4] += X[5];
    X[5] = t0 - X[5];
    t0 = X[8];
    X[8] += X[9];
    X[9] = t0 - X[9];
    t0 = X[12];
    X[12] += X[13];
    X[13] = t0 - X[13];
    t0 = X[16];
    X[16] += X[17];
    X[17] = t0 - X[17];
    t0 = X[20];
    X[20] += X[21];
    X[21] = t0 - X[21];
    t0 = X[24];
    X[24] += X[25];
    X[25] = t0 - X[25];
    t0 = X[28];
    X[28] += X[29];
    X[29] = t0 - X[29];
    t0 = X[6];
    X[6] += X[7];
    X[7] = t0 - X[7];
    t0 = X[22];
    X[22] += X[23];
    X[23] = t0 - X[23];
    t0 = X[30];
    X[30] += X[31];
    X[31] = t0 - X[31];

    /* other butterflies */
    t0 = X[3] + X[2];
    X[3] = X[2] - X[3];
    X[2] = X[0] - t0;
    X[0] += t0;
    t0 = X[11] + X[10];
    X[11] = X[10] - X[11];
    X[10] = X[8] - t0;
    X[8] += t0;
    t0 = X[19] + X[18];
    X[19] = X[18] - X[19];
    X[18] = X[16] - t0;
    X[16] += t0;
    t0 = X[27] + X[26];
    X[27] = X[26] - X[27];
    X[26] = X[24] - t0;
    X[24] += t0;
    t0 = X[15] + X[14];
    X[15] = X[14] - X[15];
    X[14] = X[12] - t0;
    X[12] += t0;
    t0 = X[6] + X[4];
    X[6] = X[4] - X[6];
    X[4] = X[0] - t0;
    X[0] += t0;
    t0 = X[22] + X[20];
    X[22] = X[20] - X[22];
    X[20] = X[16] - t0;
    X[16] += t0;
    t0 = X[30] + X[28];
    X[30] = X[28] - X[30];
    X[28] = X[24] - t0;
    X[24] += t0;
    t0 = (X[5]-X[7])*INVSQ2;
    t1 = (X[5]+X[7])*INVSQ2;
    X[5] = t1 - X[3];
    X[7] = t1 + X[3];
    X[3] = X[1] - t0;
    X[1] += t0;
    t0 = (X[21]-X[23])*INVSQ2;
    t1 = (X[21]+X[23])*INVSQ2;
    X[21] = t1 - X[19];
    X[23] = t1 + X[19];
    X[19] = X[17] - t0;
    X[17] += t0;
    t0 = (X[29]-X[31])*INVSQ2;
    t1 = (X[29]+X[31])*INVSQ2;
    X[29] = t1 - X[27];
    X[31] = t1 + X[27];
    X[27] = X[25] - t0;
    X[25] += t0;
    t0 = X[12] + X[8];
    X[12] = X[8] - X[12];
    X[8] = X[0] - t0;
    X[0] += t0;
    t0 = (X[10]-X[14])*INVSQ2;
    t1 = (X[10]+X[14])*INVSQ2;
    X[10] = t1 - X[6];
    X[14] = t1 + X[6];
    X[6] = X[2] - t0;
    X[2] += t0;
    c2 = X[9]*0.923879532511286738483136 - X[11]*0.382683432365089781779233;
    d2 = -(X[9]*0.382683432365089781779233 + X[11]*0.923879532511286738483136);
    c3 = X[13]*0.382683432365089837290384 - X[15]*0.923879532511286738483136;
    d3 = -(X[13]*0.923879532511286738483136 + X[15]*0.382683432365089837290384);
    t0 = c2 + c3;
    c3 = c2 - c3;
    t1 = d2 - d3;
    d3 += d2;
    X[9] = -X[7] - d3;
    X[11] = -X[5] + c3;
    X[13] = X[5] + c3;
    X[15] = X[7] - d3;
    X[5] = X[3] + t1;
    X[7] = X[1] - t0;
    X[1] += t0;
    X[3] -= t1;
    t0 = X[24] + X[16];
    X[24] = X[16] - X[24];
    X[16] = X[0] - t0;
    X[0] += t0;
    t0 = (X[20]-X[28])*INVSQ2;
    t1 = (X[20]+X[28])*INVSQ2;
    X[20] = t1 - X[12];
    X[28] = t1 + X[12];
    X[12] = X[4] - t0;
    X[4] += t0;
    c2 = X[17]*0.980785280403230430579242 - X[23]*0.195090322016128248083788;
    d2 = -(X[17]*0.195090322016128248083788 + X[23]*0.980785280403230430579242);
    c3 = X[25]*0.831469612302545235671403 - X[31]*0.555570233019602177648721;
    d3 = -(X[25]*0.555570233019602177648721 + X[31]*0.831469612302545235671403);
    t0 = c2 + c3;
    c3 = c2 - c3;
    t1 = d2 - d3;
    d3 += d2;
    X[17] = -X[15] - d3;
    X[23] = -X[9] + c3;
    X[25] = X[9] + c3;
    X[31] = X[15] - d3;
    X[9] = X[7] + t1;
    X[15] = X[1] - t0;
    X[1] += t0;
    X[7] -= t1;
    c2 = X[18]*0.923879532511286738483136 - X[22]*0.382683432365089781779233;
    d2 = -(X[18]*0.382683432365089781779233 + X[22]*0.923879532511286738483136);
    c3 = X[26]*0.382683432365089837290384 - X[30]*0.923879532511286738483136;
    d3 = -(X[26]*0.923879532511286738483136 + X[30]*0.382683432365089837290384);
    t0 = c2 + c3;
    c3 = c2 - c3;
    t1 = d2 - d3;
    d3 += d2;
    X[18] = -X[14] - d3;
    X[22] = -X[10] + c3;
    X[26] = X[10] + c3;
    X[30] = X[14] - d3;
    X[10] = X[6] + t1;
    X[14] = X[2] - t0;
    X[2] += t0;
    X[6] -= t1;
    c2 = X[19]*0.831469612302545235671403 - X[21]*0.555570233019602177648721;
    d2 = -(X[19]*0.555570233019602177648721 + X[21]*0.831469612302545235671403);
    c3 = X[27]*-0.195090322016128192572637 - X[29]*0.980785280403230430579242;
    d3 = -(X[27]*0.980785280403230430579242 + X[29]*-0.195090322016128192572637);
    t0 = c2 + c3;
    c3 = c2 - c3;
    t1 = d2 - d3;
    d3 += d2;
    X[19] = -X[13] - d3;
    X[21] = -X[11] + c3;
    X[27] = X[11] + c3;
    X[29] = X[13] - d3;
    X[11] = X[5] + t1;
    X[13] = X[3] - t0;
    X[3] += t0;
    X[5] -= t1;

    /* reverse Imag part! */
    for(i = 32/2+1; i < 32; i++)
	X[i] = -X[i];
}
