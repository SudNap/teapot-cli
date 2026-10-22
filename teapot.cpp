// ASCII Teapot — a tiny 3D terminal demo for Termux (C++17)
// A rotating wireframe teapot-like surface with ANSI color and FPS counter.
// Build: clang++ -std=c++17 -O2 teapot.cpp -o teapot
// Run:   ./teapot
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef __unix__
#include <sys/ioctl.h>
#include <unistd.h>
#endif

struct V { double x,y,z; };
static V rot(V p, double ax, double ay) {
    double c=std::cos(ax), s=std::sin(ax);
    V a{p.x, p.y*c-p.z*s, p.y*s+p.z*c};
    c=std::cos(ay); s=std::sin(ay);
    return {a.x*c+a.z*s, a.y, -a.x*s+a.z*c};
}
static int W=80,H=28;
static void terminalSize() {
#ifdef __unix__
    winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)==0 && ws.ws_col>20 && ws.ws_row>8) {
        W=std::min<int>(ws.ws_col, 140); H=std::min<int>(ws.ws_row-2, 55);
    }
#endif
}
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    terminalSize();
    const char* ramp=" .:-=+*#%@";
    double ax=0, ay=0;
    using clock=std::chrono::steady_clock;
    auto previous=clock::now(), fpsStart=previous;
    int frames=0; double fps=0;
    std::cout << "\x1b[2J\x1b[?25l";
    while (true) {
        terminalSize();
        std::vector<char> screen(W*H,' ');
        std::vector<double> depth(W*H, 1e9);
        std::vector<int> colors(W*H, 36);
        auto plot=[&](V p, int color) {
            p=rot(p,ax,ay);
            p.z += 4.2;
            if (p.z <= 0.1) return;
            double scale=std::min(W*0.42,H*0.9);
            int x=int(W/2 + p.x*scale/p.z);
            int y=int(H/2 - p.y*scale*0.52/p.z);
            if(x<1||x>=W-1||y<1||y>=H-1) return;
            int i=y*W+x;
            if(p.z<depth[i]) { depth[i]=p.z; screen[i]=ramp[(color/4)%10]; colors[i]=color; }
        };
        // Main bowl: a parametric rounded vessel, sampled as rings and longitudinal lines.
        for(int j=0;j<=18;j++) {
            double v=0.15 + j*(2.45/18.0);
            double radius=0.82 + 0.34*std::sin(v*1.25) - 0.12*v;
            double y=0.48*std::cos(v)-0.05;
            for(int i=0;i<=90;i++) {
                double u=i*(2*M_PI/90.0);
                V p{radius*std::cos(u), y, radius*std::sin(u)};
                int color=36 + ((i/3+j*2)%6);
                plot(p,color);
            }
        }
        // Rim and base.
        for(int k=0;k<2;k++) {
            double r=k?0.53:0.96, y=k?-0.52:0.37;
            for(int i=0;i<180;i++) {
                double u=i*2*M_PI/180.0;
                plot({r*std::cos(u),y,r*std::sin(u)}, 31+(i/10)%7);
            }
        }
        // Spout: curved tube from the right side.
        for(int n=0;n<=45;n++) {
            double t=n/45.0;
            double cx=0.72+1.0*t, cy=0.05+0.72*t*t, cz=0;
            double rr=0.20*(1.0-0.72*t)+0.035;
            for(int i=0;i<24;i++) {
                double u=i*2*M_PI/24.0;
                plot({cx,cy+rr*std::cos(u),cz+rr*std::sin(u)}, 31+(i/3)%7);
            }
        }
        // Handle: a torus-like loop on the left.
        for(int i=0;i<=100;i++) {
            double t=i*2*M_PI/100.0;
            double x=-0.98-0.37*std::sin(t), y=0.00+0.62*std::cos(t), z=0;
            plot({x,y,z}, 31+(i/7)%7);
        }
        // Lid and knob.
        for(int i=0;i<100;i++) {
            double u=i*2*M_PI/100.0;
            plot({0.43*std::cos(u),0.43+0.06*std::sin(u),0.43*std::sin(u)}, 36+(i/8)%6);
        }
        for(int i=0;i<12;i++) {
            double t=i/11.0;
            plot({0.10*std::cos(t*5),0.48+t*0.22,0.10*std::sin(t*5)}, 33+i%6);
        }
        auto now=clock::now();
        double dt=std::chrono::duration<double>(now-previous).count(); previous=now;
        frames++;
        double elapsed=std::chrono::duration<double>(now-fpsStart).count();
        if(elapsed>=0.5) { fps=frames/elapsed; frames=0; fpsStart=now; }
        int hue=31+int(std::fmod(ay*20+100,7));
        std::cout << "\x1b[H";
        std::cout << "\x1b[1;38;5;"<<hue<<"m  ASCII 3D TEAPOT  \x1b[0m"
                  << "  FPS: \x1b[1;38;5;46m"<<(int)std::round(fps)<<"\x1b[0m"
                  << "  \x1b[90mCtrl+C to quit\x1b[0m\n";
        for(int y=0;y<H;y++) {
            for(int x=0;x<W;x++) {
                int i=y*W+x;
                if(screen[i]!=' ') std::cout<<"\x1b[38;5;"<<colors[i]<<"m"<<screen[i]<<"\x1b[0m";
                else std::cout<<' ';
            }
            std::cout<<'\n';
        }
        std::cout.flush();
        ay += 1.25*std::max(0.005,dt);
        ax += 0.42*std::max(0.005,dt);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
