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
#include <unistd.h>
#include <GL/glut.h>
#include "anaglifo_opengl_armstrong.h"

/* Miscelânia Global */
int botao_atual = -1;                  /* Botão nulo inicial */
float velocidade_de_rotacao = 0.5;     /* Faz o objeto girar automaticamente */
double angulo_rotacao_camera = 1.0;    /* Incremento do ângulo de rotação da câmera */
CAMERA camera;                         /* Câmera */
XYZ origin = {0.0,0.0,0.0};            /* Ponto de origem */

/* Tipos de filtro de óculos anaglifos */
#define VERMELHO_CIANO  1
#define CIANO_VERMELHO  2
int tipo_lente = CIANO_VERMELHO;

int main(int argc,char **argv)
{
   int menu_principal, menu_lentes;

   glutInit(&argc,argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGB | GLUT_DEPTH);

   /* Cria a janela e os manipuladores */
   glutCreateWindow("Anaglifo de um bule - Armstrong - RVA");
   camera.largura_tela = 400;
   camera.altura_tela = 300;
   glutReshapeWindow(camera.largura_tela, camera.altura_tela);

   glutDisplayFunc(ManipularExibicao);
   glutReshapeFunc(ManipularRemodelacao);
   glutVisibilityFunc(ManipularVisibilidade);
   glutKeyboardFunc(ManipularTeclado);
   glutSpecialFunc(ManipularSetasDoTeclado);

   CriarAmbiente();
   SetarCameraParaOrigem(0);

   /* Configura o menu de lentes do óculos */
   menu_lentes = glutCreateMenu(ManipularMenuDeLentes);
   glutAddMenuEntry("Vermelho e Ciano", VERMELHO_CIANO);
   glutAddMenuEntry("Ciano e Vermelho", CIANO_VERMELHO);

   /* Configura o menu principal */
   menu_principal = glutCreateMenu(ManipularMenuPrincipal);
   glutAddSubMenu("Tipo de Lente", menu_lentes);
   glutAddMenuEntry("Sair", 9);
   glutAttachMenu(GLUT_RIGHT_BUTTON);

   /* Finalização */
   glutMainLoop();
   return(0);
}

/*
   É aqui que as configurações globais do OpenGL/GLUT são feitas,
   isto é, coisas que não vão mudar com o tempo.
*/
void CriarAmbiente(void)
{
   int num[2];

   glEnable(GL_DEPTH_TEST);
   glDisable(GL_LINE_SMOOTH);
   glDisable(GL_POINT_SMOOTH);
   glDisable(GL_POLYGON_SMOOTH);
   glDisable(GL_DITHER);
   glDisable(GL_CULL_FACE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glLineWidth(1.0);
   glPointSize(1.0);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glFrontFace(GL_CW);
   glClearColor(0.0,0.0,0.0,0.0);
   glClearAccum(0.0,0.0,0.0,0.0);
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

/*
   Esta é a manipulação básica de retorno de chamada de exibição
   Ele cria a geometria, iluminação e posição de visualização
*/
void ManipularExibicao(void)
{
   int i,j;
   XYZ r;
   double dist, ratio, radians, scale, wd2, ndfl;
   double left, right, top, bottom, near = 0.1, far = 10000;

   /* Setado para evitar cortes por proximidade */
   near = camera.distancia_focal / 5;

   /* Deriva o vetor "certo" */
   CROSSPROD(camera.vd,camera.vu,r);
   Normalizar(&r);
   r.x *= camera.separacao_dos_olhos / 2.0;
   r.y *= camera.separacao_dos_olhos / 2.0;
   r.z *= camera.separacao_dos_olhos / 2.0;

   /* Setando outros valores */
   ratio  = camera.largura_tela / (double)camera.altura_tela;
   radians = DTOR * camera.abertura_camera / 2;
   wd2     = near * tan(radians);
   ndfl    = near / camera.distancia_focal;

   /* Setando o buffer para escrita e leitura */
   glDrawBuffer(GL_BACK);
   glReadBuffer(GL_BACK);

   /* Limpar as coisas */
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glClear(GL_ACCUM_BUFFER_BIT);

   glViewport(0,0,camera.largura_tela,camera.altura_tela);

   /* Filtro do olho esquerdo */
   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   switch (tipo_lente) {
   case VERMELHO_CIANO:
      glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      break;
   case CIANO_VERMELHO:
      glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
      break;
   }

   /* Cria a projeção */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   left  = - ratio * wd2 + 0.5 * camera.separacao_dos_olhos * ndfl;
   right =   ratio * wd2 + 0.5 * camera.separacao_dos_olhos * ndfl;
   top    =   wd2;
   bottom = - wd2;
   glFrustum(left, right, bottom, top, near, far);

   /* Cria o modelo */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
             camera.vp.x - r.x + camera.vd.x,
             camera.vp.y - r.y + camera.vd.y,
             camera.vp.z - r.z + camera.vd.z,
             camera.vu.x,camera.vu.y,camera.vu.z);
   CriarMundo();
   glFlush();
   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

   /* Escreve sobre o buffer */
   glAccum(GL_LOAD, 1.0);

   glDrawBuffer(GL_BACK);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* A projeção em si */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   left  = - ratio * wd2 - 0.5 * camera.separacao_dos_olhos * ndfl;
   right =   ratio * wd2 - 0.5 * camera.separacao_dos_olhos * ndfl;
   top    =   wd2;
   bottom = - wd2;
   glFrustum(left, right, bottom, top, near, far);

   /* Filtro do olho direito */
   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   switch (tipo_lente) {
   case VERMELHO_CIANO:
      glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
      break;
   case CIANO_VERMELHO:
      glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      break;
   }

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
             camera.vp.x + r.x + camera.vd.x,
             camera.vp.y + r.y + camera.vd.y,
             camera.vp.z + r.z + camera.vd.z,
             camera.vu.x,camera.vu.y,camera.vu.z);
   CriarMundo();
   glFlush();
   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

   /* Adicionando a nova imagem e copiando o resultado de volta */
   glAccum(GL_ACCUM,1.0);
   glAccum(GL_RETURN,1.0);

   glutSwapBuffers();
}

