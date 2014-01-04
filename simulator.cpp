#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <vector>
using namespace std;

const double dt=1e-5;
const double G=0.5;
const int imageSparsity=(1e5)/24;
const int totalTime = imageSparsity*24*7;

const int width = 256;
const int height = 256;

const double Z=1.2;
const double minX=-Z, maxX=Z, minY=-Z, maxY=Z;
//dò per scontato che i rapporti siano uguali
const double ratio=(maxX-minX)/(double)width; 

char raw[height][(width/8)];

struct vec{
	double x,y;
	vec(){x=y=0;}
	vec(double xx, double yy){
		x=xx, y=yy;
	}
	
	vec operator *(double scalar) {
		return vec(x*scalar,y*scalar);
	}
	
	vec operator +(vec V){
		return vec(x+V.x,y+V.y);
	}
	
	vec operator -(vec V){
		return vec(x-V.x,y-V.y);
	}
	
	double norm(){
		return sqrt(x*x+y*y);
	}
};

void printPixel(int x, int y, bool remove){
	if ( 0<=x and x<width and 0<=y and y<height) {
		if( !remove )	raw[y][x>>3] |=	 ( 1 << (7-(x%8)) );
		else 			raw[y][x>>3] &= !( 1 << (7-(x%8)) );
	}
}

void printCircle(vec s, double radius, bool remove) {
	int x=(s.x-minX)/ratio;
	int y=(s.y-minY)/ratio;
	int r=radius/ratio;
	for(int i=-r;i<=r;i++)for(int j=-r;j<=r;j++) {
		if( i*i+j*j<=r*r ) printPixel(x+i,y+j,remove);
	}
}

class corpo{
	public:
		bool fixed;
		double radius;
		double mass;
		vec s,v;
		
		void print() {
			printCircle(s,radius,0);
		}
		
		void remove() {
			printCircle(s,radius,1);
		}
		
		void applyForce(vec F) {
			if(!fixed) v=v+(F*(dt/mass));
		}
		
		void gravity(corpo A){
			vec r=A.s-s;
			double d=r.norm();
			applyForce( r*((G*mass*A.mass)/(d*d*d)) );
		}
		
		void move() {
			s=s+(v*dt);
		}
};

vector <corpo> sistema;

void saveImage() {
	static int imageNum=0;
	
	
	for(int j=0;j<(int)sistema.size();j++) sistema[j].print();
	
	stringstream numero;
	numero << "00000" << imageNum;
	string ntr=numero.str();
	string nome = "images/"+ ntr.substr(ntr.size()-5,5) +".pbm";
	ofstream image(nome.c_str());
	
	image << "P4\n" << width << " " << height<< "\n";
	
	for(int i=0;i<height;i++) for(int j=0;j<width/8;j++) image.put( raw[i][j] );
	
	image.close();
	
	for(int j=0;j<(int)sistema.size();j++) sistema[j].remove();
	
	imageNum++;
}

void init() {
	ifstream in("input.txt");
	int l;
	in >> l;
	for(int i=0;i<l;i++) {
		corpo N;
		in >> N.mass >> N.radius >> N.fixed;
		in >> N.s.x >> N.s.y;
		in >> N.v.x >> N.v.y;
		sistema.push_back(N);
	}
}


int main() {
	double ratio2=(maxY-minY)/(double) height;
	assert( fabs( (ratio-ratio2)/ratio ) < 0.1 );
	assert ( width%8 == 0 );
	
	init();
	
	
	int l=sistema.size();
	
	for(int countSim=0;countSim<totalTime;countSim++) {
		for(int i=0;i<l;i++) for(int j=0;j<l;j++){
			if(i==j) continue;
			sistema[i].gravity(sistema[j]);
		}
		for(int j=0;j<l;j++) sistema[j].move();
		if( countSim%imageSparsity==0 ) {
			saveImage();
		}
	}
	
	cout << "Simulation finished" << endl;
	
	system( "yes | ./ImagesToVideo.sh" );
	system( "vlc --play-and-exit orbita.mp4" );
}