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
rfft16(register double X[])
{
    register double t0, t1, c3, d3, c2, d2;
    register int i;

    /* bit reverse */
    t0 = X[8];
    X[8] = X[1];
    X[1] = t0;
    t0 = X[4];
    X[4] = X[2];
    X[2] = t0;
    t0 = X[12];
    X[12] = X[3];
    X[3] = t0;
    t0 = X[10];
    X[10] = X[5];
    X[5] = t0;
    t0 = X[14];
    X[14] = X[7];
    X[7] = t0;
    t0 = X[13];
    X[13] = X[11];
    X[11] = t0;

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
    t0 = X[6];
    X[6] += X[7];
    X[7] = t0 - X[7];

    /* other butterflies */
    t0 = X[3] + X[2];
    X[3] = X[2] - X[3];
    X[2] = X[0] - t0;
    X[0] += t0;
    t0 = X[11] + X[10];
    X[11] = X[10] - X[11];
    X[10] = X[8] - t0;
    X[8] += t0;
    t0 = X[15] + X[14];
    X[15] = X[14] - X[15];
    X[14] = X[12] - t0;
    X[12] += t0;
    t0 = X[6] + X[4];
    X[6] = X[4] - X[6];
    X[4] = X[0] - t0;
    X[0] += t0;
    t0 = (X[5]-X[7])*INVSQ2;
    t1 = (X[5]+X[7])*INVSQ2;
    X[5] = t1 - X[3];
    X[7] = t1 + X[3];
    X[3] = X[1] - t0;
    X[1] += t0;
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

    /* reverse Imag part! */
    for(i = 16/2+1; i < 16; i++)
	X[i] = -X[i];
}