/*
   Cria o modelo e manipula a rotação em torno do eixo y
*/
void CriarMundo(void)
{
   static double angulo_de_rotacao = 0.0;

   glPushMatrix();
   glRotatef(angulo_de_rotacao, 0.0, 1.0, 0.0);
   RenderizarBule(); // Gera a imagem 3d do bule
   glPopMatrix();
}

/*
   Cria a geometria para o bule
*/
void RenderizarBule(void)
{
   static double angulo_de_rotacao = 0.0;

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   /* Rotação de nível superior - giro */
   glPushMatrix();
   glRotatef(angulo_de_rotacao, 0.0, 1.0, 0.0);

   /* Rotação em torno do eixo de rotação */
   glPushMatrix();
   glRotatef(0.0,0.0,0.0,1.0);

   /* Renderiza um bule */
   glutSolidTeapot(3.75);

   glPopMatrix(); /* Rotação do eixo do bule */
   glPopMatrix(); /* Giro do bule */
   angulo_de_rotacao += velocidade_de_rotacao;
}

/*
   Manipular as teclas
*/
void ManipularTeclado(unsigned char key, int x, int y)
{
   switch (key) {
   case ESC:                            /* Sair */
   case 'Q':
   case 'q':
      exit(0);
      break;
   case '[':                           /* Rolar no sentido anti-horário */
      GirarCamera(0,0,-1);
      break;
   case ']':                           /* Role no sentido horário */
      GirarCamera(0,0,1);
      break;
   case 'i':                           /* Transladar a câmera para cima */
   case 'I':
      TransladarCamera(0,1);
      break;
   case 'k':                           /* Transladar a câmera para baixo */
   case 'K':
      TransladarCamera(0,-1);
      break;
   case 'j':                           /* Transladar a câmera para esquerda */
   case 'J':
      TransladarCamera(-1,0);
      break;
   case 'l':                           /* Transladar a câmera para direita */
   case 'L':
      TransladarCamera(1,0);
      break;
   }
}

/*
   Manipular as setas
*/
void ManipularSetasDoTeclado(int key, int x, int y)
{
   switch (key) {
   case GLUT_KEY_LEFT:
      GirarCamera(-1, 0, 0);
      break;
   case GLUT_KEY_RIGHT:
      GirarCamera(1, 0, 0);
      break;
   case GLUT_KEY_UP:
      GirarCamera(0, 1, 0);
      break;
   case GLUT_KEY_DOWN:
      GirarCamera(0, -1, 0);
      break;
   }
}

