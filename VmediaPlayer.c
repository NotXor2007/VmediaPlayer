#include "./include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60

//float GetFrameTime(void);
//double GetTime(void);
//void DrawLineBezier(Vector2 startPos, Vector2 endPos, float thick, Color color);
//void DrawRectangleGradientV(int posX, int posY, int width, int height, Color top, Color bottom);
//void DrawRectangleGradientH(int posX, int posY, int width, int height, Color left, Color right);
//void DrawRectangleGradientEx(Rectangle rec, Color topLeft, Color bottomLeft, Color topRight, Color bottomRight);

void initVmedia(void);
static void decodeVmedia(const char*);
unsigned int exitVmedia(const char);
Vector2* scaleBeziers(const unsigned int);

unsigned int oldWidth, oldHeight, resWidth, resHeight, fps, FrameTimer;
float scale;
char *title;
Color backgroundColor;

typedef struct CircleShape
{
	float center[2];
	float radius;
	float StrokeLength;
	Color FillColor;
	Color StrokeColor;
} CircleObj;

typedef struct RectangleShape
{
	float x;
	float y;
	float width;
	float height;
	Color color;
} RectangleObj;

typedef struct OvalShape
{
	float center[2];
	float hradius;
	float vradius;
	Color color;
} OvalObj;

typedef struct LineShape
{
	float x1;
	float y1;
	float x2;
	float y2;
	float thickness;
	Color color;
} LineObj;

typedef struct TriangleShape
{
	float p1[2];
	float p2[2];
	float p3[2];
	Color color;
} TriangleObj;

typedef struct PolyShape
{
	float center[2];
	unsigned int sides;
	float radius;
	float angle;
	Color color;
} PolygonObj;

typedef struct QuadraticBezier
{
	unsigned int linesCount;
	Vector2* points;
	float thickness;
	Color color;
} QuadraticBezierObj;

typedef struct CubicBezier
{
	unsigned int linesCount;
	Vector2* points;
	float thickness;
	Color color;
} CubicBezierObj;

CircleObj *circles;
RectangleObj *rectangles;
OvalObj *ovals;
LineObj *lines;
TriangleObj *triangles;
PolygonObj *polygons;
QuadraticBezierObj *quadratics;
CubicBezierObj *cubics;

unsigned int shapesIndex[8];

unsigned int *shapes;
unsigned int shapes_count;

