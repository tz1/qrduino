#include <stdio.h>
#include <string.h>

// line 7, 1 pixel per module would be a special case I don't handle
unsigned lasty;

unsigned long ave;
unsigned modwidx8;
unsigned finds;
#define MAXFINDS 16
unsigned fx[MAXFINDS];
unsigned fy[MAXFINDS];
unsigned fh[MAXFINDS];
unsigned fw[MAXFINDS];

unsigned char *image;
unsigned width, height;

unsigned char getlum(unsigned y, unsigned x) {
    return image[ y * width + x ];
}

unsigned iabs(int a)
{
    if (a < 0)
        return -a;
    return a;
}

int center(unsigned x, unsigned y, unsigned lave)
{
    // use u d l r
    unsigned r, v, s;
    unsigned fl, fr, ft, fb;
    unsigned fl1, fr1, ft1, fb1;

    for (r = x - 1; r > 0; r--)
        if (getlum(y, r) > lave)
            break;
    s = r;
    for (; s > 0; s--)
        if (getlum(y, s) < lave)
            break;
    fl = r - s;
    for (; s > 0; s--)
        if (getlum(y, s) > lave)
            break;
    fl1 = r - s - fl;

    r++;
    for (v = x + 1; v < width; v++)
        if (getlum(y, v) > lave)
            break;
    s = v;
    for (; s < width; s++)
        if (getlum(y, s) < lave)
            break;
    fr = s - v;
    for (; s < width; s++)
        if (getlum(y, s) > lave)
            break;
    fr1 = s - v - fr;
    v--;
    x = (r + v) / 2;
    fx[finds] = x;
    fw[finds] = v - r;

    for (r = y - 1; r > 0; r--)
        if (getlum(r, x) > lave)
            break;
    s = r;
    for (; s > 0; s--)
        if (getlum(s, x) < lave)
            break;
    ft = r - s;
    for (; s > 0; s--)
        if (getlum(s, x) > lave)
            break;
    ft1 = r - s - ft;

    r++;
    for (v = y + 1; v < height; v++)
        if (getlum(v, x) > lave)
            break;
    s = v;
    for (; s < height; s++)
        if (getlum(s, x) < lave)
            break;
    fb = s - v;
    for (; s < height; s++)
        if (getlum(s, x) > lave)
            break;
    fb1 = s - v - fb;
    v--;
    y = (r + v) / 2;
    fy[finds] = y;
    fh[finds] = v - r;

#if 0
    fprintf(stderr, "C%d: %d,%d %d %d  %d %d %d %d  %d %d %d %d\n", finds, fx[finds], fy[finds], fw[finds], fh[finds],
      fl, fr, ft, fb, fl1, fr1, ft1, fb1);
#endif
    if (fw[finds] * 3 < fh[finds] * 2)
        return 0;
    if (fw[finds] * 2 > fh[finds] * 3)
        return 0;
    if (modwidx8) {
        if (fw[finds] * 4 < modwidx8 || fw[finds] > modwidx8 / 2)
            return 0;
        if (fh[finds] * 4 < modwidx8 || fh[finds] > modwidx8 / 2)
            return 0;
        // for j==0  too could check lrtb for same width and trace out a square - already checked in scan direction but not perpindicular
        if (fl * 3 > modwidx8 || fr * 3 > modwidx8 || ft * 3 > modwidx8 || fb * 3 > modwidx8)
            return 0;
        if (fl1 * 3 > modwidx8 || fr1 * 3 > modwidx8 || ft1 * 3 > modwidx8 || fb1 * 3 > modwidx8)
            return 0;
        finds++;
        return 1;
    }

    v = fw[finds] + fh[finds] + (ft + fb + fl + fr) / 2;
    // thresholding unstable so maybe shut off
#if 1
    if (fl * 3 > v || fr * 3 > v || ft * 3 > v || fb * 3 > v)
        return 0;
    if (fl1 * 3 > v || fr1 * 3 > v || ft1 * 3 > v || fb1 * 3 > v)
        return 0;
#endif
    modwidx8 = v;
    finds++;
    return 1;
}

unsigned runs[8];

