// <>============================================<>
// ||\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/||
// ||              Armstrong Lohãns              ||
// ||    Realidade Virtual e Aumentada 2021.1    ||
// ||\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/||
// <>============================================<>

// OBS: EXECUTADO EM C NO LINUX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>

typedef struct {
   double x,y,z;
} XYZ;

#define ABS(x) (x < 0 ? -(x) : (x))
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
#define SIGN(x) (x < 0 ? (-1) : 1)
#define MODULUS(p) (sqrt(p.x*p.x + p.y*p.y + p.z*p.z))
#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x

typedef struct {
   XYZ vp;                           /* Posição de visualização */
   XYZ vd;                           /* Direção de visualização do vetor */
   XYZ vu;                           /* Direção de visualização ascendente */
   XYZ pr;                           /* Ponto para girar sobre */
   double distancia_focal;           /* Distância focal ao longo de vd */
   double abertura_camera;           /* Abertura da câmera */
   double separacao_dos_olhos;       /* Separação dos olhos */
   int altura_tela, largura_tela;
} CAMERA;

void ManipularExibicao(void);
void CriarAmbiente(void);
void CriarMundo(void);
void RenderizarBule(void);
void ManipularTeclado(unsigned char key,int x, int y);
void ManipularSetasDoTeclado(int key,int x, int y);
void ManipularMenuPrincipal(int);
void ManipularMenuDeLentes(int);
void ManipularVisibilidade(int vis);
void ManipularRemodelacao(int,int);
void ManipularCronometro(int);
void GirarCamera(int,int,int);
void TransladarCamera(int,int);
void SetarCameraParaOrigem(int);
void Normalizar(XYZ *);

#define DTOR            0.0174532925
#define RTOD            57.2957795
#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322
#define ESC 27