int main(int argc, char **argv)
{
	if(argc == 2)  decodeVmedia(argv[1]);
	else
		return exitVmedia(0x01); //file_path is unavailable
	initVmedia();
	while(!WindowShouldClose())
	{
		BeginDrawing();
			ClearBackground(backgroundColor);
			for(unsigned int index=0;index < shapes_count;index++)
			{
				switch(shapes[index])
				{
					case 0x00:
					       	DrawCircle((int)((*(circles+shapesIndex[0])).center[0]*scale), (int)((*(circles+shapesIndex[0])).center[1]*scale), 
								(*(circles+shapesIndex[0])).radius*scale, (*(circles+shapesIndex[0])).FillColor);
						shapesIndex[0]++;
						break;
					case 0x01: 
						DrawRectangle((int)((*(rectangles+shapesIndex[1])).x*scale),(int)((*(rectangles+shapesIndex[1])).y*scale), 
								(int)((*(rectangles+shapesIndex[1])).width*scale), (int)((*(rectangles+shapesIndex[1])).height*scale), (*(rectangles+shapesIndex[1])).color);
						shapesIndex[1]++;
						break;
					case 0x02:
						DrawEllipse((int)((*(ovals+shapesIndex[2])).center[0]*scale),(int)((*(ovals+shapesIndex[2])).center[1]*scale) ,
								(*(ovals+shapesIndex[2])).hradius*scale ,(*(ovals+shapesIndex[2])).vradius*scale ,(*(ovals+shapesIndex[2])).color);
						shapesIndex[2]++;
						break;
					case 0x04:
						DrawTriangle((Vector2){(*(triangles+shapesIndex[4])).p1[0]*scale, (*(triangles+shapesIndex[4])).p1[1]*scale}, 
								(Vector2){(*(triangles+shapesIndex[4])).p2[0]*scale, (*(triangles+shapesIndex[4])).p2[1]*scale}, 
								(Vector2){(*(triangles+shapesIndex[4])).p3[0]*scale, (*(triangles+shapesIndex[4])).p3[1]*scale},(*(triangles+shapesIndex[4])).color);
						shapesIndex[4]++;
						break;
					case 0x03:
						shapesIndex[3]++;
						break;
					case 0x05:
						DrawPoly((Vector2){(*(polygons+shapesIndex[5])).center[0]*scale,(*(polygons+shapesIndex[5])).center[1]*scale}, 
								(*(polygons+shapesIndex[5])).sides,(*(polygons+shapesIndex[5])).radius*scale,(*(polygons+shapesIndex[5])).angle, (*(polygons+shapesIndex[5])).color);
						shapesIndex[5]++;
						break;
					case 0x06:
						DrawSplineBezierQuadratic(scaleBeziers(3) ,(*(quadratics+shapesIndex[6])).linesCount*3 ,(*(quadratics+shapesIndex[6])).thickness*scale, (*(quadratics+shapesIndex[6])).color);
						shapesIndex[6]++;
						break;
					case 0x07:
						DrawSplineBezierCubic(scaleBeziers(4) ,(*(quadratics+shapesIndex[6])).linesCount*3 ,(*(quadratics+shapesIndex[6])).thickness*scale, (*(quadratics+shapesIndex[6])).color);
						shapesIndex[7]++;
						break;
					default:
				}
			}
		FrameTimer++;
		EndDrawing();
		memset(shapesIndex, 0, sizeof shapesIndex);
		//re-calculate layout
		if(GetScreenWidth() != oldWidth || GetScreenHeight() != oldHeight)
		{	
			scale = GetScreenWidth() > GetScreenHeight() ? (float)GetScreenHeight() / (float)resHeight : (float)GetScreenWidth() / (float)resWidth;
			oldWidth = GetScreenWidth();oldHeight = GetScreenHeight();
		}
	}
	return exitVmedia(0x00); //success
}

inline unsigned int exitVmedia(const char code)
{
	CloseWindow();
	return code;
}

void initVmedia()
{
	InitWindow(resWidth, resHeight, "VmediaPlayer v1.0 - Untitled");
	scale = 1;
	SetTargetFPS(fps);
	SetWindowTitle(title);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	memset(shapesIndex, 0, sizeof shapesIndex);
	FrameTimer = 0;
}

Vector2* scaleBeziers(const unsigned int pointsCount)
{
	Vector2* pts;
	if((pts = (Vector2*)malloc(sizeof (Vector2) * pointsCount * (*(quadratics+shapesIndex[6])).linesCount)) == NULL) exit(0x10);
	for(unsigned int pindex = 0;pindex < (*(quadratics+shapesIndex[6])).linesCount*pointsCount;pindex++)
	{
		pts[pindex].x = (*(quadratics+shapesIndex[6])).points[pindex].x*scale;
		pts[pindex].y = (*(quadratics+shapesIndex[6])).points[pindex].y*scale;
	}
	return pts;	

}

void parseHeader(FILE* media_file, unsigned char next_byte, unsigned char dlength)
{
	resWidth = 0;oldWidth = 0;resHeight = 0;oldHeight = 0;fps = 0;
	next_byte = (unsigned char)fgetc(media_file);
	for(dlength=0;dlength < next_byte;dlength++) resWidth = (resWidth << 8) + (unsigned int)fgetc(media_file);	
	next_byte = (unsigned char)fgetc(media_file);
	for(dlength=0;dlength < next_byte;dlength++) resHeight = (resHeight << 8) + (unsigned int)fgetc(media_file);	
	next_byte = (unsigned char)fgetc(media_file);
	for(dlength=0;dlength < next_byte;dlength++) fps = (fps << 8) + (unsigned int)fgetc(media_file);	
	backgroundColor.r = 0xFF;
	backgroundColor.g = 0xFF;
	backgroundColor.b = 0xFF;
	backgroundColor.a = 0xFF;
}

