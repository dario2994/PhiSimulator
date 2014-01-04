#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <vector>
using namespace std;

double G, dt;
long long int ImageCapturingFrequency, Duration;

const int width = 256;
const int height = 256;

const double Z=1.2;
const double minX=-Z, maxX=Z, minY=-Z, maxY=Z;
//dò per scontato che i rapporti siano uguali
const double ratio=(maxX-minX)/(double)width; 

vector < vector <char> > raw;

void printPixel(int x, int y, bool remove){
	if ( 0<=x and x<width and 0<=y and y<height) {
		if( !remove )	raw[y][x>>3] |=	 ( 1 << (7-(x%8)) );
		else 			raw[y][x>>3] &= !( 1 << (7-(x%8)) );
	}
}

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

class body{
	public:
		bool fixed;
		double radius;
		double mass;
		vec s,v;
		
		void applyForce(vec F) {
			if(!fixed) v=v+(F*(dt/mass));
		}
		
		void gravity(body A){
			vec r=A.s-s;
			double d=r.norm();
			applyForce( r*((G*mass*A.mass)/(d*d*d)) );
		}
		
		void move() {
			s=s+(v*dt);
		}
};


vector <body> sistema;

void printBody(body &A, bool remove) {
	int x=(A.s.x-minX)/ratio;
	int y=(A.s.y-minY)/ratio;
	int r=A.radius/ratio;
	for(int i=-r;i<=r;i++)for(int j=-r;j<=r;j++) {
		if( i*i+j*j<=r*r ) printPixel(x+i,y+j,remove);
	}
}

void saveImage() {
	static long long int imageNum=0;
	
	for(int j=0;j<(int)sistema.size();j++) printBody( sistema[j], 0 );
	
	stringstream numero;
	numero << "00000" << imageNum;
	string ntr=numero.str();
	string nome = "images/"+ ntr.substr(ntr.size()-5,5) +".pbm";
	ofstream image(nome.c_str());
	
	image << "P4\n" << width << " " << height<< "\n";
	
	for(int i=0;i<height;i++) for(int j=0;j<width/8;j++) image.put( raw[i][j] );
	
	image.close();
	
	for(int j=0;j<(int)sistema.size();j++) printBody( sistema[j], 1 );
	
	imageNum++;
}

void init() {
	ifstream inB("input/bodies.txt");
	int l;
	inB >> l;
	for(int i=0;i<l;i++) {
		body N;
		inB >> N.mass >> N.radius >> N.fixed;
		inB >> N.s.x >> N.s.y;
		inB >> N.v.x >> N.v.y;
		sistema.push_back(N);
	}
	
	inB.close();
	
	ifstream inE("input/environment.txt");
	inE >> G;
	inE >> dt;
	inE >> ImageCapturingFrequency >> Duration;
	
	cout << G << " " << dt << " " << ImageCapturingFrequency << " " << Duration << "\n";
	
	inE.close();
	
	raw.resize(height);
	for(int i=0;i<height;i++) raw[i].resize(width/8);
}


int main() {
	
	init();
	
	double ratio2=(maxY-minY)/(double) height;
	assert( fabs( (ratio-ratio2)/ratio ) < 0.1 );
	assert ( width%8 == 0 );
	
	
	int l=sistema.size();
	
	for(long long int countSim=0;countSim<Duration*ImageCapturingFrequency*24;countSim++) {
		for(int i=0;i<l;i++) for(int j=0;j<l;j++){
			if(i==j) continue;
			sistema[i].gravity(sistema[j]);
		}
		for(int j=0;j<l;j++) sistema[j].move();
		if( countSim%ImageCapturingFrequency==0 ) {
			saveImage();
		}
	}
	
	cout << "Simulation finished" << endl;
	
	system( "yes | ./ImagesToVideo.sh" );
	system( "vlc --play-and-exit orbita.mp4" );
}