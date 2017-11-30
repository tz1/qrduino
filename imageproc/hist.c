#include <stdio.h>
#include <string.h>

unsigned char f[4000][4000], g[4000][4000];
unsigned char hist[256];
unsigned w, h;

unsigned iabs(int a)
{
    if (a < 0)
        return -a;
    return a;
}

void hdynhist()
{
    // moving histogram over lines
    int t, mn, mx, x, y;
    for (y = 0; y < h; y++) {
        memset(hist, 0, sizeof(hist));
        // prime the pump
        for (x = 0; x < 64; x++)
            hist[f[y][x]]++;
        mn = 255;
        for (t = 0; mn == 255; t++)
            if (hist[t])
                mn = t;
        mx = 0;
        for (t = 255; !mx; t--)
            if (hist[t])
                mx = t;
        t = (mn + mx) / 2;

        for (x = 0; x < 32; x++)
            g[y][x] |= f[y][x] > t;

        for (x = 32; x < w - 32; x++) {
            t = f[y][x - 32];
            hist[t]--;
            if (t == mn && !hist[t])
                while (!hist[mn])
                    mn++;
            if (t == mx && !hist[t])
                while (!hist[mx])
                    mx--;
            t = f[y][x + 32];
            hist[t]++;
            if (t > mx)
                mx = t;
            if (t < mn)
                mn = t;

            t = (mn + mx) / 2;
            // try t = midpoint (where half are above, half are below)
            g[y][x] |= f[y][x] > t;
        }
        for (x = w - 32; x < w; x++)
            g[y][x] |= f[y][x] > t;
    }
}

void vdynhist()
{
    // moving histogram over lines
    int t, mn, mx, x, y;
    // moving histogram over vertical lines
    for (y = 0; y < w; y++) {
        memset(hist, 0, sizeof(hist));
        // prime the pump
        for (x = 0; x < 64; x++)
            hist[f[x][y]]++;
        mn = 255;
        for (t = 0; mn == 255; t++)
            if (hist[t])
                mn = t;
        mx = 0;
        for (t = 255; !mx; t--)
            if (hist[t])
                mx = t;
        t = (mn + mx) / 2;

        for (x = 0; x < 32; x++)
            g[x][y] |= f[x][y] > t;

        for (x = 32; x < h - 32; x++) {
            t = f[x - 32][y];
            hist[t]--;
            if (t == mn && !hist[t])
                while (!hist[mn])
                    mn++;
            if (t == mx && !hist[t])
                while (!hist[mx])
                    mx--;
            t = f[x + 32][y];
            hist[t]++;
            if (t > mx)
                mx = t;
            if (t < mn)
                mn = t;

            t = (mn + mx) / 2;
            g[x][y] |= f[x][y] > t;
        }
        for (x = h - 32; x < h; x++)
            g[x][y] |= f[x][y] > t;
    }
}

void readgray()
{
    char buf[512];
    unsigned x, y, m, s;
    scanf("%2s", buf);
    scanf("%u %u", &w, &h);
    scanf("%u", &s);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            g[y][x] = 0;
            scanf("%u", &m);
            f[y][x] = m;
        }
}

main()
{

    readgray();

    hdynhist();
    vdynhist();

    unsigned x, y;
    printf("P1\n%u %u\n", w, h);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
            printf("%u ", !(1 & g[y][x]));

}