int checkfinder()
{
    int a, b, c, d, e, m;
    a = runs[1];
    e = runs[5];
    if (iabs(a - e) > (a + e + 1) / 4)
        return 0;
    b = runs[2];
    d = runs[4];
    if (iabs(b - d) > (b + d + 1) / 4)
        return 0;
    if ((a + e) * 2 < (b + d))
        return 0;
    if ((b + d) * 2 < (a + e))
        return 0;
    c = runs[3];
    if (modwidx8) {
#if 0
    fprintf( stderr, "r %d - %d %d %d %d %d , %d\n", 
             finds, runs[1],runs[2],runs[3],runs[4],runs[5], modwidx8 );
#endif
        if (c * 10 < modwidx8 * 3)
            return 0;
        if (c * 6 > modwidx8 * 3)
            return 0;
    }
    m = a + e + (b + d + 1) / 2;
    if (iabs(c - m) > (c + m) / 4)
        return 0;
    return 1;
}

unsigned char findit()
{
    unsigned x0, x, y, r, xx, mx;
    unsigned char i, b, v;

    // 13 for 2 pixel min, do line 7 if simple 1 ppmod
    finds = 0;
    for (y = lasty; y < height || y < width; y += 2, lasty += 2) {
        x0 = 0;
        if (y >= height)     // off bottom, don't count
            x0 = 1 + y - height;
        mx = y;
        if( y > width )
            mx = width;
        ave = 0;
        for (x = x0; x < mx ; x++)
            ave += getlum(y - x, x);
        ave /= mx - x0;
        b = 0; r = 0; i = 0;
        // Note that we only need the current 5 runs, not a  list
        runs[i] = 0;
        for (x = x0; x < mx; x++) {
            v = getlum(y - x, x) < ave;
            if (v == b) {
                r++;
                continue;
            }
            b = v;
            runs[i++] = r;
            runs[i] = 0;
            r = 1;
            if (i > 6) {
                for (v = 0; v < 6; v++)
                    runs[v] = runs[v + 2];
                i -= 2;
            }
            if (i < 6)
                continue;
            if (!checkfinder())
                continue;
            xx = x - runs[5] - runs[4] - (runs[3] / 2);
            center(xx, y - xx, ave);
            if (modwidx8)
                return 1;
        }
    }
    return 0;
}

void findnexty(unsigned x, unsigned y)
{
    unsigned char b = 0, v, i = 0;
    unsigned r = 0;
    unsigned avey = ave * 64;
    runs[0] = 0;
    for (; y < height; y++) {
        avey += getlum(y, x);
        avey -= avey / 64;
        v = getlum(y, x) <= avey / 64;
        if (v == b) {
            r++;
            if (y + 1 != height)
                continue;
        }
        b = v;
        runs[i++] = r;
        runs[i] = 0;
        r = 1;
        if (i > 6) {
            for (v = 0; v < 6; v++)
                runs[v] = runs[v + 2];
            i -= 2;
        }
        if (i < 6)
            continue;
        if (runs[1] * 10 < modwidx8 || runs[1] * 6 > modwidx8)
            continue;
        if (!checkfinder())
            continue;
        center(x, y - runs[5] - runs[4] - runs[3] / 2, avey / 64);
    }
    return;
}

void findnextx(unsigned x, unsigned y)
{
    unsigned char b = 0, v, i = 0;
    unsigned r = 0;
    unsigned avex = ave * 64;
    runs[0] = 0;
    for (; x < width; x++) {
        avex += getlum(y, x);
        avex -= avex / 64;
        v = getlum(y, x) <= avex / 64;
        if (v == b) {
            r++;
            if (x + 1 != width)
                continue;
        }
        b = v;
        runs[i++] = r;
        runs[i] = 0;
        r = 1;
        if (i > 6) {
            for (v = 0; v < 6; v++)
                runs[v] = runs[v + 2];
            i -= 2;
        }
        if (i < 6)
            continue;
        if (runs[1] * 10 < modwidx8 || runs[1] * 6 > modwidx8)
            continue;
        if (!checkfinder())
            continue;
        center(x - runs[5] - runs[4] - (runs[3] / 2), y, avex / 64);
    }
    return;
}