/*
   Gire (ix, iy) ou role (iz) a câmera em torno do ponto focal
   ix, iy, iz são sinalizadores, 0 não faz nada, +- 1 gira em direções opostas
   Atualizando corretamente todos os atributos da câmera
*/
void GirarCamera(int ix,int iy,int iz)
{
   XYZ vp,vu,vd;
   XYZ right;
   XYZ newvp,newr;
   double radius,dd,radians;
   double dx,dy,dz;

   vu = camera.vu;
   Normalizar(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalizar(&vd);
   CROSSPROD(vd,vu,right);
   Normalizar(&right);
   radians = angulo_rotacao_camera * PI / 180.0;

   /* Manipula o giro */
   if (iz != 0) {
      camera.vu.x += iz * right.x * radians;
      camera.vu.y += iz * right.y * radians;
      camera.vu.z += iz * right.z * radians;
      Normalizar(&camera.vu);
      return;
   }

   /* Distância do ponto de rotação */
   dx = camera.vp.x - camera.pr.x;
   dy = camera.vp.y - camera.pr.y;
   dz = camera.vp.z - camera.pr.z;
   radius = sqrt(dx*dx + dy*dy + dz*dz);

   /* Determina o novo ponto de vista */
   dd = radius * radians;
   newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
   newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
   newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
   Normalizar(&newvp);
   camera.vp.x = camera.pr.x + radius * newvp.x;
   camera.vp.y = camera.pr.y + radius * newvp.y;
   camera.vp.z = camera.pr.z + radius * newvp.z;

   /* Determine o novo vetor da direita */
   newr.x = camera.vp.x + right.x - camera.pr.x;
   newr.y = camera.vp.y + right.y - camera.pr.y;
   newr.z = camera.vp.z + right.z - camera.pr.z;
   Normalizar(&newr);
   newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
   newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
   newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

   camera.vd.x = camera.pr.x - camera.vp.x;
   camera.vd.y = camera.pr.y - camera.vp.y;
   camera.vd.z = camera.pr.z - camera.vp.z;
   Normalizar(&camera.vd);

   /* Determine o novo vetor ascendente */
   CROSSPROD(newr,camera.vd,camera.vu);
   Normalizar(&camera.vu);
}

/*
   Translada (pan) o ponto de visão da câmera
   Em resposta às teclas i, j, k, l
   Move também o local de rotação da câmera em paralelo
*/
void TransladarCamera(int ix,int iy)
{
   XYZ vp, vu, vd;
   XYZ right;
   XYZ newvp, newr;
   double radians, delta;

   vu = camera.vu;
   Normalizar(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalizar(&vd);
   CROSSPROD(vd,vu,right);
   Normalizar(&right);
   radians = angulo_rotacao_camera * PI / 180.0;
   delta = angulo_rotacao_camera * camera.distancia_focal / 90.0;

   camera.vp.x += iy * vu.x * delta;
   camera.vp.y += iy * vu.y * delta;
   camera.vp.z += iy * vu.z * delta;
   camera.pr.x += iy * vu.x * delta;
   camera.pr.y += iy * vu.y * delta;
   camera.pr.z += iy * vu.z * delta;

   camera.vp.x += ix * right.x * delta;
   camera.vp.y += ix * right.y * delta;
   camera.vp.z += ix * right.z * delta;
   camera.pr.x += ix * right.x * delta;
   camera.pr.y += ix * right.y * delta;
   camera.pr.z += ix * right.z * delta;
}

/*
   Manipulação do menu principal
*/
void ManipularMenuPrincipal(int whichone)
{
   switch (whichone) {
   case 9: 
      exit(0); 
      break;
   }
}

/*
   Manipulação do menu de lentes do óculos
*/
void ManipularMenuDeLentes(int whichone)
{
   tipo_lente = whichone;
}

/*
   Manipulação da visibilidade
*/
void ManipularVisibilidade(int visible)
{
   if (visible == GLUT_VISIBLE)
      ManipularCronometro(0);
   else
      ;
}

/*
   Manipulação do cronômetro
*/
void ManipularCronometro(int value)
{
   glutPostRedisplay();
   glutTimerFunc(30, ManipularCronometro, 0);
}

/*
   Manipula a remodelação/redimensionamento da janela
   Mantém uma potência de 2 para as texturas
   Mantém quadrado
*/
void ManipularRemodelacao(int w,int h)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glViewport(0,0,(GLsizei)w,(GLsizei)h);
   camera.largura_tela = w;
   camera.altura_tela = h;
}

/*
   Move a câmera para a posição inicial
   Ou para uma configuração estéreo predefinida
   Supõe-se que o modelo esteja em um cubo de 10x10x10
   Centrado na origem
*/
void SetarCameraParaOrigem(int mode)
{
   camera.abertura_camera = 60;
   camera.pr = origin;

   camera.vd.x = 1;
   camera.vd.y = 0;
   camera.vd.z = 0;

   camera.vu.x = 0;
   camera.vu.y = 1;
   camera.vu.z = 0;

   camera.vp.x = -10;
   camera.vp.y = 0;
   camera.vp.z = 0;

   switch (mode) {
   case 0:
   case 2:
   case 4:
      camera.distancia_focal = 10;
      break;
   case 1:
      camera.distancia_focal = 5;
      break;
   case 3:
      camera.distancia_focal = 15;
      break;
   }
   
   /* Configuração estéreo não estressante */
   camera.separacao_dos_olhos = camera.distancia_focal / 30.0;
   if (mode == 4)
      camera.separacao_dos_olhos = 0;
}

/*
   Normalizar um vetor
*/
void Normalizar(XYZ *p)
{
   double length;

   length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
   if (length != 0) {
      p->x /= length;
      p->y /= length;
      p->z /= length;
   } else {
      p->x = 0;
      p->y = 0;
      p->z = 0;
   }
}