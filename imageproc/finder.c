#include <stdio.h>

unsigned w, h;
unsigned char f[4000][4000];
unsigned runs[4000];
unsigned iabs(int a)
{
    if (a < 0)
        return -a;
    return a;
}

#if 0
int despeckle(int max)
{
    int x, y;

    for (x = 1; x < max - 1; x++) {
        if (runs[x] != 1)
            continue;
        if (runs[x - 1] == 1)
            continue;
        if (runs[x + 1] == 1)
            continue;
        runs[x - 1] += 1 + runs[x + 1];
        max -= 2;
        for (y = x; y < max - 1; y++)
            runs[y] = runs[y + 2];
        x--;
    }
    return max;
}
#endif

int isfinder(int x)
{
    int a, b, c, d, e, m;
    a = runs[x];
    e = runs[x + 4];
    if (iabs(a - e) > (a + e) / 4)
        return 0;
    b = runs[x + 1];
    d = runs[x + 3];
    if (iabs(b - d) > (b + d) / 4)
        return 0;
    c = runs[x + 2];
    m = a + e + (b + d) / 2;
    if (iabs(c - m) > (c + m) / 4)
        return 0;
#if 0
    m = runs[x - 1] + runs[x + 5];      // 4 module border
    if (m < c + b)
        return 2;
#endif
    return 1;
}

void readimage()
{
    char buf[512];
    unsigned x, y;
    int cx;
    scanf("%2s", buf);
    scanf("%u %u", &w, &h);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            for (;;) {
                cx = getchar();
                if (cx < 0)
                    break;
                if (cx == '1') {
                    f[y][x] = 1;
                    break;
                }
                if (cx == '0') {
                    f[y][x] = 0;
                    break;
                }
            }
        }
}

void horizontalruns()
{
    unsigned x, y;
    unsigned r, b, i, m;
    // horizontal runs
    for (y = 0; y < h; y++) {
        b = 0, r = 0, i = 0;
        runs[i] = 0;
        for (x = 0; x < w; x++) {
            if ((1 & f[y][x]) == b) {
                r++;
                continue;
            }
            b = 1 & f[y][x];
            runs[i++] = r;
            runs[i] = 0;
            r = 1;
        }
        runs[i++] = r;          // last 
        runs[i++] = 1;          //  edge
        if (i < 6)
            continue;
#if 0
        i = despeckle(i);
        if (i < 6)
            continue;
#endif
        r = 0;
        for (x = 1; x < i - 4; x += 2) {
            r += runs[x] + runs[x - 1];
            if (!isfinder(x))
                continue;
            m = runs[x + 2];
            for (b = 0; b < m; b++)
                f[y][r + runs[x + 1] + b] |= 2;
        }
    }
}

void verticalruns()
{
    unsigned x, y;
    unsigned r, b, i, m;
    // vertical runs
    for (y = 0; y < w; y++) {
        b = 0, r = 0, i = 0;
        runs[i] = 0;
        for (x = 0; x < h; x++) {
            if ((1 & f[x][y]) == b) {
                r++;
                continue;
            }
            b = 1 & f[x][y];
            runs[i++] = r;
            runs[i] = 0;
            r = 1;
        }
        runs[i++] = r;
        runs[i++] = 1;
        if (i < 6)
            continue;
#if 0
        i = despeckle(i);
        if (i < 6)
            continue;
#endif
        r = 0;
        for (x = 1; x < i - 4; x += 2) {
            r += runs[x] + runs[x - 1];
            if (!isfinder(x))
                continue;
            m = runs[x + 2];
            for (b = 0; b < m; b++)
                f[r + runs[x + 1] + b][y] |= 4;
        }
    }
}

void lrdilate()
{
    unsigned x, y, b, c;
    for (y = 0; y < h; y++)
        for (x = 0; x < w - 1; x++) {
            if (y > 0 && f[y - 1][x] == 7)
                continue;
            if (f[y][x] == 7 && f[y][x + 1] & 1) {
                b = 0;
                c = 0;
                // find height and adjacent black
                while (f[y + b][x] == 7 && y + b < h) {
                    if (f[y + b][x + 1] & 1)
                        c++;
                    b++;
                }
                // black adjacent? - despeckle
                if (c < b)
                    continue;
                for (c = 0; c < b; c++)
                    f[y + c][x + 1] |= 6;
            }
        }
}

void rldilate()
{
    unsigned x, y, b, c;
    for (y = 0; y < h; y++)
        for (x = w; x > 1; x--) {
            if (y > 0 && f[y - 1][x] == 7)
                continue;
            if (f[y][x] == 7 && f[y][x - 1] & 1) {
                b = 0;
                c = 0;
                // find height and adjacent black
                while (f[y + b][x] == 7 && y + b < h) {
                    if (f[y + b][x - 1] & 1)
                        c++;
                    b++;
                }
                // black adjacent? - despeckle
                if (c < b)
                    continue;
                for (c = 0; c < b; c++)
                    f[y + c][x - 1] |= 6;
            }
        }
}

void tbdilate()
{
    unsigned x, y, b, c;
    for (y = 0; y < h - 1; y++)
        for (x = 0; x < w; x++) {
            if (x > 0 && f[y][x - 1] == 7)
                continue;
            if (f[y][x] == 7 && f[y + 1][x] & 1) {
                b = 0;
                c = 0;
                // find height and adjacent black
                while (f[y][x + b] == 7 && x + b < w) {
                    if (f[y + 1][x + b] & 1)
                        c++;
                    b++;
                }
                // black adjacent? - despeckle
                if (c < b)
                    continue;
                for (c = 0; c < b; c++)
                    f[y + 1][x + c] |= 6;
            }
        }
}

void btdilate()
{
    unsigned x, y, b, c;
    for (y = h; y > 1; y--)
        for (x = 0; x < w; x++) {
            if (x > 0 && f[y][x - 1] == 7)
                continue;
            if (f[y][x] == 7 && f[y - 1][x] & 1) {
                b = 0;
                c = 0;
                // find height and adjacent black
                while (f[y][x + b] == 7 && x + b < w) {
                    if (f[y - 1][x + b] & 1)
                        c++;
                    b++;
                }
                // black adjacent? - despeckle
                if (c < b)
                    continue;
                for (c = 0; c < b; c++)
                    f[y - 1][x + c] |= 6;
            }
        }
}

int main()
{
    readimage();
    horizontalruns();
    verticalruns();

    lrdilate();
    tbdilate();
    rldilate();
    btdilate();

    unsigned x, y;
    printf("P2\n%u %u\n255\n", w, h);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
#if 0
            printf("%d ", f[y][x]);
            continue;
#endif
            if (f[y][x] == 7)
                printf("%d ", 255);
            else
#if 0
                printf("%d ", (7 & f[y][x]) << 4);
#else
                printf("%d ", 128 - ((1 & f[y][x]) << 6));
#endif
        }

}
