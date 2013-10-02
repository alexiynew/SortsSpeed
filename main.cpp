#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <cstring>
#include<time.h>
#include <queue>


const int WIDTH  = 640;
const int HEIGHT = 480;
const int p_left = 5;
const int p_bottom = 20;

const int N = 1000;
const int maxY = 10000;

const float stepX = N/(WIDTH-p_left*1.0f);
const float stepY = maxY/(WIDTH-p_bottom*1.0f);

int cmp_count = 0;

std::vector<int> rplcSort;
std::vector<int> mrgSort;
std::vector<int> mrgqueueSort;
std::vector<int> hpSort;
std::vector<int> qSort;

void print(std::string text, int x, int y){
    glRasterPos2f(x, y);
    unsigned char s[text.size()];
    strncpy((char*)s, text.c_str(), sizeof(s));
    s[text.size()] = 0;
    glutBitmapString(GLUT_BITMAP_HELVETICA_10, s);
}

void drawAxis(){
    glColor3f(0,0,0);

    glPointSize(3);
	glBegin(GL_POINTS);
    glVertex2f(p_left, HEIGHT-p_bottom);
	glEnd();
    glPointSize(1);

	glBegin(GL_LINES);
	glVertex2f(p_left,HEIGHT-p_bottom);
	glVertex2f(WIDTH,HEIGHT-p_bottom);
    glVertex2f(p_left,HEIGHT-p_bottom);
	glVertex2f(p_left,0);
	glEnd();

	print(std::to_string(N),WIDTH-30,HEIGHT-p_bottom+11);
	print(std::to_string(maxY),p_left+2,10);
	print("0",p_left+2,HEIGHT-p_bottom+11);
}

struct Color{
    float r,g,b,a;
};

void drawFoo(int (*foo)(float), Color color, std::string name){
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_LINE_STRIP);
    glVertex2f(p_left,HEIGHT-p_bottom);
    int x=1;
    int y;
    while(x < WIDTH-p_left){
        y = foo(x*stepX)/stepY;
        if(y>HEIGHT) break;
        glVertex2f(x+p_left,HEIGHT-p_bottom-y);
        x++;
    }
    glEnd();

    glColor4f(0,0,0,1);
    x+=(x+p_left+50>WIDTH) ? p_left-50 : p_left;
    y = (HEIGHT-p_bottom-y<0) ? HEIGHT-p_bottom-y+50 : HEIGHT-p_bottom-y;
    print(name, x, y);

};

void drawSort(std::vector<int>& graph, Color color, std::string name){
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_LINE_STRIP);
    int x=1;
    int y;
    while(x < WIDTH-p_left){
        if(x>(int)graph.size()) break;
        y = graph[x]/stepY;
        if(y>HEIGHT) break;
        glVertex2f(x+p_left,HEIGHT-p_bottom-y);
        x++;
    }
    glEnd();
    glColor4f(0,0,0,1);
    //x+=(x+p_left>WIDTH) ? p_left : p_left;


    while(y-30 < (graph[x--]/stepY));
     y = HEIGHT-p_bottom-y+70;

    print(name, x+p_left, y);
}

int compare(int v1, int v2){
    cmp_count++;
    return v1 - v2;
};

void replaceSort(std::vector<int>& a, int l, int r){
    while(l<r){
        int mi = l;
        for(int i=l; i<r; i++){
            if(compare(a[mi], a[i]) > 0){
                mi = i;
            }
        }

        if(mi!=l){
            int tmp = a[mi];
            a[mi] = a[l];
            a[l]=tmp;
        }
        l++;
    }
}

std::vector<int> mergeSort(std::vector<int>& a, int l, int r){
    std::vector<int> tmp;
    if(l == r) {
        tmp.push_back(a[l]);
        return tmp;
    }
    std::vector<int> left = mergeSort(a, l, l+(r-l)/2);
    std::vector<int> right = mergeSort(a, l + (r-l)/2 + 1, r);
    unsigned int i=0,j=0;
    while(i<left.size()){
        int res = j<right.size() ? compare(left[i],right[j]) : -1;
        if(res>=0){
            tmp.push_back(right[j]); j++;
        } else if(res <= 0) {
            tmp.push_back(left[i]); i++;
        }
    }
    while(j<right.size()){
        tmp.push_back(right[j]); j++;
    }

    return tmp;
}

