
#include "math.h"
#include "d3dUtility.h"
#include <iostream>


//
// Globals
//

bool collision(float x[],float y[],float z[],int n,float px,float py,float pz);
float downcollision(float x[],float y[],float z[],int n,float px,float py,float pz);
float upcollision(float x[],float y[],float z[] ,int n,float px,float py,float pz);
int coincoll(float x[],float y[],float z[] ,int n,float px,float py,float pz);

IDirect3DDevice9* Device = 0; 

const int Width  = 1280;
const int Height = 720;
float cx, cy, cz ,czud = 0 ;
IDirect3DTexture9*      Tex  = 0;
IDirect3DTexture9*      Tex2  = 0;
IDirect3DTexture9*      Tex3  = 0;
IDirect3DTexture9*      wallTex1  = 0;
IDirect3DTexture9*      wallTex2  = 0;
IDirect3DTexture9*      TexC[12]  = {0};
IDirect3DTexture9*      TexP_U[4]  = {0};
IDirect3DTexture9*      TexP_D[4]  = {0};
IDirect3DTexture9*      TexP_L[4]  = {0};
IDirect3DTexture9*      TexP_R[4]  = {0};
IDirect3DTexture9*      TexP_JL  = 0;
IDirect3DTexture9*      TexP_JR  = 0;
IDirect3DTexture9*      Tex_BG = 0 ;
IDirect3DTexture9*      Tex_Floor = 0 ;
IDirect3DTexture9*      Tex_Coin[4] = {0} ;
IDirect3DTexture9*      Tex_Num[26] = {0} ;
IDirect3DTexture9*      Tex_ResultOK = 0 ;
IDirect3DTexture9*      Tex_ResultBAD = 0 ;

ID3DXMesh* Text = 0;

float wiggle_angle = 0;
float wiggle_y = 0;

int jump = 0;
float s = 0;
float stop =0;

float objX[10] = {0} ;
float objY[10] = {0} ;
float objZ[10] = {0} ;

int TexC_i = 0;
float TexC_time = 0;
int TexCoin_i = 0;
float TexCoin_time = 0;
int TexNum_i = 0;
float TexNum_time = 0;

float Wall_x = 0.5f;
float Wall_y = 0.5f;
float Wall_z = 0.5f;
float Plane_x = 2.0f ;
float Plane_y = 0.5f ;
float Plane_z = 0.0f ;

float Player_x = 0.0f ;
float Player_y = 0.5f ;
float Player_z = -0.2f ;
float Player_w = 0.5f ;
float Player_h = 0.5f ;

float coin_w = 0.5f ;
float coin_h = 0.5f ;

float coinX[10] = {0};
float coinY[10] = {0};
float coinZ[10] = {0};
int coinLive[10] = {0};

float TextTime_w = 0.5f ;
float TextTime_h = 0.5f ;

int PDir = 2 ; //U1 D2 L3 R4
bool PAni = false ;
bool PJump = false ;

IDirect3DVertexBuffer9* Pyramid = 0;
IDirect3DVertexBuffer9* Pyramid2 = 0;
IDirect3DVertexBuffer9* Pyramid3 = 0;
IDirect3DVertexBuffer9* Pyramid4 = 0;
IDirect3DVertexBuffer9* Wall = 0;
IDirect3DVertexBuffer9* Plane = 0;
IDirect3DVertexBuffer9* Player = 0;
IDirect3DVertexBuffer9* Coin = 0;
IDirect3DVertexBuffer9* TextTime = 0;
IDirect3DVertexBuffer9* Result = 0;

D3DMATERIAL9 mtrl,mtrl2,mtrl3;

//
// Classes and Structures
//
struct Vertex
{
	Vertex(){}

	Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
	{
		_x  = x;  _y  = y;	_z  = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u  = u;  _v  = v;
	}
	float  _x,  _y,  _z;
	float _nx, _ny, _nz;
    float _u, _v; // texture coordinates

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

//
// Framework Functions
//
bool Setup()
{
//----- Font START ---------------------------------------------------------
	//
	// Get a handle to a device context.
	//
	HDC hdc = CreateCompatibleDC( 0 );
    HFONT hFont;
    HFONT hFontOld;

	//
	// Describe the font we want.
	//

    LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));

	lf.lfHeight         = 25;    // in logical units
	lf.lfWidth          = 12;    // in logical units
	lf.lfEscapement     = 0;        
	lf.lfOrientation    = 0;     
	lf.lfWeight         = 500;   // boldness, range 0(light) - 1000(bold)
	lf.lfItalic         = false;   
	lf.lfUnderline      = false;    
	lf.lfStrikeOut      = false;    
	lf.lfCharSet        = DEFAULT_CHARSET;
	lf.lfOutPrecision   = 0;              
	lf.lfClipPrecision  = 0;          
	lf.lfQuality        = 0;           
	lf.lfPitchAndFamily = 0;    
	strcpy(lf.lfFaceName, "Times New Roman"); // font style

	//
	// Create the font and select it with the device context.
	//
    hFont = CreateFontIndirect(&lf);
    hFontOld = (HFONT)SelectObject(hdc, hFont); 

	//
	// Create the text mesh based on the selected font in the HDC.
	//
    D3DXCreateText(Device, hdc, "Direct3D", 
        0.001f, 0.4f, &Text, 0, 0);

	//
	// Restore the old font and free the acquired HDC.
	//
    SelectObject(hdc, hFontOld);
    DeleteObject( hFont );
    DeleteDC( hdc );