static void decodeVmedia(const char *file_path)
{
	FILE* media_file;
	int byte;
	unsigned char next_byte;
	unsigned char dlength;
	unsigned char circles_count;
	unsigned char rectangles_count;
	unsigned int ovals_count;
	unsigned int lines_count;
	unsigned int triangles_count;
	unsigned int polygons_count;
	unsigned int quadratics_count;
	unsigned int cubics_count;
	unsigned int color_array[4];
	unsigned int StrokeColors[4];
	if( (media_file = fopen(file_path, "r")) == NULL ) exit(0x02); //file path incorrect
	parseHeader(media_file, next_byte, dlength);
	circles_count = 0;rectangles_count = 0;ovals_count = 0;lines_count = 0;triangles_count = 0;polygons_count = 0; //init shape count vars
	quadratics_count = 0;cubics_count = 0;
	while( ( byte = fgetc(media_file) ) != EOF )
	{
		switch(byte)
		{
			case 0x00: //title
				next_byte = (unsigned char)fgetc(media_file);
				if((title = (char*)malloc(sizeof(char)*(next_byte+1))) == NULL)
					exit(0x04);
				for(dlength=0;dlength < next_byte;dlength++) 
					title[dlength] = (char)fgetc(media_file);
				title[dlength] = '\0';
				break;
			case 0x01: //bg color
				memset(color_array, 0x00, sizeof color_array);
				color_array[3] = 0xFF;
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					color_array[dlength] = (unsigned char)fgetc(media_file);
				backgroundColor.r = color_array[0];
				backgroundColor.g = color_array[1];
				backgroundColor.b = color_array[2];
				backgroundColor.a = color_array[3];
				break;
			case 0x02: //circles
				circles_count++;
				shapes_count++;	
				if((circles = (CircleObj*)realloc(circles,sizeof(CircleObj)*circles_count)) == NULL) exit(0x03);	
				if((shapes = (unsigned int*)realloc(shapes,sizeof(unsigned int)*shapes_count)) == NULL) exit(0x03);
				shapes[shapes_count-1] = 0x00; //circle
				memset((*(circles+circles_count-1)).center, 0, sizeof (*(circles+circles_count-1)).center);
				(*(circles+circles_count-1)).radius = 0; 
				(*(circles+circles_count-1)).StrokeLength = 0; 
				memset(color_array, 0, sizeof color_array);
				memset(StrokeColors, 0, sizeof StrokeColors);
				color_array[3] = 0xFF;StrokeColors[3] = 0xFF;
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(circles+circles_count-1)).center[0] = (float)((int)(*(circles+circles_count-1)).center[0] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(circles+circles_count-1)).center[1] = (float)((int)(*(circles+circles_count-1)).center[1] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(circles+circles_count-1)).radius = (float)((int)(*(circles+circles_count-1)).radius << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(circles+circles_count-1)).StrokeLength = (float)((int)(*(circles+circles_count-1)).StrokeLength << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					color_array[dlength] = (unsigned char)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					StrokeColors[dlength] = (unsigned char)fgetc(media_file);
				(*(circles+circles_count-1)).FillColor.r = color_array[0];
				(*(circles+circles_count-1)).FillColor.g = color_array[1];
				(*(circles+circles_count-1)).FillColor.b = color_array[2];
				(*(circles+circles_count-1)).FillColor.a = color_array[3];
				(*(circles+circles_count-1)).StrokeColor.r = StrokeColors[0];
				(*(circles+circles_count-1)).StrokeColor.g = StrokeColors[1];
				(*(circles+circles_count-1)).StrokeColor.b = StrokeColors[2];
				(*(circles+circles_count-1)).StrokeColor.a = StrokeColors[3];
				break;
			case 0x03: //rects
				rectangles_count++;
				shapes_count++;	
				if((rectangles = (RectangleObj*)realloc(rectangles,sizeof(RectangleObj)*rectangles_count)) == NULL) exit(0x03);
				if((shapes = (unsigned int*)realloc(shapes,sizeof(unsigned int)*shapes_count)) == NULL) exit(0x03);
				shapes[shapes_count-1] = 0x01; //rectangle
				(*(rectangles+rectangles_count-1)).x = 0; 
				(*(rectangles+rectangles_count-1)).y = 0; 
				(*(rectangles+rectangles_count-1)).width = 0; 
				(*(rectangles+rectangles_count-1)).height = 0; 
				memset(color_array, 0, sizeof color_array);
				color_array[3] = 0xFF;
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(rectangles+rectangles_count-1)).x = (float)((int)(*(rectangles+rectangles_count-1)).x << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(rectangles+rectangles_count-1)).y = (float)((int)(*(rectangles+rectangles_count-1)).y << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(rectangles+rectangles_count-1)).width = (float)((int)(*(rectangles+rectangles_count-1)).width << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(rectangles+rectangles_count-1)).height = (float)((int)(*(rectangles+rectangles_count-1)).height << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					color_array[dlength] = (unsigned char)fgetc(media_file);
				(*(rectangles+rectangles_count-1)).color.r = color_array[0];
				(*(rectangles+rectangles_count-1)).color.g = color_array[1];
				(*(rectangles+rectangles_count-1)).color.b = color_array[2];
				(*(rectangles+rectangles_count-1)).color.a = color_array[3];
				break;
			case 0x04: //ovals
				ovals_count++;
				shapes_count++;	
				if((ovals = (OvalObj*)realloc(ovals,sizeof(OvalObj)*ovals_count)) == NULL) exit(0x03);
				if((shapes = (unsigned int*)realloc(shapes,sizeof(unsigned int)*shapes_count)) == NULL) exit(0x03);
				shapes[shapes_count-1] = 0x02; //oval
				memset((*(ovals+ovals_count-1)).center, 0, sizeof (*(ovals+ovals_count-1)).center);
				(*(ovals+ovals_count-1)).hradius = 0; 
				(*(ovals+ovals_count-1)).vradius = 0;
				memset(color_array, 0, sizeof color_array);
				color_array[3] = 0xFF;
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(ovals+ovals_count-1)).center[0] = (float)((int)(*(ovals+ovals_count-1)).center[0] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(ovals+ovals_count-1)).center[1] = (float)((int)(*(ovals+ovals_count-1)).center[1] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(ovals+ovals_count-1)).hradius = (float)((int)(*(ovals+ovals_count-1)).hradius << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(ovals+ovals_count-1)).vradius = (float)((int)(*(ovals+ovals_count-1)).vradius << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					color_array[dlength] = (unsigned char)fgetc(media_file);
				(*(ovals+ovals_count-1)).color.r = color_array[0];
				(*(ovals+ovals_count-1)).color.g = color_array[1];
				(*(ovals+ovals_count-1)).color.b = color_array[2];
				(*(ovals+ovals_count-1)).color.a = color_array[3];
				break;
			case 0x05:
				break;
			case 0x06: //triangles
				triangles_count++;
				shapes_count++;	
				if((triangles = (TriangleObj*)realloc(triangles,sizeof(TriangleObj)*triangles_count)) == NULL) exit(0x03);
				if((shapes = (unsigned int*)realloc(shapes,sizeof(unsigned int)*shapes_count)) == NULL) exit(0x03);
				shapes[shapes_count-1] = 0x04; //triangle
				memset((*(triangles+triangles_count-1)).p1, 0, sizeof (*(triangles+triangles_count-1)).p1);
				memset((*(triangles+triangles_count-1)).p2, 0, sizeof (*(triangles+triangles_count-1)).p2);
				memset((*(triangles+triangles_count-1)).p3, 0, sizeof (*(triangles+triangles_count-1)).p3);
				memset(color_array, 0, sizeof color_array);
				color_array[3] = 0xFF;
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(triangles+triangles_count-1)).p1[0] = (float)((int)(*(triangles+triangles_count-1)).p1[0] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(triangles+triangles_count-1)).p1[1] = (float)((int)(*(triangles+triangles_count-1)).p1[1] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(triangles+triangles_count-1)).p2[0] = (float)((int)(*(triangles+triangles_count-1)).p2[0] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(triangles+triangles_count-1)).p2[1] = (float)((int)(*(triangles+triangles_count-1)).p2[1] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(triangles+triangles_count-1)).p3[0] = (float)((int)(*(triangles+triangles_count-1)).p3[0] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(triangles+triangles_count-1)).p3[1] = (float)((int)(*(triangles+triangles_count-1)).p3[1] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					color_array[dlength] = (unsigned char)fgetc(media_file);
				(*(triangles+triangles_count-1)).color.r = color_array[0];
				(*(triangles+triangles_count-1)).color.g = color_array[1];
				(*(triangles+triangles_count-1)).color.b = color_array[2];
				(*(triangles+triangles_count-1)).color.a = color_array[3];
				break;
			case 0x07: //polygons
				polygons_count++;
				shapes_count++;	
				if((polygons = (PolygonObj*)realloc(polygons,sizeof(PolygonObj)*polygons_count)) == NULL) exit(0x03);
				if((shapes = (unsigned int*)realloc(shapes,sizeof(unsigned int)*shapes_count)) == NULL) exit(0x03);
				shapes[shapes_count-1] = 0x05; //polygon
				memset((*(polygons+polygons_count-1)).center, 0, sizeof (*(polygons+polygons_count-1)).center);
				(*(polygons+polygons_count-1)).sides = 0;
				(*(polygons+polygons_count-1)).radius = 0; 
				(*(polygons+polygons_count-1)).angle = 0;
				memset(color_array, 0, sizeof color_array);
				color_array[3] = 0xFF;				
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(polygons+polygons_count-1)).center[0] = (float)((int)(*(polygons+polygons_count-1)).center[0] << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(polygons+polygons_count-1)).center[1] = (float)((int)(*(polygons+polygons_count-1)).center[1] << 8) + (float)fgetc(media_file);				
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(polygons+polygons_count-1)).sides = (float)((*(polygons+polygons_count-1)).sides << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(polygons+polygons_count-1)).radius = (float)((int)(*(polygons+polygons_count-1)).radius << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(polygons+polygons_count-1)).angle = (float)((int)(*(polygons+polygons_count-1)).angle << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					color_array[dlength] = (unsigned char)fgetc(media_file);
				(*(polygons+polygons_count-1)).color.r = color_array[0];
				(*(polygons+polygons_count-1)).color.g = color_array[1];
				(*(polygons+polygons_count-1)).color.b = color_array[2];
				(*(polygons+polygons_count-1)).color.a = color_array[3];
				break;
			case 0x08: //quadratic beziers
				quadratics_count++;
				shapes_count++;	
				if((quadratics = (QuadraticBezierObj*)realloc(quadratics,sizeof(QuadraticBezierObj)*quadratics_count)) == NULL) exit(0x03);
				if((shapes = (unsigned int*)realloc(shapes,sizeof(unsigned int)*shapes_count)) == NULL) exit(0x03);
				shapes[shapes_count-1] = 0x06; //quadratic
				(*(quadratics+quadratics_count-1)).linesCount = 0;
				(*(quadratics+quadratics_count-1)).thickness = 0; 
				memset(color_array, 0, sizeof color_array);
				color_array[3] = 0xFF;				
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(quadratics+quadratics_count-1)).linesCount = ((*(quadratics+quadratics_count-1)).linesCount << 8) + (unsigned int)fgetc(media_file);
				if(((*(quadratics+quadratics_count-1)).points = (Vector2*)malloc(sizeof(Vector2)*((*(quadratics+quadratics_count-1)).linesCount*3))) == NULL) exit(0x03);
				memset((*(quadratics+quadratics_count-1)).points, 0, sizeof(Vector2)*((*(quadratics+quadratics_count-1)).linesCount*3));
				for(unsigned int pindex = 0;pindex < (*(quadratics+quadratics_count-1)).linesCount*3;pindex++)
				{	
					next_byte = (unsigned char)fgetc(media_file);
					for(dlength=0;dlength < next_byte;dlength++)
						(*(quadratics+quadratics_count-1)).points[pindex].x = (float)((int)(*(quadratics+quadratics_count-1)).points[pindex].x << 8) + (float)fgetc(media_file);					
					next_byte = (unsigned char)fgetc(media_file);
					for(dlength=0;dlength < next_byte;dlength++)
						(*(quadratics+quadratics_count-1)).points[pindex].y = (float)((int)(*(quadratics+quadratics_count-1)).points[pindex].y << 8) + (float)fgetc(media_file);
				}
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(quadratics+quadratics_count-1)).thickness = (float)((int)(*(quadratics+quadratics_count-1)).thickness << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					color_array[dlength] = (unsigned char)fgetc(media_file);
				(*(quadratics+quadratics_count-1)).color.r = color_array[0];
				(*(quadratics+quadratics_count-1)).color.g = color_array[1];
				(*(quadratics+quadratics_count-1)).color.b = color_array[2];
				(*(quadratics+quadratics_count-1)).color.a = color_array[3];
				break;
			case 0x09: //cubic beziers
				cubics_count++;
				shapes_count++;	
				if((cubics = (CubicBezierObj*)realloc(cubics,sizeof(CubicBezierObj)*cubics_count)) == NULL) exit(0x03);
				if((shapes = (unsigned int*)realloc(shapes,sizeof(unsigned int)*shapes_count)) == NULL) exit(0x03);
				shapes[shapes_count-1] = 0x07; //cubic
				(*(cubics+cubics_count-1)).linesCount = 0;
				(*(cubics+cubics_count-1)).thickness = 0; 
				memset(color_array, 0, sizeof color_array);
				color_array[3] = 0xFF;				
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(cubics+cubics_count-1)).linesCount = ((*(cubics+cubics_count-1)).linesCount << 8) + (unsigned int)fgetc(media_file);
				if(((*(cubics+cubics_count-1)).points = (Vector2*)malloc(sizeof(Vector2)*((*(cubics+cubics_count-1)).linesCount*4))) == NULL) exit(0x03);
				memset((*(cubics+cubics_count-1)).points, 0, sizeof (*(cubics+cubics_count-1)).points);
				for(unsigned int pindex = 0;pindex < (*(cubics+cubics_count-1)).linesCount*4;pindex++)
				{	
					next_byte = (unsigned char)fgetc(media_file);
					for(dlength=0;dlength < next_byte;dlength++)
						(*(cubics+cubics_count-1)).points[pindex].x = (float)((int)(*(cubics+cubics_count-1)).points[pindex].x << 8) + (float)fgetc(media_file);					
					next_byte = (unsigned char)fgetc(media_file);
					for(dlength=0;dlength < next_byte;dlength++)
						(*(cubics+cubics_count-1)).points[pindex].y = (float)((int)(*(cubics+cubics_count-1)).points[pindex].y << 8) + (float)fgetc(media_file);
				}
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++)
					(*(cubics+cubics_count-1)).thickness = (float)((int)(*(cubics+cubics_count-1)).thickness << 8) + (float)fgetc(media_file);
				next_byte = (unsigned char)fgetc(media_file);
				for(dlength=0;dlength < next_byte;dlength++) 
					color_array[dlength] = (unsigned char)fgetc(media_file);
				(*(cubics+cubics_count-1)).color.r = color_array[0];
				(*(cubics+cubics_count-1)).color.g = color_array[1];
				(*(cubics+cubics_count-1)).color.b = color_array[2];
				(*(cubics+cubics_count-1)).color.a = color_array[3];
				break;
			default:
				goto End;
		}
	}
	End:
		fclose(media_file);
}
