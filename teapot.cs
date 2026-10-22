// ASCII Teapot — .NET 8 console demo. Runs in Termux with dotnet.
// This is a lightweight software-rendered 3D-style terminal animation.
using System;
using System.Diagnostics;
using System.Threading;

const int W = 76, H = 26;
const string ramp = " .:-=+*#%@";
double ax = 0, ay = 0, fps = 0;
int frames = 0;
var sw = Stopwatch.StartNew();
double last = sw.Elapsed.TotalSeconds, fpsStart = last;
Console.CursorVisible = false;
Console.Clear();
try {
    while (true) {
        char[] screen = new char[W * H];
        int[] colors = new int[W * H];
        double[] depth = new double[W * H];
        Array.Fill(screen, ' ');
        Array.Fill(depth, double.MaxValue);

        void Plot(double x, double y, double z, int color) {
            double c = Math.Cos(ax), s = Math.Sin(ax);
            double yy = y * c - z * s, zz = y * s + z * c;
            c = Math.Cos(ay); s = Math.Sin(ay);
            double xx = x * c + zz * s; zz = -x * s + zz * c;
            zz += 4.2;
            if (zz <= 0.1) return;
            double scale = Math.Min(W * .42, H * .9);
            int px = (int)(W / 2 + xx * scale / zz);
            int py = (int)(H / 2 - yy * scale * .52 / zz);
            if (px < 1 || px >= W - 1 || py < 1 || py >= H - 1) return;
            int idx = py * W + px;
            if (zz < depth[idx]) {
                depth[idx] = zz;
                screen[idx] = ramp[(color / 4) % ramp.Length];
                colors[idx] = color;
            }
        }

        // Rounded bowl surface.
        for (int j = 0; j <= 18; j++) {
            double v = .15 + j * (2.45 / 18.0);
            double radius = .82 + .34 * Math.Sin(v * 1.25) - .12 * v;
            double y = .48 * Math.Cos(v) - .05;
            for (int i = 0; i <= 90; i++) {
                double u = i * (2 * Math.PI / 90.0);
                Plot(radius * Math.Cos(u), y, radius * Math.Sin(u), 31 + (i / 3 + j * 2) % 7);
            }
        }
        // Rim and foot.
        for (int k = 0; k < 2; k++) {
            double r = k == 1 ? .53 : .96, y = k == 1 ? -.52 : .37;
            for (int i = 0; i < 180; i++) {
                double u = i * 2 * Math.PI / 180.0;
                Plot(r * Math.Cos(u), y, r * Math.Sin(u), 31 + (i / 10) % 7);
            }
        }
        // Spout.
        for (int n = 0; n <= 45; n++) {
            double t = n / 45.0, cx = .72 + t, cy = .05 + .72 * t * t;
            double rr = .20 * (1 - .72 * t) + .035;
            for (int i = 0; i < 24; i++) {
                double u = i * 2 * Math.PI / 24.0;
                Plot(cx, cy + rr * Math.Cos(u), rr * Math.Sin(u), 31 + (i / 3) % 7);
            }
        }
        // Handle loop.
        for (int i = 0; i <= 100; i++) {
            double t = i * 2 * Math.PI / 100.0;
            Plot(-.98 - .37 * Math.Sin(t), .62 * Math.Cos(t), 0, 31 + (i / 7) % 7);
        }
        // Lid and knob.
        for (int i = 0; i < 100; i++) {
            double u = i * 2 * Math.PI / 100.0;
            Plot(.43 * Math.Cos(u), .43 + .06 * Math.Sin(u), .43 * Math.Sin(u), 36 + (i / 8) % 6);
        }
        for (int i = 0; i < 12; i++) {
            double t = i / 11.0;
            Plot(.1 * Math.Cos(t * 5), .48 + t * .22, .1 * Math.Sin(t * 5), 33 + i % 6);
        }

        double now = sw.Elapsed.TotalSeconds, dt = now - last; last = now;
        frames++;
        if (now - fpsStart >= .5) { fps = frames / (now - fpsStart); frames = 0; fpsStart = now; }
        Console.SetCursorPosition(0, 0);
        Console.Write($"\u001b[1;38;5;{31 + (int)(Math.Abs(ay * 20) % 7)}m ASCII 3D TEAPOT \u001b[0m  FPS: \u001b[1;38;5;46m{fps:0}\u001b[0m  Ctrl+C to quit\n");
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int idx = y * W + x;
                if (screen[idx] != ' ') Console.Write($"\u001b[38;5;{colors[idx]}m{screen[idx]}\u001b[0m");
                else Console.Write(' ');
            }
            Console.WriteLine();
        }
        ay += 1.25 * Math.Max(.005, dt);
        ax += .42 * Math.Max(.005, dt);
        Thread.Sleep(16);
    }
}
finally {
    Console.CursorVisible = true;
    Console.Write("\u001b[0m\n");
}