//----- Font END ---------------------------------------------------------

	//
	// Turn on lighting.
	//
	Device->SetRenderState(D3DRS_LIGHTING, true);

	//
	// Create the vertex buffer for the pyramid.
	//

	Device->CreateVertexBuffer(
		36 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Pyramid,
		0);
	
	Device->CreateVertexBuffer(
		36 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Pyramid2,
		0);
	 
	Device->CreateVertexBuffer(
		36 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Pyramid3,
		0);
	
	Device->CreateVertexBuffer(
		36 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Pyramid4,
		0);
	 
	Device->CreateVertexBuffer(
		36 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Wall,
		0);
	
	Device->CreateVertexBuffer(
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Plane,
		0);

	Device->CreateVertexBuffer(
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Coin,
		0);
	 
	Device->CreateVertexBuffer(
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Player,
		0);
	Device->CreateVertexBuffer(
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&TextTime,
		0);
	Device->CreateVertexBuffer(
		6 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Result,
		0);
	//
	// Fill the vertex buffer 
	//

	//-------------------ROOM-------------------//
	Vertex* v;
	Pyramid->Lock(0, 0, (void**)&v, 0);
	// front face
	v[0] = Vertex(-6.0f, -6.0f, 6.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex( -6.0f, 6.0f,  6.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex( 6.0f, 6.0f, 6.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = Vertex( -6.0f, -6.0f,  6.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex( 6.0f, 6.0f,  6.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex( 6.0f, -6.0f, 6.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	Pyramid->Unlock();
	Pyramid2->Lock(0, 0, (void**)&v, 0);
	// right face
	v[6] = Vertex( 6.0f, -6.0f, 6.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[7] = Vertex( 6.0f, 6.0f,  6.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[8] = Vertex( 6.0f, 6.0f,  -6.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[9] = Vertex( 6.0f, -6.0f, 6.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[10] = Vertex( 6.0f, 6.0f,  -6.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex( 6.0f, -6.0f,  -6.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	// back face
	v[12]  = Vertex( 6.0f, -6.0f,  -6.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[13] = Vertex( 6.0f, 6.0f,  -6.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[14] = Vertex(-6.0f, -6.0f,  -6.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[15]  = Vertex( -6.0f, -6.0f,  -6.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[16] = Vertex( 6.0f, 6.0f,  -6.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[17] = Vertex(-6.0f, 6.0f,  -6.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	// left face
	v[18] = Vertex(-6.0f, -6.0f,  6.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[19] = Vertex( -6.0f, -6.0f,  -6.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[20] = Vertex(-6.0f, 6.0f, -6.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[21] = Vertex(-6.0f, 6.0f,  -6.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( -6.0f, 6.0f,  6.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex(-6.0f, -6.0f, 6.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	Pyramid2->Unlock();
	Pyramid3->Lock(0, 0, (void**)&v, 0);
	// down face
	v[24] = Vertex( 6.0f, -6.0f,  -6.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[25] = Vertex( -6.0f, -6.0f,  6.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[26] = Vertex( 6.0f, -6.0f,  6.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[27] = Vertex( -6.0f, -6.0f,   6.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[28] = Vertex(  6.0f, -6.0f,  -6.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[29] = Vertex(-6.0f, -6.0f, -6.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	Pyramid3->Unlock();
	Pyramid4->Lock(0, 0, (void**)&v, 0);
	// up face
	v[30] = Vertex( 6.0f, 6.0f,  6.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[31] = Vertex( -6.0f, 6.0f,  6.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[32] = Vertex( 6.0f, 6.0f,  -6.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[33] = Vertex( 6.0f, 6.0f,   -6.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[34] = Vertex( -6.0f, 6.0f,  6.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[35] = Vertex( -6.0f, 6.0f, -6.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	Pyramid4->Unlock();

	
	//--------------Plane------------------//
	Vertex* vp;
	Plane->Lock(0, 0, (void**)&vp, 0);
	// back face
	vp[0]  = Vertex( Plane_x, -Plane_y,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vp[1] = Vertex(-Plane_x, -Plane_y,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vp[2] = Vertex( Plane_x, Plane_y,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vp[3]  = Vertex( -Plane_x, -Plane_y,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vp[4] = Vertex(-Plane_x, Plane_y,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vp[5] = Vertex( Plane_x, Plane_y,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	Plane->Unlock();

	
	//--------------TextTime------------------//
	Vertex* vtti;
	TextTime->Lock(0, 0, (void**)&vtti, 0);
	vtti[0]  = Vertex( TextTime_w, -TextTime_h,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vtti[1] = Vertex(-TextTime_w, -TextTime_h,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vtti[2] = Vertex( TextTime_w, TextTime_h,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vtti[3]  = Vertex( -TextTime_w, -TextTime_h,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vtti[4] = Vertex(-TextTime_w, TextTime_h,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vtti[5] = Vertex( TextTime_w, TextTime_h,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	TextTime->Unlock();

	//--------------Result------------------//
	Vertex* vrs;
	Result->Lock(0, 0, (void**)&vrs, 0);
	vrs[0]  = Vertex( TextTime_w*3, -TextTime_h*3,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vrs[1] = Vertex(-TextTime_w*3, -TextTime_h*3,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vrs[2] = Vertex( TextTime_w*3, TextTime_h*3,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vrs[3]  = Vertex( -TextTime_w*3, -TextTime_h*3,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vrs[4] = Vertex(-TextTime_w*3, TextTime_h*3,  -Plane_z, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vrs[5] = Vertex( TextTime_w*3, TextTime_h*3,  -Plane_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	Result->Unlock();
	
	//--------------Coin------------------//
	Vertex* vc;
	Coin->Lock(0, 0, (void**)&vc, 0);
	vc[0]  = Vertex( coin_w, -coin_h,  -0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vc[1] = Vertex(-coin_w, -coin_h,  -0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vc[2] = Vertex( coin_w, coin_h,  -0, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vc[3]  = Vertex( -coin_w, -coin_h,  -0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vc[4] = Vertex(-coin_w, coin_h,  -0, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vc[5] = Vertex( coin_w, coin_h,  -0, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	Coin->Unlock();
	
	//--------------Player------------------//
	Vertex* vpr;
	Player->Lock(0, 0, (void**)&vpr, 0);
	// back face
	vpr[0]  = Vertex( Player_w, -Player_h,  -0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vpr[1] = Vertex(-Player_w, -Player_h,  -0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vpr[2] = Vertex( Player_w, Player_h,  -0, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vpr[3]  = Vertex( -Player_w, -Player_h,  -0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vpr[4] = Vertex(-Player_w, Player_h,  -0, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vpr[5] = Vertex( Player_w, Player_h,  -0, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	Player->Unlock();



	//--------------Wall------------------//
	Vertex* vv;
	Wall->Lock(0, 0, (void**)&vv, 0);
	// front face
	vv[0] = Vertex(-Wall_x, -Wall_y, Wall_z, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	vv[1] = Vertex( Wall_x, Wall_y, Wall_z, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	vv[2] = Vertex( -Wall_x, Wall_y,  Wall_z, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vv[3] = Vertex( -Wall_x, -Wall_y,  Wall_z, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	vv[4] = Vertex( Wall_x, -Wall_y, Wall_z, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	vv[5] = Vertex( Wall_x, Wall_y,  Wall_z, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	// right face
	vv[6] = Vertex( Wall_x, -Wall_y, Wall_z, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vv[7] = Vertex( Wall_x, Wall_y,  -Wall_z, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vv[8] = Vertex( Wall_x, Wall_y,  Wall_z, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vv[9] = Vertex( Wall_x, -Wall_y, Wall_z, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vv[10] = Vertex( Wall_x, -Wall_y,  -Wall_z, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vv[11] = Vertex( Wall_x, Wall_y,  -Wall_z, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// back face
	vv[12]  = Vertex( Wall_x, -Wall_y,  -Wall_z, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vv[13] = Vertex(-Wall_x, -Wall_y,  -Wall_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vv[14] = Vertex( Wall_x, Wall_y,  -Wall_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vv[15]  = Vertex( -Wall_x, -Wall_y,  -Wall_z, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vv[16] = Vertex(-Wall_x, Wall_y,  -Wall_z, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vv[17] = Vertex( Wall_x, Wall_y,  -Wall_z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	// left face
	vv[18] = Vertex(-Wall_x, -Wall_y,  Wall_z, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vv[19] = Vertex(-Wall_x, Wall_y, -Wall_z, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vv[20] = Vertex( -Wall_x, -Wall_y,  -Wall_z, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vv[21] = Vertex(-Wall_x, Wall_y,  -Wall_z, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vv[22] = Vertex(-Wall_x, -Wall_y, Wall_z, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vv[23] = Vertex( -Wall_x, Wall_y,  Wall_z, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// down face
	vv[24] = Vertex( Wall_x, -Wall_y,  -Wall_z, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	vv[25] = Vertex( Wall_x, -Wall_y,  Wall_z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	vv[26] = Vertex( -Wall_x, -Wall_y,  Wall_z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	vv[27] = Vertex( -Wall_x, -Wall_y,   Wall_z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	vv[28] = Vertex(-Wall_x, -Wall_y, -Wall_z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vv[29] = Vertex(  Wall_x, -Wall_y,  -Wall_z, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	// up face
	vv[30] = Vertex( Wall_x, Wall_y,  Wall_z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	vv[31] = Vertex( Wall_x, Wall_y,  -Wall_z, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	vv[32] = Vertex( -Wall_x, Wall_y,  Wall_z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	vv[33] = Vertex( Wall_x, Wall_y,   -Wall_z, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	vv[34] = Vertex( -Wall_x, Wall_y, -Wall_z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vv[35] = Vertex( -Wall_x, Wall_y,  Wall_z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	Wall->Unlock();

	//
	// Create the texture and set filters.
	//
	
	D3DXCreateTextureFromFile(Device,"img/u0.png",&TexP_U[0]);
	D3DXCreateTextureFromFile(Device,"img/u1.png",&TexP_U[1]);
	D3DXCreateTextureFromFile(Device,"img/u2.png",&TexP_U[2]);
	D3DXCreateTextureFromFile(Device,"img/u3.png",&TexP_U[3]);
	D3DXCreateTextureFromFile(Device,"img/d0.png",&TexP_D[0]);
	D3DXCreateTextureFromFile(Device,"img/d1.png",&TexP_D[1]);
	D3DXCreateTextureFromFile(Device,"img/d2.png",&TexP_D[2]);
	D3DXCreateTextureFromFile(Device,"img/d3.png",&TexP_D[3]);
	D3DXCreateTextureFromFile(Device,"img/l0.png",&TexP_L[0]);
	D3DXCreateTextureFromFile(Device,"img/l1.png",&TexP_L[1]);
	D3DXCreateTextureFromFile(Device,"img/l2.png",&TexP_L[2]);
	D3DXCreateTextureFromFile(Device,"img/l3.png",&TexP_L[3]);
	D3DXCreateTextureFromFile(Device,"img/r0.png",&TexP_R[0]);
	D3DXCreateTextureFromFile(Device,"img/r1.png",&TexP_R[1]);
	D3DXCreateTextureFromFile(Device,"img/r2.png",&TexP_R[2]);
	D3DXCreateTextureFromFile(Device,"img/r3.png",&TexP_R[3]);

	D3DXCreateTextureFromFile(Device,"img/jl.png",&TexP_JL);
	D3DXCreateTextureFromFile(Device,"img/jr.png",&TexP_JR);
	
	D3DXCreateTextureFromFile(Device,"img/coin0.png",&Tex_Coin[0]);
	D3DXCreateTextureFromFile(Device,"img/coin1.png",&Tex_Coin[1]);
	D3DXCreateTextureFromFile(Device,"img/coin2.png",&Tex_Coin[2]);
	D3DXCreateTextureFromFile(Device,"img/coin3.png",&Tex_Coin[3]);
	
	D3DXCreateTextureFromFile(Device,"img/n/n00.png",&Tex_Num[25]);
	D3DXCreateTextureFromFile(Device,"img/n/n01.png",&Tex_Num[24]);
	D3DXCreateTextureFromFile(Device,"img/n/n02.png",&Tex_Num[23]);
	D3DXCreateTextureFromFile(Device,"img/n/n03.png",&Tex_Num[22]);
	D3DXCreateTextureFromFile(Device,"img/n/n04.png",&Tex_Num[21]);
	D3DXCreateTextureFromFile(Device,"img/n/n05.png",&Tex_Num[20]);
	D3DXCreateTextureFromFile(Device,"img/n/n06.png",&Tex_Num[19]);
	D3DXCreateTextureFromFile(Device,"img/n/n07.png",&Tex_Num[18]);
	D3DXCreateTextureFromFile(Device,"img/n/n08.png",&Tex_Num[17]);
	D3DXCreateTextureFromFile(Device,"img/n/n09.png",&Tex_Num[16]);
	D3DXCreateTextureFromFile(Device,"img/n/n10.png",&Tex_Num[15]);
	D3DXCreateTextureFromFile(Device,"img/n/n11.png",&Tex_Num[14]);
	D3DXCreateTextureFromFile(Device,"img/n/n12.png",&Tex_Num[13]);
	D3DXCreateTextureFromFile(Device,"img/n/n13.png",&Tex_Num[12]);
	D3DXCreateTextureFromFile(Device,"img/n/n14.png",&Tex_Num[11]);
	D3DXCreateTextureFromFile(Device,"img/n/n15.png",&Tex_Num[10]);
	D3DXCreateTextureFromFile(Device,"img/n/n16.png",&Tex_Num[9]);
	D3DXCreateTextureFromFile(Device,"img/n/n17.png",&Tex_Num[8]);
	D3DXCreateTextureFromFile(Device,"img/n/n18.png",&Tex_Num[7]);
	D3DXCreateTextureFromFile(Device,"img/n/n19.png",&Tex_Num[6]);
	D3DXCreateTextureFromFile(Device,"img/n/n20.png",&Tex_Num[5]);
	D3DXCreateTextureFromFile(Device,"img/n/n21.png",&Tex_Num[4]);
	D3DXCreateTextureFromFile(Device,"img/n/n22.png",&Tex_Num[3]);
	D3DXCreateTextureFromFile(Device,"img/n/n23.png",&Tex_Num[2]);
	D3DXCreateTextureFromFile(Device,"img/n/n24.png",&Tex_Num[1]);
	D3DXCreateTextureFromFile(Device,"img/n/n25.png",&Tex_Num[0]);
	
	D3DXCreateTextureFromFile(Device,"img/ok.png",&Tex_ResultOK);
	D3DXCreateTextureFromFile(Device,"img/failed.png",&Tex_ResultBAD);

	//
	// Create the texture and set filters.
	//
	D3DXCreateTextureFromFile(Device,"img/bg.png",&Tex_BG);
	D3DXCreateTextureFromFile(Device,"img/floor_edited.png",&Tex_Floor);
	D3DXCreateTextureFromFile(Device,"sky.png",&Tex);
	D3DXCreateTextureFromFile(Device,"img/grass.png",&Tex2);	
	D3DXCreateTextureFromFile(Device,"cat3.jpg",&Tex3);
	D3DXCreateTextureFromFile(Device,"img/box0.png",&wallTex1);
	D3DXCreateTextureFromFile(Device,"img/box1.png",&wallTex2);

	// SetSamplerState
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// take alpha from alpha channel
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// set blending factors so that alpha component determines transparency
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	//
	// Create and set the material.
	//

	//D3DMATERIAL9 mtrl;
	mtrl.Ambient  = d3d::WHITE;
	mtrl.Diffuse  = d3d::WHITE;
	mtrl.Specular = d3d::WHITE;
	mtrl.Emissive = d3d::WHITE*0.2f;
	mtrl.Power    = 2.0f;

	//Device->SetMaterial(&mtrl);

	//D3DMATERIAL9 mtrl2;
	mtrl2.Ambient  = d3d::WHITE;
	mtrl2.Diffuse.a = 1.0f;
	mtrl2.Diffuse.r = 1.0f;
	mtrl2.Diffuse.g = 0.5f;
	mtrl2.Diffuse.b = 0.5f;
	mtrl2.Specular = d3d::WHITE;
	mtrl2.Emissive = d3d::WHITE*0.2f;
	mtrl2.Power    = 3.0f;

	//D3DMATERIAL9 mtrl3;
	mtrl3.Ambient  = d3d::WHITE;
	mtrl3.Diffuse.a = 1.0f;
	mtrl3.Diffuse.r = 1.0f;
	mtrl3.Diffuse.g = 1.0f;
	mtrl3.Diffuse.b = 1.0f;
	mtrl3.Specular = d3d::WHITE;
	mtrl3.Emissive = d3d::WHITE*0.5f;
	mtrl3.Power    = 5.0f;

	//Device->SetMaterial(&mtrl2);


	//
	// Setup a directional light.
	//
	
	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type      = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse.a = 0.8f;
	dir.Diffuse.r = 0.15f;
	dir.Diffuse.g = 0.15f;
	dir.Diffuse.b = 0.15f;
	dir.Specular  = d3d::RED * 0.05f;
	dir.Ambient   = d3d::WHITE * 0.05f;
	dir.Direction = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	
	D3DLIGHT9 dir2;
	::ZeroMemory(&dir2, sizeof(dir2));
	dir2.Type      = D3DLIGHT_DIRECTIONAL;
	dir2.Diffuse   = d3d::WHITE*0.7f ;
	dir2.Specular  = d3d::WHITE * 0.1f;
	dir2.Ambient   = d3d::WHITE * 0.1f;
	dir2.Direction = D3DXVECTOR3(1.0f, -1.0f, 1.0f);


	// Fill in a light structure defining our light
	  D3DLIGHT9 ppLight;
	  ::ZeroMemory( &ppLight, sizeof(ppLight) );
	  ppLight.Type       = D3DLIGHT_POINT;
	  ppLight.Diffuse.r  = 0.0f;
	  ppLight.Diffuse.g  = 1.0f;
	  ppLight.Diffuse.b  = 1.0f;
	  ppLight.Position.x =  1.0f;
	  ppLight.Position.y =  1.0f;
	  ppLight.Position.z =  0.0f;
	  ppLight.Attenuation0 = 0.5f;
	  ppLight.Range=10.0f;

  	// Fill in a light structure defining our light
	  D3DLIGHT9 pppLight;
	  ::ZeroMemory( &pppLight, sizeof(pppLight) );
	  pppLight.Type       = D3DLIGHT_POINT;
	  pppLight.Diffuse.r  = 0.5f;
	  pppLight.Diffuse.g  = 0.5f;
	  pppLight.Diffuse.b  = 1.0f;
	  pppLight.Position.x =  -1.0f;
	  pppLight.Position.y =  1.0f;
	  pppLight.Position.z =  0.0f;
	  pppLight.Attenuation0 = 0.5f;
	  pppLight.Range=10.0f;
	//
	// Set and Enable the light.
	//
	
	Device->SetLight(0, &dir);
	Device->LightEnable(0, false);
	Device->SetLight(1, &dir2);
	Device->LightEnable(1, true);
	Device->SetLight(2, &ppLight);
	Device->LightEnable(2, true);
	Device->SetLight(3, &pppLight);
	Device->LightEnable(3, true);

	//
	// Turn on specular lighting and instruct Direct3D
	// to renormalize normals.
	//

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set the projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(Pyramid);
	d3d::Release<IDirect3DVertexBuffer9*>(Pyramid2);
	d3d::Release<IDirect3DVertexBuffer9*>(Pyramid3);
	d3d::Release<IDirect3DVertexBuffer9*>(Pyramid4);
	d3d::Release<IDirect3DVertexBuffer9*>(Wall);
	d3d::Release<IDirect3DVertexBuffer9*>(Plane);
	d3d::Release<IDirect3DVertexBuffer9*>(Player);
	d3d::Release<IDirect3DTexture9*>(Tex);
	d3d::Release<IDirect3DTexture9*>(Tex2);
	d3d::Release<IDirect3DTexture9*>(Tex3);
	d3d::Release<IDirect3DTexture9*>(Tex_BG);
	d3d::Release<IDirect3DTexture9*>(TexP_U[0]);
	d3d::Release<IDirect3DTexture9*>(TexP_U[1]);
	d3d::Release<IDirect3DTexture9*>(TexP_U[2]);
	d3d::Release<IDirect3DTexture9*>(TexP_U[3]);
	d3d::Release<IDirect3DTexture9*>(TexP_D[0]);
	d3d::Release<IDirect3DTexture9*>(TexP_D[1]);
	d3d::Release<IDirect3DTexture9*>(TexP_D[2]);
	d3d::Release<IDirect3DTexture9*>(TexP_D[3]);
	d3d::Release<IDirect3DTexture9*>(TexP_L[0]);
	d3d::Release<IDirect3DTexture9*>(TexP_L[1]);
	d3d::Release<IDirect3DTexture9*>(TexP_L[2]);
	d3d::Release<IDirect3DTexture9*>(TexP_L[3]);
	d3d::Release<IDirect3DTexture9*>(TexP_R[0]);
	d3d::Release<IDirect3DTexture9*>(TexP_R[1]);
	d3d::Release<IDirect3DTexture9*>(TexP_R[2]);
	d3d::Release<IDirect3DTexture9*>(TexP_R[3]);
	d3d::Release<IDirect3DTexture9*>(Tex_Floor);
	d3d::Release<ID3DXMesh*>(Text);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		
		float h =0;

		D3DXMATRIX yRot;

		static float y = 0.0f;

		D3DXMatrixRotationY(&yRot, y);
		//y += timeDelta;
		y = 0.0f ;
		if( y >= 6.28f ) { y = 0.0f; }

		Device->SetTransform(D3DTS_WORLD, &yRot);

		
		//coin coll
		int i =0;
		int c[10] ={0};
		while(i<9){
			//定義物件大小
			if(Player_x < coinX[i]+0.5f & Player_x > coinX[i]-0.5f & Player_y < coinY[i]+0.5f & Player_y > coinY[i]-0.5f & Player_z < coinZ[i]+0.5f  & Player_z > coinZ[i]-0.5f ){
				c[i]=1;
			}
			i++;
		}
		for(int j=0;j<9;j++){
			if(c[j] == 1){
				coinLive[j]=1;
			}
		}
		
		//頭頂
		if(!collision(objX, objY, objZ, 6, Player_x, Player_y, Player_z) & Player_y > upcollision(objX, objY, objZ, 6, Player_x, Player_y, Player_z)){
			jump =0;s=0;
		}
		//平台碰撞
		if(!collision(objX, objY, objZ, 6, Player_x, Player_y, Player_z) & Player_y < downcollision(objX, objY, objZ, 6, Player_x, Player_y, Player_z)){
			Player_y=downcollision(objX, objY, objZ, 6, Player_x, Player_y, Player_z);
			stop =0;
		}
	
		// Input 
			if( Player_z<=5.5f ){
				if( ::GetAsyncKeyState(VK_UP) & 0x8000f ) 
				{ 
					if( collision(objX, objY, objZ, 6, Player_x, Player_y, Player_z)||PDir==2)
					{
						Player_z+=0.005;PDir=1;
					} 
					PAni=true;
				} 
				
			} 
		
			if( Player_z >=-0.5f ){
				if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
				{ 
					if(collision(objX, objY, objZ, 6, Player_x, Player_y, Player_z)||PDir==1)
					{
						Player_z-=0.005;PDir=2;
					}
					PAni=true;
				} 
				
			}
			if( Player_x>=-5.5f ){
				if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f ) 
				{ 
					if(collision(objX, objY, objZ, 6, Player_x, Player_y, Player_z)||PDir==4)
					{					
						Player_x-=0.005; PDir=3;
					}
					PAni=true;
				}
				
			}

			if( Player_x<=5.5f){
				if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f ) 
				{ 
					if(collision(objX, objY, objZ, 6, Player_x, Player_y, Player_z)||PDir==3)
					{
						Player_x+=0.005;PDir=4;
					} 
					PAni=true;
				} 
				
			}
		
			//if( Player_x<=6.0f ){
				if( ::GetAsyncKeyState('Z') & 0x8000f ){
					if(jump ==0 & stop ==0){
						jump = 1;
						PJump==true;
					}
				}
				if( ::GetAsyncKeyState('S') & 0x8000f ){
						Player_y+=0.02f;
				}
				if( ::GetAsyncKeyState('A') & 0x8000f ){
						Player_y-=0.02f;
				}
			//}
			Player_y=Player_y-6* timeDelta;
			if(jump == 1){
				stop=1;
				if(s < 5){
					h = 20 *timeDelta;
					Player_y+=h;
					s+=h;
				}
				else{
					jump =0;s=0;
				}
			}
		//
		// Position and aim the camera.
		//

		D3DXVECTOR3 pos(2.0f-cx, -2.0f, -4.0f+cz);
		D3DXVECTOR3 target(2.0f-cx, -3.0f+cy, 4.0f+cz);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &pos, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();
		
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true); // Alpha open
		
		D3DXMATRIX W;
		D3DXMatrixIdentity(&W);


		//ROOM Front
		Device->SetMaterial(&mtrl);
		Device->SetStreamSource(0, Pyramid, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex_BG);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		//ROOM R & L
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Pyramid2, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex_BG);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		//ROOM Floor
		objX[0] = 0.0f ;
		objY[0] = -6.0f ;
		objZ[0] = 0.0f ;
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Pyramid3, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex_Floor);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		//ROOM Sky
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Pyramid4, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex_BG);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		

		// Plane Grass 
		D3DXMatrixTranslation(&W, -4.0f, -5.5f, 5.0f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Plane, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		D3DXMatrixTranslation(&W, 3.0f, -5.5f, 4.5f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Plane, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		D3DXMatrixTranslation(&W, 5.0f, -5.5f, 4.0f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Plane, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);

		D3DXMatrixTranslation(&W, -5.0f, -5.5f, 4.0f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Plane, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		D3DXMatrixTranslation(&W, -1.0f, -5.5f, 3.0f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Plane, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);

		// OBJs
		objX[1] = -3.0f ;	objY[1] = -5.5f ;	objZ[1] = 1.0f ;
		D3DXMatrixTranslation(&W, objX[1], objY[1], objZ[1]);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Wall, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, wallTex1);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		objX[2] = 2.0f ;	objY[2] = -5.5f ;	objZ[2] = 1.0f ;
		D3DXMatrixTranslation(&W, objX[2], objY[2], objZ[2]);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Wall, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, wallTex1);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		objX[3] = 0.0f ;	objY[3] = -3.0f ;	objZ[3] = 1.0f ;
		D3DXMatrixTranslation(&W, objX[3], objY[3], objZ[3]);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Wall, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, wallTex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		objX[4] = 5.0f ;	objY[4] = -3.0f ;	objZ[4] = 1.0f ;
		D3DXMatrixTranslation(&W, objX[4], objY[4], objZ[4]);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Wall, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, wallTex1);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		objX[5] = 4.0f ;	objY[5] = -3.0f ;	objZ[5] = 1.0f ;
		D3DXMatrixTranslation(&W, objX[5], objY[5], objZ[5]);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Wall, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, wallTex1);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);

		// Coin
		if(coinLive[0] == 0){
			coinX[0] = -3.0f;	coinY[0] = -3.0f;	coinZ[0] = 1.0f;
			D3DXMatrixTranslation(&W, coinX[0], coinY[0], coinZ[0]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}
		
		if(coinLive[1] == 0){
			coinX[1] = 2.0f;	coinY[1] = -3.0f;	coinZ[1] = 1.0f;
			D3DXMatrixTranslation(&W, coinX[1], coinY[1], coinZ[1]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}

		if(coinLive[2] == 0){
			coinX[2] = 0.0f;	coinY[2] = -0.5f;	coinZ[2] = 1.0f;
			D3DXMatrixTranslation(&W, coinX[2], coinY[2], coinZ[2]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}
		
		if(coinLive[3] == 0){
			coinX[3] = 1.0f;	coinY[3] = -0.5f;	coinZ[3] = 1.0f;
			D3DXMatrixTranslation(&W, coinX[3], coinY[3], coinZ[3]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}
		
		if(coinLive[4] == 0){
			coinX[4] = -1.0f;	coinY[4] = -0.5f;	coinZ[4] = 1.0f;
			D3DXMatrixTranslation(&W, coinX[4], coinY[4], coinZ[4]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}
		
		if(coinLive[5] == 0){
			coinX[5] = 4.0f;	coinY[5] = 0.5f;	coinZ[5] = 1.0f;
			D3DXMatrixTranslation(&W, coinX[5], coinY[5], coinZ[5]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}
		
		if(coinLive[6] == 0){
			coinX[6] = 5.0f;	coinY[6] = 0.5f;	coinZ[6] = 1.0f;
			D3DXMatrixTranslation(&W, coinX[6], coinY[6], coinZ[6]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}
		
		if(coinLive[7] == 0){
			coinX[7] = 4.0f;	coinY[7] = -5.5f;	coinZ[7] = -0.5f;
			D3DXMatrixTranslation(&W, coinX[7], coinY[7], coinZ[7]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}
		
		if(coinLive[8] == 0){
			coinX[8] = 5.0f;	coinY[8] = -5.5f;	coinZ[8] = -0.5f;
			D3DXMatrixTranslation(&W, coinX[8], coinY[8], coinZ[8]);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Coin, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				//Ani
				TexCoin_time += 16*timeDelta ;
				if ( TexCoin_time>=12 ) { TexCoin_i += 1 ; TexCoin_time=0 ; }
				if ( TexCoin_i>=4 ) TexCoin_i=0 ;
				Device->SetTexture(0, Tex_Coin[TexCoin_i]);
				//Ani END
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}

		// Player
		D3DXMatrixTranslation(&W, Player_x, Player_y, Player_z);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl3);
		Device->SetStreamSource(0, Player, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, TexP_D[0]);
			// Animation
		if ( PAni==true ){
			TexC_time += 16*timeDelta ;
			if ( TexC_time>=2 ) { TexC_i += 1 ; TexC_time=0 ; }
			if ( TexC_i>=4 ) TexC_i=0 ;
			if (PDir==1) Device->SetTexture(0, TexP_U[TexC_i]);
			if (PDir==2) Device->SetTexture(0, TexP_D[TexC_i]);
			if (PDir==3) Device->SetTexture(0, TexP_R[TexC_i]);
			if (PDir==4) Device->SetTexture(0, TexP_L[TexC_i]);
		}
		if ( stop==1 ){
			if (PDir==1 || PDir==3) Device->SetTexture(0, TexP_JR);
			if (PDir==2 || PDir==4) Device->SetTexture(0, TexP_JL);
		}
			// Animation End
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,6);
		
		// Plane Grass Front
		D3DXMatrixTranslation(&W, -1.5f, -5.5f, -0.7f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Plane, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);

		D3DXMatrixTranslation(&W, -4.0f, -5.5f, -1.0f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Plane, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		D3DXMatrixTranslation(&W, 4.0f, -5.5f, -1.0f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, Plane, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->SetTexture(0, Tex2);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);

		
		// TextTime
		D3DXMatrixTranslation(&W, 1.0f, -0.3f, -1.0f);
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Device->SetStreamSource(0, TextTime, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
			//Ani
			TexNum_time += 16*timeDelta ;
			if ( TexNum_time>=17 ) { TexNum_i += 1 ; TexNum_time=0 ; }
			if ( TexNum_i>=26 ) TexNum_i=25 ;
			Device->SetTexture(0, Tex_Num[TexNum_i]);
			//Ani END
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		
		// Result !!
		
		wiggle_angle += 0.5*0.01744444 ;
		wiggle_y = 0.2*sinf(wiggle_angle);
		if ( TexNum_i<25 &&
		(coinLive[0]+coinLive[1]+coinLive[2]+coinLive[3]+coinLive[4]+coinLive[5]+coinLive[6]+coinLive[7]+coinLive[8])>=9 ){
			D3DXMatrixTranslation(&W, 1.0f, -0.3f+wiggle_y, -1.05f);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Result, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				Device->SetTexture(0, Tex_ResultOK);
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}
		if (TexNum_i==25 &&
		(coinLive[0]+coinLive[1]+coinLive[2]+coinLive[3]+coinLive[4]+coinLive[5]+coinLive[6]+coinLive[7]+coinLive[8])<9){
			D3DXMatrixTranslation(&W, 1.0f, -0.3f+wiggle_y, -1.05f);
			Device->SetTransform(D3DTS_WORLD, &W);
			Device->SetMaterial(&mtrl3);
			Device->SetStreamSource(0, Result, 0, sizeof(Vertex));
			Device->SetFVF(Vertex::FVF);
				Device->SetTexture(0, Tex_ResultBAD);
			Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,12);
		}

		// 3D Text
		//
		// Update: Spin the 3D text.
		//
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&mtrl2);
		Text->DrawSubset(0);

		// Reset check
		PAni = false ;
		if(Player_y<-5.5){
			Player_y=-5.5;
			stop =0;
		}
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}

	return true;
}


//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}

// ----- Function ------------------------------------------------------------------
bool collision(float x[],float y[],float z[] ,int n,float px,float py,float pz)
{
	int i =1;
	int c =0;
	while(i<n){
		//定義物件大小
		if(px-0.5f > x[i]+0.5f || px+0.5f < x[i]-0.5f || py-0.5f > y[i]+0.49f || py+0.5f < y[i]-0.5f || pz-0.01f > z[i]+0.5f  || pz+0.01f < z[i]-0.5f ){
			i++;
		}
		else{ 
			c++; i++;

		}
	}
	if(c > 0) return false;
	else return true;
}


float downcollision(float x[],float y[],float z[] ,int n,float px,float py,float pz)
{
	int i =1;
	int c =0;
	int box[] ={0};
	while(i<n){
		//定義物件大小
		if(px-0.47f < x[i]+0.5f & px+0.47f > x[i]-0.5f & pz-0.1f < z[i]+0.5f & pz+0.1f > z[i]-0.5f & py > y[i]+0.5f){
			return y[i]+1.0f;
			break;
		}
		i++;
	}
}

float upcollision(float x[],float y[],float z[] ,int n,float px,float py,float pz)
{
	int i =1;
	int c =0;
	int box[] ={0};
	while(i<n){
		//定義物件大小
		if(px < x[i]+0.5f & px > x[i]-0.5f & pz < z[i]+0.5f & pz > z[i]-0.5f & py < y[i]-0.5f){
			return y[i]-1.0f;
			break;
		}
		i++;
	}
}


int coincoll(float x[],float y[],float z[] ,int n,float px,float py,float pz)
{
	int i =0;
	int c[10] ={0};
	while(i<n){
		//定義物件大小
		if(px < x[i]+0.5f & px > x[i]-0.5f & py < y[i]+0.5f & py > y[i]-0.5f & pz < z[i]+0.5f  & pz > z[i]-0.5f ){
			c[i]=1;
		}
		else{ 
			c[i]=1;
		}
		i++;
	}
	return i;
}