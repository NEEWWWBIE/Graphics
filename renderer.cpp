#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

double p = 20;
double n = -20;
double z1 = 100;
double z2 = 140;

void init()
{
    glClearColor(1.0,1.0,1.0,1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,799.0,0.0,649.0);
}

    class _3dpoint
    {
    public:
        float x,y,z;
        _3dpoint():x(0),y(0),z(0){}
        _3dpoint(float xx,float yy,float zz):x(xx),y(yy),z(zz){}
    };

    class  _2dpoint
    {
    public:
        int h, v;
        _2dpoint():h(0),v(0){}
        _2dpoint(float xx,float yy):h(xx),v(yy){}
    };

    _3dpoint from(0,0,-50);

    class Camera
    {
    public:
        float anglev,angleh,zoom,z_near,z_far;
        short projection;
        _3dpoint to, up;
        Camera()
        {
            to = _3dpoint(0,0,50); // from-to gives the normal vector
            up = _3dpoint(0,1,0); // up vector
            anglev = 45.0;
            angleh = 45.0;
            zoom = 1.0;
            z_near = 1.0;
            z_far = 200.0;
            projection = 0;
        }
    };

    class Screen//window >> width=1440 ,height= 870 //viewplane >> left=320 ,top=20,right=1120 and bottom=820.
    {
    public:
        _2dpoint center, size;
        Screen():center(_2dpoint(720,420)),size(_2dpoint(800,800)){}
    };

    class Projection
    {
    private:
        _3dpoint origin;
        _3dpoint e1, e2, n1, n2;
        Camera camera;
        Screen screen;
        double tan_anglev, tan_angleh;
        _3dpoint u, v, n;
        double epsilon;
        double dtor;
    public:
        _2dpoint p1, p2;
        Projection()
        {
            origin = _3dpoint();
            e1 = _3dpoint();e2 = _3dpoint();
            n1 = _3dpoint();n2 = _3dpoint();
            camera = Camera();
            screen = Screen();
            u = _3dpoint();v = _3dpoint();n = _3dpoint();
            epsilon = 0.001;
            dtor = 0.01745329252;
            p1 = _2dpoint();
            p2 = _2dpoint();

            if (Trans_Initialize()!=true){
                cout<<"variables initializing error.";
                //exit(0);
            }

        }

         bool Trans_Initialize()
            {
                if (EqualVertex(camera.to,from))
                {// camera position and view vector
                    cout<<"-1";
                    return (false);
                }
                if (EqualVertex(camera.up, origin)) // camera up vector
                {
                    cout<<"0";
                    return (false);
                }

                n.x = camera.to.x - from.x;
                n.y = camera.to.y - from.y;
                n.z = camera.to.z - from.z;

                Normalize (n);

                u = CrossProduct(camera.up, n);
                Normalize (u);

                if (EqualVertex(u, origin))
                {
                    cout<<"1";
                    return (false);
                }

                v = CrossProduct(n, u);

                if (camera.angleh < epsilon || camera.anglev < epsilon)
                {// camera arperture
                    cout<<"2";
                    return (false);
                }
                tan_angleh = tan(camera.angleh * dtor / 2);
                tan_anglev = tan(camera.anglev * dtor / 2);

                if (camera.zoom < epsilon)
                {//check the zoom factor
                    cout<<"3";
                    return (false);
                }
                if (camera.z_near < 0 || camera.z_far < 0 || camera.z_near >= camera.z_far)
                {
                    cout<<"4";
                    return (false);
                }
                cout<<"No error";
                return (true);
            }

            bool EqualVertex(_3dpoint a, _3dpoint b)
            {
                if ((a.x - b.x)>=epsilon)
                    return (false);
                if ((a.y - b.y)>=epsilon)
                    return (false);
                if ((a.z - b.z)>=epsilon)
                    return (false);
            }

            _3dpoint CrossProduct(_3dpoint a, _3dpoint b)
            {
                _3dpoint  p;
                p =   _3dpoint();
                p.x = a.y*b.z - a.z*b.y;
                p.y = a.z*b.x - a.x*b.z;
                p.z = a.x*b.y - a.y*b.x;
                return p;
            }

            void Normalize(_3dpoint  a)
            {
                double length;

                length = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
                a.x /= length;
                a.y /= length;
                a.z /= length;
            }

            void Trans_World2Eye(_3dpoint w, _3dpoint e) // eye = camera...
            {
                //translate world -> camera comes to origin
                w.x -= from.x;
                w.y -= from.y;
                w.z -= from.z;

                // rotation // align uvn to the wcs
                e.x = w.x*u.x + w.y*u.y + w.z*u.z;//convert to world to eye coords using uvn vectors
                e.y = w.x*n.x + w.y*n.y + w.z*n.z;
                e.z = w.x*v.x + w.y*v.y + w.z*v.z;


            }

            bool Trans_ClipEye(_3dpoint e1, _3dpoint e2)
            {
                // check, both lies before z_near?
                if ((e1.y<=camera.z_near )&&(e2.y<=camera.z_near))
                    return (false);
                // both lies behinf z_far?
                if ((e1.y>=camera.z_far)&&(e2.y>=camera.z_far))
                    return (false);

                double mu;

                // one lies before z_near -> clip
                if  ((e1.y<camera.z_near)&&(e2.y>camera.z_far)||(e1.y>camera.z_near)&&(e2.y<camera.z_far))
                {
                    mu = (camera.z_near - e1.y)/(e2.y - e1.y);
                    if (e1.y<camera.z_near)
                    {
                        e1.x = e1.x + mu*(e2.x - e1.x);
                        e1.y = camera.z_near;
                        e1.z = e1.z + mu*(e2.z - e1.z);
                    }
                    if (e2.y<camera.z_near)
                    {
                        e2.x = e2.x + mu*(e2.x - e1.x);
                        e2.y = camera.z_near;
                        e2.z = e2.z + mu*(e2.z - e1.z);
                    }
                }

                // one lies after the z_far ->clip
                if ((e1.y>camera.z_far)&&(e2.y<camera.z_far)||(e1.y<camera.z_far)&&(e2.y>camera.z_far))
                {
                    mu = (camera.z_far - e1.y)/(e2.y-e1.y);
                    if (e1.y>camera.z_far)
                    {
                        e1.x = e1.x + mu*(e2.x - e1.x);
                        e1.y = camera.z_far;
                        e1.z = e1.z + mu*(e2.z - e1.z);
                    }
                    if (e2.y>camera.z_far)
                    {
                        e2.x = e2.x + mu*(e2.x - e1.x);
                        e2.y = camera.z_far;
                        e2.z = e2.z + mu*(e2.z - e1.z);
                    }
                }

                return (true);
            }

            void Trans_Eye2Norm(_3dpoint e, _3dpoint n)
            {
                double d;
                if (camera.projection = 0)
                {
                    d =  camera.zoom / e.y;
                    n.x = d * e.x / tan_angleh;
                    n.y = e.y;
                    n.z = d * e.z / tan_anglev;

                }
                else
                {
                    d = camera.zoom;
                    n.x = d * e.x / tan_angleh;
                    n.y = e.y;
                    n.z = d * e.z / tan_anglev;
                }
            }

            bool Trans_ClipNorm(_3dpoint n1, _3dpoint n2)
            {
                double mu;
                // check, is line beyond x = 1?
                if ((n1.x>=1)&&(n2.x>=1))
                    return (false);
                    //  beyond x = - 1?
                if ((n1.x<=-1)&&(n2.x<=-1))
                    return (false);
                // if cuts x = 1 ...
                if ((n1.x>1)&&(n2.x<1)||(n1.x<1)&&(n2.x>1))
                {
                    mu = (1-n1.x)/(n2.x-n1.x);
                    if (n1.x>1)
                    {
                        n1.x = 1;
                        n1.z = n1.z + mu*(n2.z - n1.z);
                    }
                    else // ie n2.x>1
                    {
                        n2.x = 1;
                        n2.z = n2.z + mu*(n2.z - n1.z);
                    }
                }
                // if cuts x = -1
                if ((n1.x<-1)&&(n2.x > -1) || (n1.x>-1)&&(n2.x<-1))
                {
                    mu = (-1 - n1.x) / (n2.x - n1.x);
                    if (n1.x > -1)
                    {
                        n2.z = n1.z + mu * (n2.z - n1.z);
                        n2.x = -1;
                    }
                    else
                    {
                        n1.z = n1.z + mu * (n2.z - n1.z);
                        n1.x = -1;
                    }
                }
                // now similar process for z coord
                // check, if beyond z = 1
                if ((n1.z>=1)&&(n2.z>=1))
                    return (false);
                    //  beyond z = - 1?
                if ((n1.z<=-1)&&(n2.z<=-1))
                    return (false);
                // if cuts z = 1 ...
                if ((n1.z>1)&&(n2.z<1)||(n1.z<1)&&(n2.z>1))
                {
                    mu = (1-n1.z)/(n2.z-n1.z);
                    if (n1.z>1)
                    {
                        n1.z = 1;
                        n1.x = n1.x + mu*(n2.x - n1.x);
                    }
                    else // ie n2.x>1
                    {
                        n2.z = 1;
                        n2.x = n2.x + mu*(n2.x - n1.x);
                    }
                }
                // if cuts z = -1
                if ((n1.z<-1)&&(n2.z > -1) || (n1.z>-1)&&(n2.z<-1))
                {
                    mu = (-1 - n1.z) / (n2.x - n1.z);
                    if (n1.z > -1)
                    {
                        n2.x = n1.x + mu * (n2.x - n1.x);
                        n2.z = -1;
                    }
                    else
                    {
                        n1.x = n1.x + mu * (n2.x - n1.x);
                        n1.z = -1;
                    }
                }

                return (true);

            } //end Trans_Clipnorm

            void Trans_Norm2Screen(_3dpoint norm,_2dpoint projected)
            {
                projected.h = (int)(screen.center.h - screen.size.h * norm.x/2);
                projected.v = (int)(screen.center.v - screen.size.v * norm.z/2);
            }// end Trans_Norm2Screen

            bool Trans_Line(_3dpoint w1, _3dpoint w2)
            {
                Trans_World2Eye(w1,e1);
                Trans_World2Eye(w2,e2);
                if (Trans_ClipEye(e1,e2))
                {
                    Trans_Eye2Norm(e1,n1);
                    Trans_Eye2Norm(e2,n2);
                    if (Trans_ClipNorm(n1,n2))
                    {
                        Trans_Norm2Screen(n1,p1);
                        Trans_Norm2Screen(n2,p2);
                        return (true);
                    }
                }

                return (true); // false

            }// end Trans_Line

    }; // end class Projection

