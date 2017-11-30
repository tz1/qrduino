// Copyright 2010, tz, released under the GNU General Public License v3.

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import javax.imageio.*;
import javax.swing.*;

public class QuickFinderFinder extends Component {
    BufferedImage img;


    // line 7, 1 pixel per module would be a special case I don't handle
    int lasty;

    long ave;
    int modwidx8;
    int finds;

    int[] fx = new int[32];
    int[] fy = new int[32];
    int[] fw = new int[32];
    int[] fh = new int[32];

    int width, height;

    int getlum(int y, int x) {
        int argb = img.getRGB(x,y);
        argb = argb >> 8;
        argb = argb & 255;
        return argb;
    }

    int iabs(int a)
    {
        if (a < 0)
            return -a;
        return a;
    }

    int center(int x, int y, long lave)
    {
        // use u d l r
        int r, v, s;
        int fl, fr, ft, fb;
        int fl1, fr1, ft1, fb1;

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

        System.out.print("C"+finds+": "+fx[finds]+" "+fy[finds]+" "+fw[finds]+" "+fh[finds]);
            System.out.print("  "+fl+" "+fr+" "+ft+" "+fb);
            System.out.println("  "+fl1+" "+fr1+" "+ft1+" "+fb1);

        if (fw[finds] * 3 < fh[finds] * 2)
            return 0;
        if (fw[finds] * 2 > fh[finds] * 3)
            return 0;
        if (0 != modwidx8) {
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
        if (fl * 3 > v || fr * 3 > v || ft * 3 > v || fb * 3 > v)
            return 0;
        if (fl1 * 3 > v || fr1 * 3 > v || ft1 * 3 > v || fb1 * 3 > v)
            return 0;
        modwidx8 = v;
        finds++;
        return 1;
    }

    int runs[] = new int[8];

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
        if (0 != modwidx8) {
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

    byte findit()
    {
        int x0, x, y, r, xx, mx;
        byte i, b, v;

        // 13 for 2 pixel min, do line 7 if simple 1 ppmod
        finds = 0;
        for (y = lasty; y < height || y < width; y += 2, lasty += 2) {
            x0 = 0;
            if (y >= height)     // off bottom, don't count
                x0 = 1 + y - height;
            mx = y;
            if( y > width )
                mx = width;
            //            System.out.println(y+" "+ave+" "+mx+" "+x0);
            ave = 0;
            for (x = x0; x < mx ; x++)
                ave += getlum(y - x, x);
            ave = ave / (mx - x0);
            b = 0; r = 0; i = 0;
            // Note that we only need the current 5 runs, not a  list
            runs[i] = 0;
            for (x = x0; x < mx; x++) {
                if( getlum(y - x, x) < ave )
                    v = 1;
                else
                    v = 0;
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
                //System.out.println(runs[1]+" "+runs[2]+" "+runs[3]+" "+runs[4]+" "+runs[5]);
                if (0 == checkfinder())
                    continue;
                xx = x - runs[5] - runs[4] - (runs[3] / 2);
                center(xx, y - xx, ave);
                if (0 != modwidx8)
                    return 1;
            }
        }
        return 0;
    }

    void findnexty(int x, int y)
    {
        byte b = 0;
        byte v;
        byte i = 0;
        int r = 0;
        long avey = ave * 64;
        runs[0] = 0;
        for (; y < height; y++) {
            avey += getlum(y, x);
            avey -= avey / 64;
            if( getlum(y, x) <= avey / 64 )
                v = 1;
            else
                v = 0;

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
            if (0 == checkfinder())
                continue;
            center(x, y - runs[5] - runs[4] - runs[3] / 2, avey / 64);
        }
        return;
    }

    void findnextx(int x, int y)
    {
        byte b = 0;
        byte v;
        byte i = 0;
        int r = 0;
        long avex = ave * 64;
        runs[0] = 0;
        for (; x < width; x++) {
            avex += getlum(y, x);
            avex -= avex / 64;
            if( getlum(y, x) <= avex / 64 )
                v = 1;
            else
                v = 0;

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
            if (0 == checkfinder())
                continue;
            center(x - runs[5] - runs[4] - (runs[3] / 2), y, avex / 64);
        }
        return;
    }

    int TEST(int x,int y) { return (x*y/modwidx8); }

    void findfinders() {

        width = img.getWidth();
        height = img.getHeight();

        for( lasty = 13; lasty < width || lasty < height; lasty += 2 ) {

            if (0 == findit())
                return;
            findnexty(fx[0], fy[0]);
            findnextx(fx[0], fy[0]);

            //        System.out.println("Found2:" + finds);
            int i,j;

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
                    if (iabs(fx[i] - fx[j]) < modwidx8 / 2 && iabs(fy[i] - fy[j]) < modwidx8 / 2) {     // coincident centers
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
                        //                    fprintf(stderr, "A %d %d = %d\n", i, j, k);
                    }
            }

            // pick most likely 3
            for (i = 0; i < finds; i++) {
                //            fprintf(stderr, "%d : %d,%d %d %d\n", (i == 0 || i == besti || i == bestj), fx[i], fy[i], fw[i], fh[i]);
                System.out.println(i+":"+fx[i]+":"+fy[i]+":"+fw[i]+":"+fh[i]);
                if( finds > 2 && (i == 0 || i == besti || i == bestj) ) {

                    // Mark Found Finder
                    for (j = 0; j < fw[i]; j++)
                        img.setRGB( fx[i] - fw[i] / 2 + j, fy[i], img.getRGB(fx[i] - fw[i] / 2 + j, fy[i]) | (255<<8) );
                    for (j = 0; j < fh[i]; j++)
                        img.setRGB( fx[i], fy[i] - fh[i] / 2 + j, img.getRGB( fx[i], fy[i] - fh[i] / 2 + j) | (255<<8) );
                }
                else {
                    for (j = 0; j < 3; j++)
                        img.setRGB( fx[i] - 1 + j, fy[i], img.getRGB( fx[i] - 1 + j, fy[i]) | (255<<16) );
                    for (j = 0; j < 3; j++)
                        img.setRGB( fx[i], fy[i] - 1 + j, img.getRGB( fx[i], fy[i] - 1 + j) | (255<<16) );
                }
            }

            // try harder at next diagonal
            if( finds > 2 )
                break;
        }
    }

////////////////////////////////////////////////////////////////////////




    public void paint(Graphics g) {
        g.drawImage(img, 0, 0, null);
    }

    public QuickFinderFinder(String s)  {
        try {
            img = ImageIO.read(new File(s));
            findfinders();
        } catch (IOException e) {
        }
    }
    public Dimension getPreferredSize() {
        if (img == null) 
            return new Dimension(10,10);
        else 
            return new Dimension(img.getWidth(null), img.getHeight(null));
    }
    public static void main(String[] args) {
        JFrame f = new JFrame("Quick Finder Finder");

        f.addWindowListener(new WindowAdapter(){
                public void windowClosing(WindowEvent e) {
                    System.exit(0);
                }
            });
        f.add(new QuickFinderFinder(args[0]));
        f.pack();
        f.setVisible(true);
    }
}