void readgray()
{
    char buf[8];
    unsigned x, y, m, s;
    scanf("%2s", buf);
    scanf("%u %u", &width, &height);
    scanf("%u", &s);
    image = malloc(width*height);
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++) {
            scanf("%u", &m);
            image[y*width+x] = m * 255 / s;
        }
}

int main(int argc, char *argv[])
{
    int i, j;
    readgray();

    // For true quickie, the || should be && here and in findit since the finder should be in the UL corner
    for (lasty = 13; lasty < width || lasty < height ; lasty += 2) {
        modwidx8 = 0;
        if (!findit())
            return 1;

        findnexty(fx[0], fy[0]);
        findnextx(fx[0], fy[0]);

        j = finds;
        i = 1;
        while (i < j) {
            findnexty(fx[i], fy[i]);
            findnextx(fx[i], fy[i]);
            i++;
        }

        i = j + 1;
        j = finds;
        while (i < j) {
            findnexty(fx[i], fy[i]);
            findnextx(fx[i], fy[i]);
            i++;
        }

        if (finds < 3) {
            // try harder, misalignment
            findnexty(fx[0] - fw[0] / 2, fy[0]);
            findnexty(fx[0] + fw[0] / 2, fy[0]);
            findnextx(fx[0], fy[0] - fh[0] / 2);
            findnextx(fx[0], fy[0] + fh[0] / 2);
        }

        for (i = 0; i < finds - 1; i++)
            for (j = i + 1; j < finds; j++)
                if (iabs(fx[i] - fx[j]) < modwidx8 / 2 
                    && iabs(fy[i] - fy[j]) < modwidx8 / 2) {     // coincident centers
                    //                fprintf(stderr, "DUP - %d,%d %d %d\n", fx[i], fy[i], fw[i], fh[i]);
                    if (j < finds - 1) {
                        fx[j] = fx[finds - 1];
                        fy[j] = fy[finds - 1];
                        fw[j] = fw[finds - 1];
                        fh[j] = fh[finds - 1];
                        j--;
                    }
                    finds--;
                }

        int besti = 1;
        int bestj = 2;
        int bestk = 0;
        if (finds > 2) {
            for (i = 1; i < finds - 1; i++)
                for (j = i + 1; j < finds; j++) {
                    int k, m;
                    // smallest side of largest rectangle
#define TEST(x,y) (x*y)
                    k = TEST(iabs(fx[0] - fx[i]), iabs(fy[0] - fy[i]));
                    m = TEST(iabs(fx[0] - fx[j]), iabs(fy[0] - fy[j]));
                    if (m > k)
                        k = m;
                    m = TEST(iabs(fx[j] - fx[i]), iabs(fy[j] - fy[i]));
                    if (m > k)
                        k = m;
                    if (k > bestk) {
                        besti = i;
                        bestj = j;
                        bestk = k;
                    }
                    fprintf(stderr, "A %d %d = %d\n", i, j, k);
                }
        }

        // pick most likely 3
        for (i = 0; i < finds; i++) {
            fprintf(stderr, "%d : %d,%d %d %d\n", (i == 0 || i == besti || i == bestj), fx[i], fy[i], fw[i], fh[i]);
            if( i && finds < 3 )
                continue;
            if( finds > 2 && (i == 0 || i == besti || i == bestj) ) {
                for (j = 0; j < fw[i]; j++)
                    image[fy[i] * width +  fx[i] - fw[i] / 2 + j] = 255;
                for (j = 0; j < fh[i]; j++)
                    image[(fy[i] - fh[i] / 2 + j) * width + fx[i]] = 255;
            }
            else {
                for (j = 0; j < 3; j++)
                    image[fy[i] * width +  fx[i] - 1 + j] = 255;
                for (j = 0; j < 3; j++)
                    image[(fy[i] - 1 + j) * width + fx[i]] = 255;
            }
        }
        fprintf(stderr, "\n");

        // try harder at next diagonal
        if (finds > 2)
            break;
    }

    printf("P2\n%u %u\n255\n", width, height);
    for (j = 0; j < height; j++)
        for (i = 0; i < width; i++)
            printf("%u ", getlum(j, i));

    return 0;
}