std::vector<int> mergeQueueSort(std::vector<int>& a, int l, int r){
    std::queue<std::vector<int>> q;
    for(int i=l; i<=r; i++)
        q.push({a[i]});
    while(q.size() > 1){
        std::vector<int> left = q.front(); q.pop();
        std::vector<int> right = q.front(); q.pop();
        std::vector<int> tmp;
        unsigned int i=0,j=0;
        while(i<left.size()){
            int res = j<right.size() ? compare(left[i],right[j]) : -1;
            if(res>=0){
                tmp.push_back(right[j]); j++;
            } else if(res <= 0) {
                tmp.push_back(left[i]); i++;
            }
        }
        while(j<right.size()){
            tmp.push_back(right[j]); j++;
        }
        q.push(tmp);
    }
    return q.front();
}

void heapifay(std::vector<int>& a, int i, int size){
    const int l = i*2+1;
    const int r = l+1;
    int m = i;
    m = (l < size && (compare(a[l], a[i]) > 0)) ? l : i;
    m = (r < size && (compare(a[r], a[m]) > 0)) ? r : m;
    if(m!=i){
        int tmp = a[i];
        a[i]=a[m];
        a[m]=tmp;
        heapifay(a, m, size);
    }

}

void heapSort(std::vector<int>& a, int l, int r){
    int size = r-l+1;
    for(int i=r-(size/2); i>=l;i--){
        heapifay(a,i,size);
    }
    while(--size && l!=r){
        int tmp = a[l];
        a[l] = a[size];
        a[size] = tmp;
        heapifay(a,l,size);
    }
}

void quickSort(std::vector<int>& a, int l, int r){
    if(l == r) return;
    while(l<r){
        int size = r-l+1;
        int b = size/2+l;
        int i=l, j=r;
        while(i<=j){
            while(i!=b && compare(a[b], a[i]) > 0)i++;
            while(j!=b && compare(a[j], a[b]) > 0)j--;
            if(i<=j){
                int tmp = a[j];
                a[j] = a[i];
                a[i] = tmp;
                if(i==b)
                    b = j;
                else if(j==b)
                    b = i;
                i++; j--;
            }
        }
        if(b-1>l)quickSort(a, l, b-1);
        l=b+1;
    }
}

std::vector<int> genArrey(int size){
    std::vector<int> a;
    for(int i=0; i<size; i++)
        a.push_back(rand()%10000000);
    return a;
};



void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawAxis();

    glLineWidth(2);

    drawFoo([](float n)->int{return n*log(n);}, {0.8,0.5,0,1}, "NlogN");
    drawFoo([](float n)->int{return n*log10(n);}, {0.2,0.5,0.9,1}, "Nlog10N");
    drawFoo([](float n)->int{return n*n;}, {1,0,0,1}, "Npow2");
    drawFoo([](float n)->int{return n;}, {0,1,0,1}, "linarN ");

    drawSort(rplcSort, {0,1,1,1}, "replace");
    drawSort(mrgSort, {0,0,1,1}, "megre");
    drawSort(mrgqueueSort, {1,0,1,1},"");
    drawSort(hpSort, {1,0.6,0,1},"heap");
    drawSort(qSort, {0.3,0.6,0.5,1},"quick");
    glLineWidth(1);


	glutSwapBuffers();
}

void timer(int = 0){
    display();
    glutTimerFunc(1, timer, 0);
}

int main(int argc, char **argv) {
    srand(time(0));

    for(int x = 0; x < WIDTH-p_left; x++){
        int n = x*stepX;
        if(n >= N) n = N-1;
        std::vector<int> tmp = genArrey(n+1);

        cmp_count = 0;
        quickSort(tmp, 0, n);
        qSort.push_back(cmp_count);

        cmp_count = 0;
        tmp = genArrey(n+1);
        replaceSort(tmp, 0, n+1);
        rplcSort.push_back(cmp_count);

        cmp_count = 0;
        tmp = genArrey(n+1);
        tmp = mergeSort(tmp, 0, n);
        mrgSort.push_back(cmp_count);

        cmp_count = 0;
        tmp = genArrey(n+1);
        tmp = mergeQueueSort(tmp, 0, n);
        mrgqueueSort.push_back(cmp_count);

        cmp_count = 0;
        tmp = genArrey(n+1);
        heapSort(tmp, 0, n);
        hpSort.push_back(cmp_count);

    }

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Sorts");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glClearColor(1,1,1,1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,WIDTH,HEIGHT,0,-1,1);

	glutDisplayFunc(display);
	timer();

	glutMainLoop();

	return 0;
}