//}  // end namespace _3dto2d

void Key(unsigned char c, int x, int y)
{
    switch (c){
    case 27:
        {
            exit(0);
            break;
        }
    }
}

void DrawLine()
{
    Projection proc;
    proc = Projection();

    glColor3ub(255,255,255);// black border along given vertices
    glBegin(GL_LINE_LOOP);
    glVertex2d(320, 20);
    glVertex2d(1120, 20);
    glVertex2d(320, 820);
    glVertex2d(1120, 820);
    glEnd();

    _3dpoint d1(n,z1,p);
    _3dpoint d2(p,z1,p);
    _3dpoint d3(p,z1,n);
    _3dpoint d4(n,z1,n); // 4 vertices for a face

// front face
    //d1 = _3dpoint(n,near,p);

    glColor3ub(255,0,0); // red
    glBegin(GL_LINES);

    proc.Trans_Line(d1,d2);
    glVertex2d(proc.p1.h, proc.p1.v);
    glVertex2d(proc.p2.h, proc.p2.v);
    proc.Trans_Line(d2,d3);
    glVertex2d(proc.p1.h, proc.p1.v);
    glVertex2d(proc.p2.h, proc.p2.v);
    proc.Trans_Line(d3,d4);
    glVertex2d(proc.p1.h, proc.p1.v);
    glVertex2d(proc.p2.h, proc.p2.v);
    proc.Trans_Line(d4,d1);
    glVertex2d(proc.p1.h, proc.p1.v);
    glVertex2d(proc.p2.h, proc.p2.v);

    glEnd();

    glFlush();
}


int main(int argc,char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(800,650);

    init();
    glutCreateWindow("3dTO2d");
    glutKeyboardFunc(Key);
    glutDisplayFunc(DrawLine);

    glutMainLoop();

    return 0;
}


// lyaang level = extreme
