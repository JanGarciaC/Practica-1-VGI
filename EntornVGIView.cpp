//******** PRACTICA VISUALITZACIÓ GRÀFICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn bàsic VS2022 MULTIFINESTRA amb OpenGL 4.6, interfície MFC i llibreries GLM
//******** Ferran Poveda, Marc Vivet, Carme Julià, Débora Gil, Enric Martí Gòdia (Setembre 2025)
// EntornVGIView.cpp: implementación de la clase CEntornVGIView
// FUNCIONS:		- Control del bucle principal (OnPaint)
//					- Control teclat (OnKeyDown)
//					- Control mouse interactiu i botons mouse 
//							(OnLButtomDown, OnRButtomDown, OnMouseMove)
//					- Control opcions de menú (On*, OnUpdate*)
//					- Control de color de fons per teclat (FONS)
//					- Transformacions Geomètriques Interactives via mouse
//
//    Versió 2.0:	- Canvi de color de l'objecte per teclat (OBJ)
//					- Fixar Transformacions Geomètriques desactivant opcions Transforma (INSERT dins opcions Transforma)
//					- Esborrar Transformacions Geomètriques fixades (DELETE dins opcions Transforma)
//					- Fixar Traslació pantalla fora de l'opció Vista->Pan? (INSERT dins opció Vista->Pan?)
//					- Esborrar Traslació pantalla fixada (DELETE dins opció Vista->Pan?)
//
//	  Versió 2.2:	- Opció VISTA --> Satèl.lit: Punt de Vista en moviment segons moviment mouse
//
//	  Versió 2.5:	- Afegits objectes cubRGB i Tie (nau Star Wars fet per alumnes)
//					- Opció VISTA --> FullScreen?: Opció FullScreen per menú i per PopUp menu enmig pantalla. Té un bug al restaurar pantalla 
//													normal (inclos menú IDR_MENU_WINDOW)

#include "stdafx.h"

// Se pueden definir SHARED_HANDLERS en un proyecto ATL implementando controladores de vista previa, miniatura
// y filtro de búsqueda, y permiten compartir código de documentos con ese proyecto.
#ifndef SHARED_HANDLERS
#include "EntornVGI.h"
#endif

#include "EntornVGIDoc.h"
#include "EntornVGIView.h"
#include "visualitzacio.h"	// Include funcions de projeció i il.luminació
#include "escena.h"			// Include funcions d'objectes OpenGL

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntornVGIView

IMPLEMENT_DYNCREATE(CEntornVGIView, CView)

BEGIN_MESSAGE_MAP(CEntornVGIView, CView)
	// Comandos de impresión estándar
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEntornVGIView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_ARXIU_OBRIR_FRACTAL, &CEntornVGIView::OnArxiuObrirFractal)
	ON_COMMAND(ID_ARXIU_OBRIR_FITXER_OBJ, &CEntornVGIView::OnArxiuObrirFitxerObj)
	ON_COMMAND(ID_VISTA_Mobil, &CEntornVGIView::OnVistaMobil)
	ON_UPDATE_COMMAND_UI(ID_VISTA_Mobil, &CEntornVGIView::OnUpdateVistaMobil)
	ON_COMMAND(ID_VISTA_ZOOM, &CEntornVGIView::OnVistaZoom)
	ON_UPDATE_COMMAND_UI(ID_VISTA_ZOOM, &CEntornVGIView::OnUpdateVistaZoom)
	ON_COMMAND(ID_VISTA_POLARS_X, &CEntornVGIView::OnVistaPolarsX)
	ON_UPDATE_COMMAND_UI(ID_VISTA_POLARS_X, &CEntornVGIView::OnUpdateVistaPolarsX)
	ON_COMMAND(ID_VISTA_POLARS_Y, &CEntornVGIView::OnVistaPolarsY)
	ON_UPDATE_COMMAND_UI(ID_VISTA_POLARS_Y, &CEntornVGIView::OnUpdateVistaPolarsY)
	ON_COMMAND(ID_VISTA_POLARS_Z, &CEntornVGIView::OnVistaPolarsZ)
	ON_UPDATE_COMMAND_UI(ID_VISTA_POLARS_Z, &CEntornVGIView::OnUpdateVistaPolarsZ)
	ON_COMMAND(ID_VISTA_PAN, &CEntornVGIView::OnVistaPan)
	ON_UPDATE_COMMAND_UI(ID_VISTA_PAN, &CEntornVGIView::OnUpdateVistaPan)
	ON_COMMAND(ID_VISTA_ORIGENPAN, &CEntornVGIView::OnVistaOrigenpan)
	ON_COMMAND(ID_VISTA_NAVEGA, &CEntornVGIView::OnVistaNavega)
	ON_UPDATE_COMMAND_UI(ID_VISTA_NAVEGA, &CEntornVGIView::OnUpdateVistaNavega)
	ON_COMMAND(ID_VISTA_ORIGENNAVEGA, &CEntornVGIView::OnVistaOrigennavega)
	ON_COMMAND(ID_VISTA_EIXOS, &CEntornVGIView::OnVistaEixos)
	ON_UPDATE_COMMAND_UI(ID_VISTA_EIXOS, &CEntornVGIView::OnUpdateVistaEixos)
	ON_COMMAND(ID_PROJECCIO_PERSPECTIVA, &CEntornVGIView::OnProjeccioPerspectiva)
	ON_UPDATE_COMMAND_UI(ID_PROJECCIO_PERSPECTIVA, &CEntornVGIView::OnUpdateProjeccioPerspectiva)
	ON_COMMAND(ID_OBJECTE_CUB, &CEntornVGIView::OnObjecteCub)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CUB, &CEntornVGIView::OnUpdateObjecteCub)
	ON_COMMAND(ID_OBJECTE_ESFERA, &CEntornVGIView::OnObjecteEsfera)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_ESFERA, &CEntornVGIView::OnUpdateObjecteEsfera)
	ON_COMMAND(ID_OBJECTE_TETERA, &CEntornVGIView::OnObjecteTetera)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_TETERA, &CEntornVGIView::OnUpdateObjecteTetera)
	ON_COMMAND(ID_TRANSFORMA_TRASLACIO, &CEntornVGIView::OnTransformaTraslacio)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_TRASLACIO, &CEntornVGIView::OnUpdateTransformaTraslacio)
	ON_COMMAND(ID_TRANSFORMA_ORIGENTRASLACIO, &CEntornVGIView::OnTransformaOrigentraslacio)
	ON_COMMAND(ID_TRANSFORMA_ROTACIO, &CEntornVGIView::OnTransformaRotacio)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_ROTACIO, &CEntornVGIView::OnUpdateTransformaRotacio)
	ON_COMMAND(ID_TRANSFORMA_ORIGENROTACIO, &CEntornVGIView::OnTransformaOrigenrotacio)
	ON_COMMAND(ID_TRANSFORMA_ESCALAT, &CEntornVGIView::OnTransformaEscalat)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_ESCALAT, &CEntornVGIView::OnUpdateTransformaEscalat)
	ON_COMMAND(ID_TRANSFORMA_ORIGENESCALAT, &CEntornVGIView::OnTransformaOrigenescalat)
	ON_COMMAND(ID_TRANSFORMA_MOBILX, &CEntornVGIView::OnTransformaMobilx)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBILX, &CEntornVGIView::OnUpdateTransformaMobilx)
	ON_COMMAND(ID_TRANSFORMA_MOBILY, &CEntornVGIView::OnTransformaMobily)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBILY, &CEntornVGIView::OnUpdateTransformaMobily)
	ON_COMMAND(ID_TRANSFORMA_MOBILZ, &CEntornVGIView::OnTransformaMobilz)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBILZ, &CEntornVGIView::OnUpdateTransformaMobilz)
	ON_COMMAND(ID_OCULTACIONS_TESTVIS, &CEntornVGIView::OnOcultacionsTestvis)
	ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_TESTVIS, &CEntornVGIView::OnUpdateOcultacionsTestvis)
	ON_COMMAND(ID_OCULTACIONS_ZBUFFER, &CEntornVGIView::OnOcultacionsZbuffer)
	ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_ZBUFFER, &CEntornVGIView::OnUpdateOcultacionsZbuffer)
	ON_COMMAND(ID_ILUMINACIO_LLUMFIXE, &CEntornVGIView::OnIluminacioLlumfixe)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_LLUMFIXE, &CEntornVGIView::OnUpdateIluminacioLlumfixe)
	ON_COMMAND(ID_ILUMINACIO_FILFERROS, &CEntornVGIView::OnIluminacioFilferros)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_FILFERROS, &CEntornVGIView::OnUpdateIluminacioFilferros)
	ON_COMMAND(ID_ILUMINACIO_GOURAUD, &CEntornVGIView::OnIluminacioGouraud)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_GOURAUD, &CEntornVGIView::OnUpdateIluminacioGouraud)
	ON_COMMAND(ID_MATERIAL_EMISSIO, &CEntornVGIView::OnMaterialEmissio)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_EMISSIO, &CEntornVGIView::OnUpdateMaterialEmissio)
	ON_COMMAND(ID_MATERIAL_AMBIENT, &CEntornVGIView::OnMaterialAmbient)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_AMBIENT, &CEntornVGIView::OnUpdateMaterialAmbient)
	ON_COMMAND(ID_MATERIAL_DIFUSA, &CEntornVGIView::OnMaterialDifusa)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_DIFUSA, &CEntornVGIView::OnUpdateMaterialDifusa)
	ON_COMMAND(ID_MATERIAL_ESPECULAR, &CEntornVGIView::OnMaterialEspecular)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_ESPECULAR, &CEntornVGIView::OnUpdateMaterialEspecular)
	ON_COMMAND(ID_ILUMINACIO_TEXTURES, &CEntornVGIView::OnIluminacioTextures)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_TEXTURES, &CEntornVGIView::OnUpdateIluminacioTextures)
	ON_COMMAND(ID_ILUMINACIO_TEXTURA_FITXERIMATGE, &CEntornVGIView::OnIluminacioTexturaFitxerimatge)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_TEXTURA_FITXERIMATGE, &CEntornVGIView::OnUpdateIluminacioTexturaFitxerimatge)
	ON_COMMAND(ID_LLUMS_LLUMAMBIENT, &CEntornVGIView::OnLlumsLlumambient)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUMAMBIENT, &CEntornVGIView::OnUpdateLlumsLlumambient)
	ON_COMMAND(ID_LLUMS_LLUM0, &CEntornVGIView::OnLlumsLlum0)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM0, &CEntornVGIView::OnUpdateLlumsLlum0)
	ON_COMMAND(ID_VISTA_SATELIT, &CEntornVGIView::OnVistaSatelit)
	ON_UPDATE_COMMAND_UI(ID_VISTA_SATELIT, &CEntornVGIView::OnUpdateVistaSatelit)
	ON_COMMAND(ID_VISTA_FULLSCREEN, &CEntornVGIView::OnVistaFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VISTA_FULLSCREEN, &CEntornVGIView::OnUpdateVistaFullscreen)
	ON_COMMAND(ID_OBJECTE_TIE, &CEntornVGIView::OnObjecteTie)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_TIE, &CEntornVGIView::OnUpdateObjecteTie)
	ON_COMMAND(ID_ILUMINACIO_PUNTS, &CEntornVGIView::OnIluminacioPunts)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_PUNTS, &CEntornVGIView::OnUpdateIluminacioPunts)
	ON_COMMAND(ID_SHADER_LOAD_FILES, &CEntornVGIView::OnShaderLoadFiles)
	ON_UPDATE_COMMAND_UI(ID_SHADER_LOAD_FILES, &CEntornVGIView::OnUpdateShaderLoadFiles)
	ON_COMMAND(ID_OCULTACIONS_FRONTFACES, &CEntornVGIView::OnOcultacionsFrontFaces)
	ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_FRONTFACES, &CEntornVGIView::OnUpdateOcultacionsFrontFaces)
	ON_COMMAND(ID_LLUMS_LLUM1, &CEntornVGIView::OnLlumsLlum1)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM1, &CEntornVGIView::OnUpdateLlumsLlum1)
	ON_COMMAND(ID_LLUMS_LLUM2, &CEntornVGIView::OnLlumsLlum2)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM2, &CEntornVGIView::OnUpdateLlumsLlum2)
	ON_COMMAND(ID_LLUMS_LLUM3, &CEntornVGIView::OnLlumsLlum3)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM3, &CEntornVGIView::OnUpdateLlumsLlum3)
	ON_COMMAND(ID_LLUMS_LLUM4, &CEntornVGIView::OnLlumsLlum4)
	ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM4, &CEntornVGIView::OnUpdateLlumsLlum4)
	ON_COMMAND(ID_OBJETE_CORBA_BEZIER, &CEntornVGIView::OnObjeteCorbaBezier)
	ON_UPDATE_COMMAND_UI(ID_OBJETE_CORBA_BEZIER, &CEntornVGIView::OnUpdateObjeteCorbaBezier)
	ON_COMMAND(ID_OBJECTE_CORBA_LEMNISCATA, &CEntornVGIView::OnObjecteCorbaLemniscata)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_LEMNISCATA, &CEntornVGIView::OnUpdateObjecteCorbaLemniscata)
	ON_COMMAND(ID_OBJECTE_CORBA_BSPLINE, &CEntornVGIView::OnObjecteCorbaBSpline)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_BSPLINE, &CEntornVGIView::OnUpdateObjecteCorbaBSpline)
	ON_COMMAND(ID_OBJECTE_PUNTS_CONTROL, &CEntornVGIView::OnObjectePuntsControl)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_PUNTS_CONTROL, &CEntornVGIView::OnUpdateObjectePuntsControl)
	ON_COMMAND(ID_ILUMINACIO2SIDES, &CEntornVGIView::OnIluminacio2Sides)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO2SIDES, &CEntornVGIView::OnUpdateIluminacio2Sides)
	ON_COMMAND(ID_OBJECTE_MATRIU_PRIMITIVES, &CEntornVGIView::OnObjecteMatriuPrimitives)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_MATRIU_PRIMITIVES, &CEntornVGIView::OnUpdateObjecteMatriuPrimitives)
	ON_COMMAND(ID_OBJECTE_MATRIU_PRIMITIVES_VBO, &CEntornVGIView::OnObjecteMatriuPrimitivesVBO)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_MATRIU_PRIMITIVES_VBO, &CEntornVGIView::OnUpdateObjecteMatriuPrimitivesVBO)
	ON_COMMAND(ID_VISTA_SKYBOX, &CEntornVGIView::OnVistaSkyBox)
	ON_UPDATE_COMMAND_UI(ID_VISTA_SKYBOX, &CEntornVGIView::OnUpdateVistaSkyBox)
	ON_COMMAND(ID_ILUMINACIO_PHONG, &CEntornVGIView::OnIluminacioPhong)
	ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_PHONG, &CEntornVGIView::OnUpdateIluminacioPhong)
	ON_COMMAND(ID_OBJECTE_ARC, &CEntornVGIView::OnObjecteArc)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_ARC, &CEntornVGIView::OnUpdateObjecteArc)
		ON_COMMAND(ID_CAMERA_ESFERICA, &CEntornVGIView::OnCameraEsferica)
		ON_UPDATE_COMMAND_UI(ID_CAMERA_ESFERICA, &CEntornVGIView::OnUpdateCameraEsferica)
		ON_COMMAND(ID_VISTA_ZOOM_ORTO, &CEntornVGIView::OnVistaZoomOrto)
		ON_UPDATE_COMMAND_UI(ID_VISTA_ZOOM_ORTO, &CEntornVGIView::OnUpdateVistaZoomOrto)
		ON_COMMAND(ID_CAMERA_GEODE, &CEntornVGIView::OnCameraGeode)
		ON_UPDATE_COMMAND_UI(ID_CAMERA_GEODE, &CEntornVGIView::OnUpdateCameraGeode)
		ON_COMMAND(ID_CAMERA_ORIGEN_GEODE, &CEntornVGIView::OnCameraOrigenGeode)
		ON_COMMAND(ID_CORBES_TRIEDREFRENET, &CEntornVGIView::OnCorbesTriedreFrenet)
		ON_UPDATE_COMMAND_UI(ID_CORBES_TRIEDREFRENET, &CEntornVGIView::OnUpdateCorbesTriedreFrenet)
		ON_COMMAND(ID_OBJECTE_CUB_RGB, &CEntornVGIView::OnObjecteCubRGB)
		ON_UPDATE_COMMAND_UI(ID_OBJECTE_CUB_RGB, &CEntornVGIView::OnUpdateObjecteCubRGB)
		ON_COMMAND(ID_ILUMINACIO_TEXTURA_FLAGINVERTY, &CEntornVGIView::OnIluminacioTexturaFlagInvertY)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_TEXTURA_FLAGINVERTY, &CEntornVGIView::OnUpdateIluminacioTexturaFlagInvertY)
		ON_COMMAND(ID_MATERIAL_REFLMATERIAL, &CEntornVGIView::OnMaterialReflmaterial)
		ON_UPDATE_COMMAND_UI(ID_MATERIAL_REFLMATERIAL, &CEntornVGIView::OnUpdateMaterialReflMaterial)
		ON_COMMAND(ID_ARXIU_OBRIR_FITXER_FONT_LLUM, &CEntornVGIView::OnArxiuObrirFitxerFontLlum)
		ON_COMMAND(ID_OBJECTE_CAP, &CEntornVGIView::OnObjecteCap)
		ON_UPDATE_COMMAND_UI(ID_OBJECTE_CAP, &CEntornVGIView::OnUpdateObjecteCap)
		ON_COMMAND(ID_ARXIU_OBRIRSKYBOX, &CEntornVGIView::OnArxiuObrirSkybox)
		ON_COMMAND(ID_SHADER_PBINARY_WRITE, &CEntornVGIView::OnShaderPBinaryWrite)
		ON_COMMAND(ID_SHADER_PBINARY_READ, &CEntornVGIView::OnShaderPBinaryRead)
		ON_UPDATE_COMMAND_UI(ID_SHADER_PBINARY_READ, &CEntornVGIView::OnUpdateShaderPBinaryRead)
		ON_COMMAND(ID_SHADERS_FLAT, &CEntornVGIView::OnShadersFlat)
		ON_UPDATE_COMMAND_UI(ID_SHADERS_FLAT, &CEntornVGIView::OnUpdateShadersFlat)
		ON_COMMAND(ID_ILUMINACIO_SUAU, &CEntornVGIView::OnIluminacioSuau)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_SUAU, &CEntornVGIView::OnUpdateIluminacioSuau)
		ON_COMMAND(ID_ILUMINACIO_PLANA, &CEntornVGIView::OnIluminacioPlana)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_PLANA, &CEntornVGIView::OnUpdateIluminacioPlana)
		ON_COMMAND(ID_OBJECTE_CORBA_HERMITTE, &CEntornVGIView::OnObjecteCorbaHermitte)
		ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_HERMITTE, &CEntornVGIView::OnUpdateObjecteCorbaHermitte)
		ON_COMMAND(ID_CORBES_CATMULLROM, &CEntornVGIView::OnObjecteCorbaCatmullRom)
		ON_UPDATE_COMMAND_UI(ID_CORBES_CATMULLROM, &CEntornVGIView::OnUpdateObjecteCorbaCatmullRom)
		ON_COMMAND(ID_LLUMS_LLUM5, &CEntornVGIView::OnLlumsLlum5)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM5, &CEntornVGIView::OnUpdateLlumsLlum5)
		ON_COMMAND(ID_LLUMS_LLUM6, &CEntornVGIView::OnLlumsLlum6)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM6, &CEntornVGIView::OnUpdateLlumsLlum6)
		ON_COMMAND(ID_LLUMS_LLUM7, &CEntornVGIView::OnLlumsLlum7)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM7, &CEntornVGIView::OnUpdateLlumsLlum7)
//		ON_COMMAND(ID_PROJECCIO_ORTOGRAFICA, &CEntornVGIView::controlador)
ON_COMMAND(ID_PROJECCIO_ORTOGRAFICA, &CEntornVGIView::OnProjeccioOrtografica)
ON_UPDATE_COMMAND_UI(ID_PROJECCIO_ORTOGRAFICA, &CEntornVGIView::OnUpdateProjeccioOrtografica)
ON_COMMAND(ID_PROJECCIO_AXONOMETRICA, &CEntornVGIView::OnProjeccioAxonometrica)
ON_UPDATE_COMMAND_UI(ID_PROJECCIO_AXONOMETRICA, &CEntornVGIView::OnUpdateProjeccioAxonometrica)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Construcción o destrucción de CEntornVGIView

CEntornVGIView::CEntornVGIView()
{
// TODO: agregar aquí el código de construcción
//	int i = 0;

//------ Entorn VGI: Inicialització de les variables globals de CEntornVGIView
	int i;

// Entorn VGI: Variables de control per Menú Càmera: Esfèrica, Navega, Mòbil, Zoom, Satelit, Polars... 
	camera = CAM_ESFERICA;
	mobil = true;	zzoom = true;		zzoomO = false;		satelit = false;

// Entorn VGI: Variables de control de l'opció Càmera->Navega?
	n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
	opvN.x = 10.0;	opvN.y = 0.0;		opvN.z = 0.0;
	angleZ = 0.0;
	ViewMatrix = glm::mat4(1.0);		// Inicialitzar a identitat

// Entorn VGI: Variables de control de l'opció Càmera->Geode?
	OPV_G.R = 15.0;		OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;	// Origen PV en esfèriques per a Vista_Geode

// Entorn VGI: Variables de control per Menú Vista: Pantalla Completa, Pan, dibuixar eixos i grids
	fullscreen = false;
	pan = false;	
	eixos = true;	eixos_programID = 0;  eixos_Id = 0;
	sw_grid = false;	
	grid.x = false;	grid.y = false;		grid.z = false;		grid.w = false;
	hgrid.x = 0.0;	hgrid.y = 0.0;		hgrid.z = 0.0;		hgrid.w = 0.0;

// Entorn VGI: Variables opció Vista->Pan
	fact_pan = 1;
	tr_cpv.x = 0;	tr_cpv.y = 0;	tr_cpv.z = 0;		tr_cpvF.x = 0;	tr_cpvF.y = 0;	tr_cpvF.z = 0;

// Entorn VGI: Variables de control per les opcions de menú Projecció, Objecte
	projeccio = CAP;	// projeccio = PERSPECT;
	ProjectionMatrix = glm::mat4(1.0);	// Inicialitzar a identitat
	objecte = CAP;		// objecte = TETERA;

// Entorn VGI: Variables de control Skybox Cube
	SkyBoxCube = false;		skC_programID = 0;		
	skC_VAOID.vaoId = 0;	skC_VAOID.vboId = 0;	skC_VAOID.nVertexs = 0;	
	cubemapTexture = 0;

// Entorn VGI: Variables de control del menú Transforma
	transf = false;		trasl = false;		rota = false;		escal = false;
	fact_Tras = 1;		fact_Rota = 90;
	TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0;	TGF.VTras.x = 0.0;	TGF.VTras.y = 0.0;	TGF.VTras.z = 0;
	TG.VRota.x = 0;		TG.VRota.y = 0;		TG.VRota.z = 0;	TGF.VRota.x = 0;	TGF.VRota.y = 0;	TGF.VRota.z = 0;
	TG.VScal.x = 1;		TG.VScal.y = 1;		TG.VScal.z = 1;	TGF.VScal.x = 1;	TGF.VScal.y = 1;	TGF.VScal.z = 1;

	transX = false;	transY = false;	transZ = false;
	GTMatrix = glm::mat4(1.0);		// Inicialitzar a identitat

// Entorn VGI: Variables de control per les opcions de menú Ocultacions
	front_faces = true;	test_vis = false;	oculta = false;	

// Entorn VGI: Variables de control del menú Iluminació		
	ilumina = FILFERROS;			ifixe = false;					ilum2sides = false;
// Reflexions actives: Ambient [1], Difusa [2] i Especular [3]. No actives: Emission [0]. 
	sw_material[0] = false;			sw_material[1] = true;			sw_material[2] = true;			sw_material[3] = true;	sw_material[4] = true;
	sw_material_old[0] = false;		sw_material_old[1] = true;		sw_material_old[2] = true;		sw_material_old[3] = true;	sw_material_old[4] = true;
	textura = false;				t_textura = CAP;				textura_map = true;
	for (i = 0; i < NUM_MAX_TEXTURES; i++) texturesID[i] = 0;
	tFlag_invert_Y = false;

// ----------------- Entorn VGI: Variables de control del menú Llums
// Entorn VGI: Inicialització variables Llums
	llum_ambient = true;		llumGL[0].encesa = true;
	for (i = 1; i<NUM_MAX_LLUMS; i++) llumGL[i].encesa = false;
	for (i = 0; i<NUM_MAX_LLUMS; i++) {
		llumGL[i].posicio.x = 0.0;		llumGL[i].posicio.y = 0.0;		llumGL[i].posicio.z = 0.0;		llumGL[i].posicio.w = 1.0;
		llumGL[i].difusa.r = 1.0f;		llumGL[i].difusa.g = 1.0f;		llumGL[i].difusa.b = 1.0f;		llumGL[i].difusa.a = 1.0f;
		llumGL[i].especular.r = 1.0f;	llumGL[i].especular.g = 1.0f;	llumGL[i].especular.b = 1.0f;	llumGL[i].especular.a = 1.0f;
		llumGL[i].atenuacio.a = 0.0;	llumGL[i].atenuacio.b = 0.0;	llumGL[i].atenuacio.c = 1.0;
		llumGL[i].restringida = false;
		llumGL[i].spotdirection.x = 0.0;	llumGL[i].spotdirection.y = 0.0;	llumGL[i].spotdirection.z = 0.0; llumGL[i].spotdirection.w = 0.0;
		llumGL[i].spotcoscutoff = 0.0;		llumGL[i].spotexponent = 0.0;
		}

// ---------------- LLUM #0 - (+Z) no restringida, amb atenuació constant (a,b,c) = (0,0,1)
// Posició de la font de llum (x,y,z)=(0,200,0):
	llumGL[0].posicio.x = 0.0;			llumGL[0].posicio.y = 0.0;			llumGL[0].posicio.z = 200.0;	llumGL[0].posicio.w = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[0].difusa.r = 1.0f;			llumGL[0].difusa.g = 1.0f;			llumGL[0].difusa.b = 1.0f;		llumGL[0].difusa.a = 1.0f;
	llumGL[0].especular.r = 1.0f;		llumGL[0].especular.g = 1.0f;		llumGL[0].especular.b = 1.0f;	llumGL[0].especular.a = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[0].atenuacio.a = 0.0;		llumGL[0].atenuacio.b = 0.0;		llumGL[0].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[0].restringida = false;
	llumGL[0].spotdirection.x = 0.0;	llumGL[0].spotdirection.y = 0.0;	llumGL[0].spotdirection.z = -1.0;
	llumGL[0].spotcoscutoff = cos(25.0*PI/180);		llumGL[0].spotexponent = 1.0;		// llumGL[0].spotexponent = 45.0; Model de Warn (10, 500)

// Activació font de llum: ENCESA
	llumGL[0].encesa = true;

// ---------------- LLUM #1 - (+X) no restringida, amb atenuació constant (a,b,c) = (0,0,1)
// Posició de la font de llum (x,y,z)=(75,0,0):
	llumGL[1].posicio.x = 75.0;			llumGL[1].posicio.y = 0.0;			llumGL[1].posicio.z = 0.0;		llumGL[1].posicio.w = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[1].difusa.r = 1.0f;			llumGL[1].difusa.g = 1.0f;			llumGL[1].difusa.b = 1.0f;		llumGL[1].difusa.a = 1.0f;
	llumGL[1].especular.r = 1.0f;		llumGL[1].especular.g = 1.0f;		llumGL[1].especular.b= 1.0f;	llumGL[1].especular.a = 1.0f;

	// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[1].atenuacio.a = 0.0;		llumGL[1].atenuacio.b = 0.0;		llumGL[1].atenuacio.c = 1.0;
	
// Paràmetres font de llum restringida:
	llumGL[1].restringida = false;
	llumGL[1].spotdirection.x = 0.0;	llumGL[1].spotdirection.y = 0.0;	llumGL[1].spotdirection.z = 0.0;
	llumGL[1].spotcoscutoff = 0.0;		llumGL[1].spotexponent = 0.0;

// Activació font de llum: APAGADA
	llumGL[1].encesa = false;

// ---------------- LLUM #2 - (+Y) no restringida, amb atenuació constant (a,b,c) = (0,0,1)
// Posició de la font de llum (x,y,z)=(0,75,0):
	llumGL[2].posicio.x = 0.0;			llumGL[2].posicio.y = 75.0;			llumGL[2].posicio.z = 0.0;		llumGL[2].posicio.w = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[2].difusa.r = 1.0f;			llumGL[2].difusa.g = 1.0f;			llumGL[2].difusa.b = 1.0f;		llumGL[2].difusa.a = 1.0f;
	llumGL[2].especular.r = 1.0f;		llumGL[2].especular.b = 1.0f;		llumGL[2].especular.b = 1.0f;	llumGL[2].especular.a = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum amb atenuació per distància (a,b,c)=(0,0.025,1):
	llumGL[2].atenuacio.a = 0.0;		llumGL[2].atenuacio.b = 0.0;		llumGL[2].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[2].restringida = false;
	llumGL[2].spotdirection.x = 0.0;	llumGL[2].spotdirection.y = -1.0;	llumGL[2].spotdirection.z = 0.0;
	llumGL[2].spotcoscutoff = cos(2.5 * PI / 180);							llumGL[2].spotexponent = 1.0;

// Activació font de llum: APAGADA
	llumGL[2].encesa = false;

// ---------------- LLUM #3 - (Y=X), restringida amb 25 graus obertura i exponent 45, amb atenuació constant (a,b,c) = (0,0,1)
// Posició de la font de llum (x,y,z)=(75,75,75):
	llumGL[3].posicio.x = 75.0;			llumGL[3].posicio.y = 75.0;			llumGL[3].posicio.z = 75.0;		llumGL[3].posicio.w = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (0,1,0):
	llumGL[3].difusa.r = 0.0f;			llumGL[2].difusa.g = 1.0f;			llumGL[3].difusa.b = 0.0f;		llumGL[3].difusa.a = 1.0f;
	llumGL[3].especular.r = 0.0f;		llumGL[2].especular.g = 1.0f;		llumGL[3].especular.b = 0.0f;	llumGL[3].especular.a = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[3].atenuacio.a = 0.0;		llumGL[3].atenuacio.b = 0.0;		llumGL[3].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[3].restringida = true;
	llumGL[3].spotdirection.x = -1.0;	llumGL[3].spotdirection.y = -1.0;	llumGL[3].spotdirection.z = -1.0;
	llumGL[3].spotcoscutoff = cos(25.0 * PI / 180);							llumGL[3].spotexponent = 45.0;

// Activació font de llum: APAGADA
	llumGL[3].encesa = false;

// ---------------- LLUM #4 - (-Z), no restringida, amb atenuació constant (a,b,c) = (0,0,1)
// Posició de la font de llum (x,y,z)=(0,0,-75):
	llumGL[4].posicio.x = 0.0;			llumGL[4].posicio.y = 0.0;			llumGL[4].posicio.z = -75.0;	llumGL[4].posicio.w = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[4].difusa.r = 1.0f;			llumGL[4].difusa.g = 1.0f;			llumGL[4].difusa.b = 1.0f;		llumGL[4].difusa.a = 1.0f;
	llumGL[4].especular.r = 1.0f;		llumGL[4].especular.g = 1.0f;		llumGL[4].especular.b = 1.0f;	llumGL[4].especular.a = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[4].atenuacio.a = 0.0;		llumGL[4].atenuacio.b = 0.0;		llumGL[4].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[4].restringida = false;
	llumGL[4].spotdirection.x = 0.0;	llumGL[4].spotdirection.y = 0.0;	llumGL[4].spotdirection.z = -1.0;
	llumGL[4].spotcoscutoff = cos(5 * PI / 180);							llumGL[4].spotexponent = 30.0;

// Activació font de llum: APAGADA
	llumGL[4].encesa = false;

// ---------------- LLUM #5 - (-Z), direccional, no restringida, amb atenuació constant (a,b,c) = (0,0,1)
// Vector de la font de llum direccional (x,y,z)=(-1,-1,-1):
	llumGL[5].posicio.x = -1.0;			llumGL[5].posicio.y = -1.0;			llumGL[5].posicio.z = -1.0;		llumGL[5].posicio.w = 0.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,0,1):
	llumGL[5].difusa.r = 1.0f;			llumGL[5].difusa.g = 0.0f;			llumGL[5].difusa.b = 1.0f;		llumGL[5].difusa.a = 1.0f;
	llumGL[5].especular.r = 1.0f;		llumGL[5].especular.g = 0.0f;		llumGL[5].especular.b = 1.0f;	llumGL[5].especular.a = 1.0f;
	
// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[5].atenuacio.a = 0.0;		llumGL[5].atenuacio.b = 0.0;		llumGL[5].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[5].restringida = false;
	llumGL[5].spotdirection.x = 0.0;	llumGL[5].spotdirection.y = 0.0;	llumGL[5].spotdirection.z = 0.0;
	llumGL[5].spotcoscutoff = 0.0;		llumGL[5].spotexponent = 0.0;

// Activació font de llum: APAGADA
	llumGL[5].encesa = false;

// ---------------- LLUM #6 - Llum Vaixell, configurada a la funció vaixell() en escena.cpp.
// Posició de la font de llum (x,y,z)=(-75,75,75):
	llumGL[6].posicio.x = -75.0;		llumGL[6].posicio.y = 75.0;			llumGL[6].posicio.z = 75.0;		llumGL[6].posicio.w = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,0,0):
	llumGL[6].difusa.r = 1.0f;			llumGL[6].difusa.g = 0.0f;			llumGL[6].difusa.b = 0.0f;		llumGL[6].difusa.a = 1.0f;
	llumGL[6].especular.r = 1.0f;		llumGL[6].especular.g = 0.0f;		llumGL[6].especular.b = 0.0f;	llumGL[6].especular.a = 1;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[6].atenuacio.a = 0.0;		llumGL[6].atenuacio.b = 0.0;		llumGL[6].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[6].restringida = true;
	llumGL[6].spotdirection.x = 1.0;	llumGL[6].spotdirection.y = -1.0;	llumGL[6].spotdirection.z = -1.0;
	llumGL[6].spotcoscutoff = sqrt(2.0) / 2.0;		llumGL[6].spotexponent = 10.0;

// Activació font de llum: APAGADA
	llumGL[6].encesa = false;

// ---------------- LLUM #7 - Llum Far, configurada a la funció faro() en escena.cpp.
// Posició de la font de llum (x,y,z)=(50,50,-50).
	llumGL[7].posicio.x = 50.0;			llumGL[7].posicio.y = 50.0;			llumGL[7].posicio.z = -50.0;	llumGL[7].posicio.w = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (0,1,1):
	llumGL[7].difusa.r = 0.0f;			llumGL[7].difusa.g = 1.0f;			llumGL[7].difusa.b = 1.0f;		llumGL[7].difusa.a = 1.0f;
	llumGL[7].especular.r = 0.0f;		llumGL[7].especular.g = 1.0f;		llumGL[7].especular.b = 1.0f;	llumGL[7].especular.a = 1;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[7].atenuacio.a = 0.0;		llumGL[7].atenuacio.b = 0.0;		llumGL[7].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[7].restringida = true;
	llumGL[7].spotdirection.x = -1.0;	llumGL[7].spotdirection.y = -1.0;	llumGL[7].spotdirection.z = 1.0;
	llumGL[7].spotcoscutoff = 0.5;		llumGL[7].spotexponent = 5.0;

// Activació font de llum: APAGADA
	llumGL[7].encesa = false;
// ---------------- FI DEFINICIÓ LLUMS

// Entorn VGI: Variables de control del menú Shaders		
	shader = CAP_SHADER;		shader_programID = 0;

// Entorn VGI: Variables de control dels botons de mouse
	m_PosEAvall = (0, 0);		m_PosDAvall = (0, 0);
	m_ButoEAvall = false;		m_ButoDAvall = false;
	m_EsfeEAvall.R = 0.0;		m_EsfeEAvall.alfa = 0.0;	m_EsfeEAvall.beta = 0.0;
	m_EsfeIncEAvall.R = 0.0;	m_EsfeIncEAvall.alfa = 0.0;	m_EsfeIncEAvall.beta = 0.0;

// Entorn VGI: Variables que controlen paràmetres visualització: Mides finestra Windows i PV
	w = 0;				h = 0;								// Mides finestra
	w_old = 0;			h_old = 0;							// Copia mides finestre per a FullScreen
	OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;	// Origen PV en esfèriques
	Vis_Polar = POLARZ;

// Entorn VGI: Color de fons i de l'objecte
	fonsR = true;		fonsG = true;		fonsB = true;
	c_fons.r = 0.0;		c_fons.g = 0.0;		c_fons.b = 0.0;
	sw_color = false;
	col_obj.r = 1.0;	col_obj.g = 1.0;	col_obj.b = 1.0;		col_obj.a = 1.0;

// Entorn VGI: Objecte OBJ
	ObOBJ = NULL;		vao_OBJ.vaoId = 0;		vao_OBJ.vboId = 0;		vao_OBJ.nVertexs = 0;

// VGI: OBJECTE --> Corba B-Spline i Bezier
	npts_T = 0;
	for (i = 0; i < MAX_PATCH_CORBA; i = i++)
	{	PC_t[i].x = 0.0;
		PC_t[i].y = 0.0;
		PC_t[i].z = 0.0;
	}

	pas_CS = PAS_BSPLINE;
	sw_Punts_Control = false;

// TRIEDRE DE FRENET / DARBOUX: VT: vector Tangent, VNP: Vector Normal Principal, VBN: vector BiNormal
	dibuixa_TriedreFrenet = false;
	VT = { 0.0, 0.0, 1.0 };		VNP = { 1.0, 0.0, 0.0 };	VBN = { 0.0, 1.0, 0.0 };

// Entorn VGI: Variables del Timer
	t = 0;			anima = false;

// Entorn VGI: Variables de l'objecte FRACTAL
	t_fractal = CAP;	soroll = 'C';
	pas = 64;			pas_ini = 64;
	sw_il = true;		palcolFractal = false;

// Entorn VGI: Altres variables
	mida = 1.0;			nom = "";		buffer = "";
	initVAOList();	// Inicialtzar llista de VAO'S.

// Entorn VGI: Definir desplegable per a Full Screen
	ContextMenu = new CMenu();
	if (!ContextMenu->LoadMenu(IDR_MENU_WINDOW))	AfxMessageBox(_T("Fail to create context menu"));

}

CEntornVGIView::~CEntornVGIView()
{
}

BOOL CEntornVGIView::PreCreateWindow(CREATESTRUCT& cs)
{
// TODO: modificar aquí la clase Window o los estilos cambiando
//  CREATESTRUCT cs

// An OpenGL window must be created with the following flags and must not
// include CS_PARENTDC for the class style.
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// Impresión de CEntornVGIView

void CEntornVGIView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CEntornVGIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Preparación predeterminada
	return DoPreparePrinting(pInfo);
}

void CEntornVGIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
// TODO: agregar inicialización adicional antes de imprimir
}

void CEntornVGIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
// TODO: agregar limpieza después de imprimir
}

void CEntornVGIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Diagnósticos de CEntornVGIView

#ifdef _DEBUG
void CEntornVGIView::AssertValid() const
{
	CView::AssertValid();
}

void CEntornVGIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEntornVGIDoc* CEntornVGIView::GetDocument() const // La versión de no depuración está alineada
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEntornVGIDoc)));
	return (CEntornVGIDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// Controladores de mensaje de CEntornVGIView

int CEntornVGIView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

// TODO:  Agregue aquí su código de creación especializado

	//CDC* pDC = GetDC();
	//m_glRenderer.CreateGLContext(pDC);
	m_pDC = new CClientDC(this);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

// Entorn VGI: Format del pixel que millor s'ajusti al descrit en pfd
	//int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	int nPixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);

	if (!nPixelFormat)
	{	::MessageBoxW(NULL, L"Error en SetPixelFormat", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

// Activació format pixel per al contexte dispositiu
	//BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);
	BOOL bResult = SetPixelFormat(m_pDC->GetSafeHdc(), nPixelFormat, &pfd);

	if (!bResult) 
	{	::MessageBoxW(NULL, L"Error en SetPixelFormat", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

// Entorn VGI: Creació contexte generació OpenGL
	// m_hRC=wglCreateContext(m_hDC);
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if (!m_hRC)
	{	::MessageBoxW(NULL, L"Error en GL Rendering Context", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

// OPENGL 2.0 - OPENGL 3.0 **********************************************************
	// --- OpenGL 3.x ---
	HGLRC tempContext = wglCreateContext(m_pDC->GetSafeHdc());
	wglMakeCurrent(m_pDC->GetSafeHdc(), tempContext);

//Get access to modern OpenGL functionality from this old style context.
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		AfxMessageBox(_T("GLEW could not be initialized!"));
		return FALSE;
	}

//InitAPI();

//	if (glewIsExtensionSupported("GLEW_ARB_vertex_shader"))
//	AfxMessageBox(_T("INFO: GLEW_ARB_vertex_shader supported, proceeding\n"));
//	else AfxMessageBox(_T("INFO: GLEW_ARB_vertex_shader NOT supported, proceeding\n"));
//
//	if (glewIsExtensionSupported("GLEW_ARB_fragment_shader"))
//	AfxMessageBox(_T("INFO: GLEW_ARB_fragment_shader supported, proceeding\n"));
//	else AfxMessageBox(_T("INFO: GLEW_ARB_fragment_shader NOT supported, proceeding\n"));
//
	if (glewIsSupported("GL_VERSION_2_0")) //(GLEW_VERSION_2_0)
	{	} //AfxMessageBox(_T("INFO: OpenGL 2.0 supported!. Proceed\n"));
	else
	{
		AfxMessageBox(_T("INFO: OpenGL 2.0 not supported!. Exit\n"));
		//return EXIT_FAILURE;
	}

	
	int major, minor;
	GetGLVersion(&major, &minor);

	if (major < 3 || (major == 3 && minor < 2))
		AfxMessageBox(_T("INFO: OpenGL 3.2 is not supported!. Exit\n"));
//	else 
//		AfxMessageBox(_T("OpenGL 3.2 is supported!. Proceed"));

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, //0x9126 , 0x00000001,
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB != NULL)
	{
		m_hrc = wglCreateContextAttribsARB(m_pDC->GetSafeHdc(), 0, attribs);
	}

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hrc);

// Entorn VGI: Inicialització de funcions de shaders per a OpenGL 2.0
	InitAPI();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);

	if (!m_hrc)
	{
		AfxMessageBox(_T("INFO: OpenGL 3.3+ was not created!"));
		return false;
	}

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// ------------ - Entorn VGI : Enable OpenGL debug context if context allows for DEBUG CONTEXT (GL 4.6)
	if (GLEW_VERSION_4_6)
	{
		GLint flags; 
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{	glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
			glDebugMessageCallback((GLDEBUGPROC)wglGetProcAddress("glDebugOutput"), nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
	}

// ------------ - Entorn VGI : To verify that the driver supports at least one shader binary format (GL4.6)
	GLboolean shader_compiler = 0;
	glGetBooleanv(GL_SHADER_COMPILER, &shader_compiler);
	if (shader_compiler == GL_FALSE) {	AfxMessageBox(_T("INFO: Shader compilation NOT supported"));
										//exit(EXIT_FAILURE);
									}

	GLint program_formats=0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &program_formats);
	if (program_formats <1)	{	AfxMessageBox(_T("INFO: Program binary loading NOT supported.\n"));
								//exit(EXIT_FAILURE);
							}

	GLint shader_formats = 0;
	glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &shader_formats);
	if (shader_formats < 1)	{	//AfxMessageBox(_T("INFO: Shader binary loading NOT supported.\n"));
								//exit(EXIT_FAILURE);
							}

// Entorn VGI: Desactivació el contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	return true;
}

void CEntornVGIView::InitAPI()
{
/*	Vendor, Renderer, Version, Shading Laguage Version i Extensions suportades per la placa gràfica gravades en fitxer extensions.txt
	CString nomf;
	nomf = "extensions.txt";
	char *nomfitxer = CString2Char(nomf);
	char* str = (char*)glGetString(GL_VENDOR);
	FILE* f = fopen(nomfitxer, "w");
	if(f)	{	fprintf(f,"VENDOR: %s\n",str);
				str = (char*)glGetString(GL_RENDERER);
				fprintf(f, "RENDERER: %s\n", str);
				str = (char*)glGetString(GL_VERSION);
				fprintf(f, "VERSION: %s\n", str);
				str = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
				fprintf(f, "SHADING_LANGUAGE_VERSION: %s\n", str);
				str = (char*)glGetString(GL_EXTENSIONS);
				fprintf(f, "EXTENSIONS: %s\n", str);
				fclose(f);
			}
	*/

// Program
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");

// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");

// VAO
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
}

void CEntornVGIView::GetGLVersion(int* major, int* minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"

	*major = ver[0] - '0';
	if (*major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, major);		// major = 3
		glGetIntegerv(GL_MINOR_VERSION, minor);		// minor = 2
	}
	else
	{
		*minor = ver[2] - '0';
	}

	// GLSL
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);	// 1.50 NVIDIA via Cg compiler
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	fprintf(stderr, "---------------\n");
	fprintf(stderr, "Debug message ( %3i %s \n", id, message);

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             fprintf(stderr, "Source: API \n"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   fprintf(stderr, "Source: Window System \n"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "Source: Shader Compiler \n"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     fprintf(stderr, "Source: Third Party \n"); break;
	case GL_DEBUG_SOURCE_APPLICATION:     fprintf(stderr, "Source: Application \n"); break;
	case GL_DEBUG_SOURCE_OTHER:           fprintf(stderr, "Source: Other \n"); break;
	} //std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               fprintf(stderr, "Type: Error\n"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "Type: Deprecated Behaviour\n"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  fprintf(stderr, "Type: Undefined Behaviour\n"); break;
	case GL_DEBUG_TYPE_PORTABILITY:         fprintf(stderr, "Type: Portability\n"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:         fprintf(stderr, "Type: Performance\n"); break;
	case GL_DEBUG_TYPE_MARKER:              fprintf(stderr, "Type: Marker\n"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          fprintf(stderr, "Type: Push Group\n"); break;
	case GL_DEBUG_TYPE_POP_GROUP:           fprintf(stderr, "Type: Pop Group\n"); break;
	case GL_DEBUG_TYPE_OTHER:               fprintf(stderr, "Type: Other\n"); break;
	} //std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         fprintf(stderr, "Severity: high\n"); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       fprintf(stderr, "Severity: medium\n"); break;
	case GL_DEBUG_SEVERITY_LOW:          fprintf(stderr, "Severity: low\n"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity: notification\n"); break;
	} //std::cout << std::endl;
	//std::cout << std::endl;
	fprintf(stderr, "\n");
}


void CEntornVGIView::OnDestroy()
{
	CView::OnDestroy();

// TODO: Agregue aquí su código de controlador de mensajes
	CDC* pDC = GetDC();
	ReleaseDC(pDC);
}


void CEntornVGIView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

// TODO: Agregue aquí su código de controlador de mensajes

// A resize event occured; cx and cy are the window's new width and height.
// Find the OpenGL change size function given in the Lab 1 notes and call it here

// Entorn VGI: MODIFICACIÓ ->Establim les mides de la finestra actual
	w = cx;
	h = cy;
}


void CEntornVGIView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CDC* pDC = GetDC();
	//m_glRenderer.PrepareScene(pDC);
	ReleaseDC(pDC);
}


/////////////////////////////////////////////////////////////////////////////
// Dibujo de CEntornVGIView

void CEntornVGIView::OnDraw(CDC* /*pDC*/)
{
// TODO: agregar aquí el código de dibujo para datos nativos
	CEntornVGIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


void CEntornVGIView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
// TODO: Agregue aquí su código de controlador de mensajes
	GLdouble vpv[3] = { 0.0, 0.0, 1.0 };

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

/*
// Color de l'objecte condicionat al color de fons
	if ((c_fons.r < 0.5) || (c_fons.g < 0.5) || (c_fons.b < 0.5))
		{	col_obj.r = 1.0;  col_obj.g = 1.0;  col_obj.b = 1.0; col_obj.a = 1.0;
		}
		else {	col_obj.r = 0.0;  col_obj.g = 0.0;  col_obj.b = 0.0; col_obj.a = 1.0;
			 }
*/

// Cridem a les funcions de l'escena i la projecció segons s'hagi 
// seleccionat una projecció o un altra
	switch (projeccio)
	{
	case AXONOM:
// PROJECCIÓ AXONOMÈTRICA
// Activació del retall de pantalla
		glEnable(GL_SCISSOR_TEST);

// Retall
		glScissor(0, 0, w, h);
		glViewport(0, 0, w, h);

// Aquí farem les crides per a definir Viewport, Projecció i Càmara: INICI -------------------------
		ProjectionMatrix = Projeccio_Orto(shader_programID, 0, 0, w, h);
		ViewMatrix = Vista_Ortografica(shader_programID, 3, OPV.R, c_fons, col_obj, objecte, mida, pas,
			front_faces, oculta, test_vis,
			ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
// Aquí farem les cridesper a definir Viewport, Projecció i Càmara:: FI -------------------------
		// Dibuixar Model (escena)
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	case ORTO:
		// PROJECCIÓ ORTOGRÀFICA
		// Activació del retall de pantalla
		glEnable(GL_SCISSOR_TEST);

		// Retall
		glViewport(0, 0, w, h);

		// Fons condicionat al color de fons
		if ((c_fons.r < 0.5) || (c_fons.g < 0.5) || (c_fons.b < 0.5))
			FonsB();
		else
			FonsN();

		// Entorn VGI: TO DO -> Aquí farem les quatre crides a ProjeccioOrto i Ortografica per obtenir 
		//						les quatre vistes ortogràfiques. De moment n'activem només una de prova
		//						IMPORTANT: DESCOMENTAR LA RESTA QUAN FUNCIONI LA PRIMERA
		// PLANTA (Inferior Esquerra)

				// Definició de Viewport, Projecció i Càmara
		glScissor(0, 0, w / 2, h / 2);
		ProjectionMatrix = Projeccio_Orto(shader_programID, 0, 0, w / 2, h / 2);
		ViewMatrix = Vista_Ortografica(shader_programID, 1, OPV.R, c_fons, col_obj, objecte, mida, pas,
			front_faces, oculta, test_vis,
			ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

		// ISOMÈTRICA (Inferior Dreta)
				// Definició de Viewport, Projecció i Càmara
		glScissor(w / 2, 0, w / 2, h / 2);
		ProjectionMatrix = Projeccio_Orto(shader_programID, w / 2, 0, w / 2, h / 2);
		ViewMatrix = Vista_Ortografica(shader_programID, 3, OPV.R, c_fons, col_obj, objecte, mida, pas,
			front_faces, oculta, test_vis,
			ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.


		// ALÇAT (Superior Esquerra)
				// Definició de Viewport, Projecció i Càmara
		glScissor(0, h / 2, w / 2, h / 2);
		ProjectionMatrix = Projeccio_Orto(shader_programID, 0, h / 2, w / 2, h / 2);
		ViewMatrix = Vista_Ortografica(shader_programID, 0, OPV.R, c_fons, col_obj, objecte, mida, pas,
			front_faces, oculta, test_vis,
			ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geom?triques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

		// PERFIL (Superior Dreta)

				// Definició de Viewport, Projecció i Càmara
		glScissor(w / 2, h / 2, w / 2, h / 2);
		ProjectionMatrix = Projeccio_Orto(shader_programID, w / 2, h / 2, w / 2, h / 2);
		ViewMatrix = Vista_Ortografica(shader_programID, 2, OPV.R, c_fons, col_obj, objecte, mida, pas,
			front_faces, oculta, test_vis,
			ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
			eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geom?triques segons persiana Transformacio i configurar objectes
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.


		// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;


	case PERSPECT:
// PROJECCIÓ PERSPECTIVA
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Set Perspective Calculations To Most Accurate
		glDisable(GL_SCISSOR_TEST);		// Desactivació del retall de pantalla

		// Definició de Viewport, Projecció i Càmara
		ProjectionMatrix = Projeccio_Perspectiva(shader_programID, 0, 0, w, h, OPV.R);

		// Definició de la càmera.
		if (camera == CAM_ESFERICA) {	n[0] = 0;		n[1] = 0;		n[2] = 0;
				ViewMatrix = Vista_Esferica(shader_programID, OPV, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida, pas,
				front_faces, oculta, test_vis,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
				eixos, grid, hgrid);
				}
		else if (camera == CAM_NAVEGA) {
			if (Vis_Polar == POLARZ) {	vpv[0] = 0.0;	vpv[1] = 0.0;	vpv[2] = 1.0;
									}
			else if (Vis_Polar == POLARY) {	vpv[0] = 0.0;	vpv[1] = 1.0;	vpv[2] = 0.0;
											}
			else if (Vis_Polar == POLARX) {	vpv[0] = 1.0;	vpv[1] = 0.0;	vpv[2] = 0.0;
										}
			ViewMatrix = Vista_Navega(shader_programID, opvN, n, vpv, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, true, pas,
				front_faces, oculta, test_vis,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
				eixos, grid, hgrid);
			}
		else if (camera == CAM_GEODE) {
			ViewMatrix = Vista_Geode(shader_programID, OPV_G, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida, pas,
				front_faces, oculta, test_vis,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides,
				eixos, grid, hgrid);
				}

		// Dibuix de l'Objecte o l'Escena
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.

// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	default:
// Càrrega SHADERS
// Càrrega Shader Eixos
		if (!eixos_programID) eixos_programID = shaderEixos.loadFileShaders(".\\shaders\\eixos.VERT", ".\\shaders\\eixos.FRAG");

// Càrrega Shader de Gouraud
		if (!shader_programID) {
			shader_programID = shaderLighting.loadFileShaders(".\\shaders\\gouraud_shdrML.vert", ".\\shaders\\gouraud_shdrML.frag");
			shader = GOURAUD_SHADER;
			}

// Entorn VGI: Creació de la llista que dibuixarà els eixos Coordenades Món. Funció on està codi per dibuixar eixos	
		if (!eixos_Id) eixos_Id = deixos();						// Funció que defineix els Eixos Coordenades Món com un VAO.

// Crida a la funció Fons Blanc
		FonsB();

// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;
}

// Entorn VGI: Desactivació el contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

//  Actualitzar la barra d'estat de l'aplicació amb els valors R,A,B,PVx,PVy,PVz
	Barra_Estat();
}

// configura_Escena: Funcio que configura els parametres de Model i dibuixa les
//                   primitives OpenGL dins classe Model
void CEntornVGIView::configura_Escena() 
{
// Aplicar Transformacions Geometriques segons persiana Transformacio i Quaternions
	GTMatrix = instancia(transf, TG, TGF);
}

// dibuixa_Escena: Funcio que crida al dibuix dels diferents elements de l'escana
void CEntornVGIView::dibuixa_Escena() 
{
//	Dibuix SkyBox Cúbic.
	if (SkyBoxCube) dibuixa_Skybox(skC_programID, cubemapTexture, Vis_Polar, ProjectionMatrix, ViewMatrix);

//	Dibuix Coordenades Món i Reixes.
	dibuixa_Eixos(eixos_programID, eixos, eixos_Id, grid, hgrid, ProjectionMatrix, ViewMatrix);

// Escalat d'objectes, per adequar-los a les vistes ortogràfiques (Pràctica 2)
//	GTMatrix = glm::scale();

//	Dibuix geometria de l'escena amb comandes GL.
	dibuixa_EscenaGL(shader_programID, eixos, eixos_Id, grid, hgrid, objecte, col_obj, sw_material, 
		textura, texturesID, textura_map, tFlag_invert_Y,
		npts_T, PC_t, pas_CS, sw_Punts_Control, dibuixa_TriedreFrenet, 
		ObOBJ,				// Classe de l'objecte OBJ que conté els VAO's
		ViewMatrix, GTMatrix);
}

// Barra_Estat: Actualitza la barra d'estat (Status Bar) de l'aplicació amb els
//      valors R,A,B,PVx,PVy,PVz en Visualització Interactiva.
void CEntornVGIView::Barra_Estat()
{
	CString sss;
	CEsfe3D OPVAux = { 0.0,0.0,0.0 };
	double PVx, PVy, PVz;
	//GLdouble color;

// Status Bar fitxer fractal
	if (nom != "") GetStatusBar().SetPaneText(0, nom);

// Càlcul dels valors per l'opció Vista->Navega
	if (projeccio != CAP && projeccio != ORTO) {
		if (camera == CAM_ESFERICA)
			{	// Càmera Esfèrica
				OPVAux.R = OPV.R; OPVAux.alfa = OPV.alfa; OPVAux.beta = OPV.beta;
			}
		else if (camera == CAM_NAVEGA)
		{	// Càmera Navega
			OPVAux.R = sqrt(opvN.x*opvN.x + opvN.y*opvN.y + opvN.z*opvN.z);
			OPVAux.alfa = (asin(opvN.z / OPVAux.R) * 180) / PI;
			OPVAux.beta = (atan(opvN.y / opvN.x)) * 180 / PI;
		}
		else {	// Càmera Geode
			OPVAux.R = OPV_G.R; OPVAux.alfa = OPV_G.alfa; OPVAux.beta = OPV_G.beta; 
		}
	}
	else {
		OPVAux.R = OPV.R; OPVAux.alfa = OPV.alfa; OPVAux.beta = OPV.beta;
		}

// Status Bar R Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";	
		else if (projeccio==ORTO) buffer=" ORTO   ";
			else if (camera == CAM_NAVEGA) buffer = " NAV   ";
			else buffer.Format(_T("%.1f"), OPVAux.R);
	sss = _T("R=") + buffer;
// Refrescar posició R Status Bar
	GetStatusBar().SetPaneText(1, sss);

// Status Bar angle alfa Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
		else if (projeccio==ORTO) buffer="ORTO   ";
			else if (camera == CAM_NAVEGA) buffer = " NAV   ";
				else buffer.Format(_T("%.1f"), OPVAux.alfa);
	sss = _T("a=") + buffer;
// Refrescar posició angleh Status Bar
	GetStatusBar().SetPaneText(2, sss);

// Status Bar angle beta Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
		else if (projeccio==ORTO) buffer="ORTO   ";
			else if (camera == CAM_NAVEGA) buffer = " NAV   ";
				else buffer.Format(_T("%.1f"), OPVAux.beta);
	sss = _T("ß=") + buffer;
// Refrescar posició anglev Status Bar
	GetStatusBar().SetPaneText(3, sss);

// Transformació PV de Coord. esfèriques (R,anglev,angleh) --> Coord. cartesianes (PVx,PVy,PVz)
	if (camera == CAM_NAVEGA) { PVx = opvN.x; PVy = opvN.y; PVz = opvN.z; }
	else {
		if (Vis_Polar == POLARZ) {
			PVx = OPVAux.R * cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVy = OPVAux.R * sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVz = OPVAux.R * sin(OPVAux.alfa * PI / 180);
			if (camera == CAM_GEODE)
				{	// Vector camp on mira (cap a (R,alfa,beta)
					PVx = PVx + cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
					PVy = PVy + sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
					PVz = PVz + sin(OPVAux.alfa * PI / 180);
				}
			}
		else if (Vis_Polar == POLARY) {
			PVx = OPVAux.R*sin(OPVAux.beta*PI / 180)*cos(OPVAux.alfa*PI / 180);
			PVy = OPVAux.R*sin(OPVAux.alfa*PI / 180);
			PVz = OPVAux.R*cos(OPVAux.beta*PI / 180)*cos(OPVAux.alfa*PI / 180);
			if (camera == CAM_GEODE)
				{	// Vector camp on mira (cap a (R,alfa,beta)
					PVx = PVx + sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
					PVy = PVy + sin(OPVAux.alfa * PI / 180);
					PVz = PVz + cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
				}
			}
		else {	PVx = OPVAux.R*sin(OPVAux.alfa*PI / 180);
				PVy = OPVAux.R*cos(OPVAux.beta*PI / 180)*cos(OPVAux.alfa*PI / 180);
				PVz = OPVAux.R*sin(OPVAux.beta*PI / 180)*cos(OPVAux.alfa*PI / 180);
				if (camera == CAM_GEODE)
				{	// Vector camp on mira (cap a (R,alfa,beta)
					PVx = PVx + sin(OPVAux.alfa * PI / 180);
					PVy	= PVy + cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
					PVz = PVz + sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
				}
			}
		}

// Status Bar PVx
	if (projeccio == CAP) buffer = "       ";
		else if (pan) double2CString(tr_cpv.x);
			else double2CString(PVx);
	sss = _T("PVx=") + buffer;
// Refrescar posició PVx Status Bar
	GetStatusBar().SetPaneText(4, sss);

// Status Bar PVy
	if (projeccio == CAP) buffer = "       ";
		else if (pan) double2CString(tr_cpv.y);
			else double2CString(PVy);
	sss = _T("PVy=") + buffer;
// Refrescar posició PVy Status Bar
	GetStatusBar().SetPaneText(5, sss);

// Status Bar PVz
	if (projeccio == CAP) buffer = "       ";
		else if (pan) double2CString(tr_cpv.z);
			else double2CString(PVz);
	sss = _T("PVz=") + buffer;
// Refrescar posició PVz Status Bar
	GetStatusBar().SetPaneText(6, sss);

// Status Bar per indicar el modus de canvi de color (FONS o OBJECTE)
	sss = " ";
	if (sw_grid) sss = "GRID";
		else if (pan) sss = " PAN";
			else if (camera == CAM_NAVEGA) sss = " NAV";
				else if (sw_color) sss = " OBJ";
					else sss = "FONS";

// Refrescar posició Transformacions en Status Bar
	GetStatusBar().SetPaneText(7, sss);

// Status Bar per indicar tipus de Transformació (TRAS, ROT, ESC)
	sss = " ";
	if (transf) {
		if (rota) sss = "ROT";
		else if (trasl) sss = "TRA";
		else if (escal) sss = "ESC";
	}
	else if ((!sw_grid) && (!pan) && (camera != CAM_NAVEGA))
	  {	// Components d'intensitat de fons que varien per teclat
		if ((fonsR) && (fonsG) && (fonsB)) sss = " RGB";
		else if ((fonsR) && (fonsG)) sss = " RG ";
		else if ((fonsR) && (fonsB)) sss = " R   B";
		else if ((fonsG) && (fonsB)) sss = "   GB";
		else if (fonsR) sss = " R  ";
		else if (fonsG) sss = "   G ";
		else if (fonsB) sss = "      B";
	  }
// Refrescar posició Transformacions en Status Bar
	GetStatusBar().SetPaneText(8, sss);

// Status Bar dels paràmetres de Transformació, Color i posicions de Robot i Cama
	sss = " ";
	if (transf)
	{	if (rota)
		{	buffer.Format(_T("%.1f"), TG.VRota.x);
			sss = _T("  ") + buffer + _T("   ");

			buffer.Format(_T("%.1f"), TG.VRota.y);
			sss = sss + buffer + _T("   ");

			buffer.Format(_T("%.1f"), TG.VRota.z);
			sss = sss + buffer;
		}
		else if (trasl)
		{	buffer.Format(_T("%.1f"), TG.VTras.x);
			sss = _T("  ") + buffer + _T("   ");

			buffer.Format(_T("%.1f"), TG.VTras.y);
			sss = sss + buffer + _T("   ");

			buffer.Format(_T("%.1f"), TG.VTras.z);
			sss = sss + buffer;
		}
		else if (escal)
		{	buffer.Format(_T("%.2f"), TG.VScal.x);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.2f"), TG.VScal.y);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.2f"), TG.VScal.x);
			sss = sss + buffer;
		}
	}
	else if ((!sw_grid) && (!pan) && (camera != CAM_NAVEGA))
	 {	// Color fons
		if (!sw_color)
		{	buffer.Format(_T("%.3f"), c_fons.r);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.3f"), c_fons.g);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.3f"), c_fons.b);
			sss = sss + buffer;
		}
		else
		{	// Color objecte
			buffer.Format(_T("%.3f"), col_obj.r);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.3f"), col_obj.g);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.3f"), col_obj.b);
			sss = sss + buffer;
		}
	}

// Refrescar posició dels paràmetres de Transformació, Color i posicions de Robot i Cama
	GetStatusBar().SetPaneText(9, sss);

// Status Bar per indicar el pas del Fractal
	if (objecte == O_FRACTAL)
	{	buffer.Format(_T("%.1d"), pas);
		sss = _T("Pas=") + buffer;
	}
	else { sss = "          "; }

// Refrescar posició Transformacions en Status Bar
	GetStatusBar().SetPaneText(10, sss);
}

void CEntornVGIView::double2CString(double varf)
{
	double vdouble;
	vdouble = varf;
	if (abs(varf)<1.0) buffer.Format(_T("%.5f"), varf);
	else if (abs(varf)<99.0) buffer.Format(_T("%.4f"), varf);
	else if (abs(varf)<999.0) buffer.Format(_T("%.3f"), varf);
	else if (abs(varf)<9999.0) buffer.Format(_T("%.2f"), varf);
	else if (abs(varf)<99999.0) buffer.Format(_T("%.1f"), varf);
	else buffer.Format(_T("%.0f"), varf);

}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL TECLAT                              */
/* ------------------------------------------------------------------------- */

// OnKeyDown: Funció de tractament de teclat (funció que es crida quan es prem una tecla)
//   PARÀMETRES:
//    - nChar: Codi del caracter seleccionat
//    - nRepCnt: Nombre de vegades que s'ha apretat la tecla (acceleració)
//    - nFlags: Flags d'interrupció activats.
void CEntornVGIView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	const double incr = 0.025f;
	double modul = 0;
	GLdouble vdir[3] = { 0, 0, 0 };

	if (nChar == 'F') this->OnVistaFullscreen();	// Activació-Desactivació Full Screen
	else if (objecte==C_BEZIER || objecte == C_BSPLINE || objecte == C_LEMNISCATA || objecte == C_HERMITTE 
		                       || objecte == C_CATMULL_ROM) Teclat_PasCorbes(nChar, nRepCnt);
	else if ((sw_grid) && ((grid.x) || (grid.y) || (grid.z))) Teclat_Grid(nChar, nRepCnt);
		else if (((nChar == 'G') || (nChar == 'g')) && ((grid.x) || (grid.y) || (grid.z))) sw_grid = !sw_grid;
			else if ((!pan) && (!transf) && (camera != CAM_NAVEGA))
					{	if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
							else Teclat_ColorObjecte(nChar, nRepCnt);
					}
					else {	if (transf)
								{	if (rota) Teclat_TransRota(nChar, nRepCnt);
										 else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
											else if (escal) Teclat_TransEscala(nChar, nRepCnt);
								}
							if (pan) Teclat_Pan(nChar, nRepCnt);
								else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
									else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
										else Teclat_ColorObjecte(nChar, nRepCnt);
						}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CEntornVGIView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


// Teclat_ColorObjecte: Teclat pels canvis de color de l'objecte per teclat.
void CEntornVGIView::Teclat_ColorObjecte(UINT nChar, UINT nRepCnt)
{
	const double incr = 0.025f;

// FRACTAL: Canvi resolució del fractal pe tecles '+' i'-'
	if (objecte == O_FRACTAL)
	{	if (nChar == 109) // Caràcter '-' 
			{	pas = pas * 2;
				if (pas>64) pas = 64;
				sw_il = true;
		}
		else if (nChar == 107) // Caràcter '+' 
				{	pas = pas / 2;
					if (pas<1) pas = 1;
					sw_il = true;
				}
	}
	//	else 
	if (nChar == VK_DOWN) 
	{	if (fonsR) {	col_obj.r -= nRepCnt*incr;
						if (col_obj.r<0.0) col_obj.r = 0.0;
					}
		if (fonsG) {	col_obj.g -= nRepCnt*incr;
						if (col_obj.g<0.0) col_obj.g = 0.0;
					}
		if (fonsB) {	col_obj.b -= nRepCnt*incr;
						if (col_obj.b<0.0) col_obj.b = 0.0;
					}
	}
	else if (nChar == VK_UP) 
		{	if (fonsR) {	col_obj.r += nRepCnt*incr;
							if (col_obj.r>1.0) col_obj.r = 1.0;
						}
			if (fonsG) {	col_obj.g += nRepCnt*incr;
							if (col_obj.g>1.0) col_obj.g = 1.0;
						}
			if (fonsB) {	col_obj.b += nRepCnt*incr;
							if (col_obj.b>1.0) col_obj.b = 1.0;
						}
		}
		else if (nChar == VK_SPACE) 
			{	if ((fonsR) && (fonsG) && (fonsB)) {	fonsG = false;
														fonsB = false;
													}
				  else if ((fonsR) && (fonsG)) {	fonsG = false;
													fonsB = true;
												}
					else if ((fonsR) && (fonsB)) {	fonsR = false;
													fonsG = true;
												}
						else if ((fonsG) && (fonsB)) fonsR = true;
							else if (fonsR) {	fonsR = false;
												fonsG = true;
											}
								else if (fonsG) {	fonsG = false;
													fonsB = true;
												}
									else if (fonsB) {	fonsR = true;
														fonsG = true;
														fonsB = false;
													}
			}
				else if ((nChar == 'o') || (nChar == 'O')) sw_color = !sw_color;
					else if ((nChar == 'b') || (nChar == 'B')) sw_color = !sw_color;
}


// Teclat_ColorFons: Teclat pels canvis del color de fons.
void CEntornVGIView::Teclat_ColorFons(UINT nChar, UINT nRepCnt)
{
	const double incr = 0.025f;

// FRACTAL: Canvi resolució del fractal pe tecles '+' i'-'
	if (objecte == O_FRACTAL)
	{	if (nChar == 109) // Caràcter '-' 
		{	pas = pas * 2;
			if (pas>64) pas = 64;
			sw_il = true;
		}
		else if (nChar == 107) // Caràcter '+' 
		{	pas = pas / 2;
			if (pas<1) pas = 1;
			sw_il = true;
		}
	}
	//	else 
	if (nChar == VK_DOWN) {
		if (fonsR) {	c_fons.r -= nRepCnt*incr;
						if (c_fons.r<0.0) c_fons.r = 0.0;
					}
		if (fonsG) {	c_fons.g -= nRepCnt*incr;
						if (c_fons.g<0.0) c_fons.g = 0.0;
					}
		if (fonsB) {	c_fons.b -= nRepCnt*incr;
						if (c_fons.b<0.0) c_fons.b = 0.0;
					}
		}
	else if (nChar == VK_UP) {
		if (fonsR) {	c_fons.r += nRepCnt*incr;
						if (c_fons.r>1.0) c_fons.r = 1.0;
					}
		if (fonsG) {	c_fons.g += nRepCnt*incr;
						if (c_fons.g>1.0) c_fons.g = 1.0;
					}
		if (fonsB) {	c_fons.b += nRepCnt*incr;
						if (c_fons.b>1.0) c_fons.b = 1.0;
					}
		}
	else if (nChar == VK_SPACE) {
		if ((fonsR) && (fonsG) && (fonsB)) {	fonsG = false;
												fonsB = false;
											}
			else if ((fonsR) && (fonsG)) {		fonsG = false;
												fonsB = true;
										}
				else if ((fonsR) && (fonsB)) {	fonsR = false;
												fonsG = true;
											}
					else if ((fonsG) && (fonsB)) fonsR = true;
						else if (fonsR) {	fonsR = false;
											fonsG = true;
										}
							else if (fonsG) {	fonsG = false;
												fonsB = true;
											}
								else if (fonsB) {	fonsR = true;
													fonsG = true;
													fonsB = false;
												}
		}
		else if ((nChar == 'o') || (nChar == 'O')) sw_color = !sw_color;
			else if ((nChar == 'b') || (nChar == 'B')) sw_color = sw_color;
}

// Teclat_Navega: Teclat pels moviments de navegació.
void CEntornVGIView::Teclat_Navega(UINT nChar, UINT nRepCnt)
{
	GLdouble vdir[3] = { 0, 0, 0 };
	double modul = 0;

// Entorn VGI: Controls de moviment de navegació
	vdir[0] = n[0] - opvN.x;
	vdir[1] = n[1] - opvN.y;
	vdir[2] = n[2] - opvN.z;
	modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
	vdir[0] = vdir[0] / modul;
	vdir[1] = vdir[1] / modul;
	vdir[2] = vdir[2] / modul;
	switch (nChar)
	{	
	// Tecla cursor amunt segons configuració eixos en Polars
	case VK_UP:
		if (Vis_Polar == POLARZ) {  // (X,Y,Z)
			opvN.x += nRepCnt * fact_pan * vdir[0];
			opvN.y += nRepCnt * fact_pan * vdir[1];
			n[0] += nRepCnt * fact_pan * vdir[0];
			n[1] += nRepCnt * fact_pan * vdir[1];
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			opvN.x += nRepCnt * fact_pan * vdir[0];
			opvN.z += nRepCnt * fact_pan * vdir[2];
			n[0] += nRepCnt * fact_pan * vdir[0];
			n[2] += nRepCnt * fact_pan * vdir[2];
		}
		else if (Vis_Polar == POLARX) {	//(X,Y,Z) --> (Y,Z,X)
			opvN.y += nRepCnt * fact_pan * vdir[1];
			opvN.z += nRepCnt * fact_pan * vdir[2];
			n[1] += nRepCnt * fact_pan * vdir[1];
			n[2] += nRepCnt * fact_pan * vdir[2];
		}
		break;

	// Tecla cursor avall segons configuració eixos en Polars
	case VK_DOWN:
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			opvN.x -= nRepCnt * fact_pan * vdir[0];
			opvN.y -= nRepCnt * fact_pan * vdir[1];
			n[0] -= nRepCnt * fact_pan * vdir[0];
			n[1] -= nRepCnt * fact_pan * vdir[1];
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			opvN.x -= nRepCnt * fact_pan * vdir[0];
			opvN.z -= nRepCnt * fact_pan * vdir[2];
			n[0] -= nRepCnt * fact_pan * vdir[0];
			n[2] -= nRepCnt * fact_pan * vdir[2];
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			opvN.y -= nRepCnt * fact_pan * vdir[1];
			opvN.z -= nRepCnt * fact_pan * vdir[2];
			n[1] -= nRepCnt * fact_pan * vdir[1];
			n[2] -= nRepCnt * fact_pan * vdir[2];
		}
		break;

	// Tecla cursor esquerra segons configuració eixos en Polars
	case VK_LEFT:
		angleZ = +nRepCnt*fact_pan;
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			n[0] = vdir[0]; // n[0] - opvN.x;
			n[1] = vdir[1]; // n[1] - opvN.y;
			n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
			n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
			n[0] = n[0] + opvN.x;
			n[1] = n[1] + opvN.y;
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			n[2] = vdir[2]; // n[2] - opvN.z;
			n[0] = vdir[0]; // n[0] - opvN.x;
			n[2] = n[2] * cos(angleZ * PI / 180) - n[0] * sin(angleZ * PI / 180);
			n[0] = n[2] * sin(angleZ * PI / 180) + n[0] * cos(angleZ * PI / 180);
			n[2] = n[2] + opvN.z;
			n[0] = n[0] + opvN.x;
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			n[1] = vdir[1]; // n[1] - opvN.y;
			n[2] = vdir[2]; // n[2] - opvN.z;
			n[1] = n[1] * cos(angleZ * PI / 180) - n[2] * sin(angleZ * PI / 180);
			n[2] = n[1] * sin(angleZ * PI / 180) + n[2] * cos(angleZ * PI / 180);
			n[1] = n[1] + opvN.y;
			n[2] = n[2] + opvN.z;
		}
		break;

	// Tecla cursor dret segons configuració eixos en Polars
	case VK_RIGHT:
		angleZ = 360 - nRepCnt * fact_pan;
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			n[0] = vdir[0]; // n[0] - opvN.x;
			n[1] = vdir[1]; // n[1] - opvN.y;
			n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
			n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
			n[0] = n[0] + opvN.x;
			n[1] = n[1] + opvN.y;
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			n[2] = vdir[2]; // n[2] - opvN.z;
			n[0] = vdir[0]; // n[0] - opvN.x;
			n[2] = n[2] * cos(angleZ * PI / 180) - n[0] * sin(angleZ * PI / 180);
			n[0] = n[2] * sin(angleZ * PI / 180) + n[0] * cos(angleZ * PI / 180);
			n[2] = n[2] + opvN.z;
			n[0] = n[0] + opvN.x;
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			n[1] = vdir[1]; // n[1] - opvN.y;
			n[2] = vdir[2]; // n[2] - opvN.z;
			n[1] = n[1] * cos(angleZ * PI / 180) - n[2] * sin(angleZ * PI / 180);
			n[2] = n[1] * sin(angleZ * PI / 180) + n[2] * cos(angleZ * PI / 180);
			n[1] = n[1] + opvN.y;
			n[2] = n[2] + opvN.z;
		}
		break;

	// Tecla Inicio segons configuració eixos en Polars
	case VK_HOME:
		if (Vis_Polar == POLARZ) {
			opvN.z += nRepCnt * fact_pan;
			n[2] += nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARY) {
			opvN.y += nRepCnt * fact_pan;
			n[1] += nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x += nRepCnt * fact_pan;
			n[0] += nRepCnt * fact_pan;
		}
		break;

	// Tecla 7 NUMERIC KEYPAD: Inicio segons configuració eixos en Polars
	case VK_NUMPAD7:
		if (Vis_Polar == POLARZ) {
			opvN.z += nRepCnt * fact_pan;
			n[2] += nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARY) {
			opvN.y += nRepCnt * fact_pan;
			n[1] += nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x += nRepCnt * fact_pan;
			n[0] += nRepCnt * fact_pan;
		}
		break;

	// Tecla Fin segons configuració eixos en Polars
	case VK_END:
		if (Vis_Polar == POLARZ) {
			opvN.z -= nRepCnt * fact_pan;
			n[2] -= nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARY) {
			opvN.y -= nRepCnt * fact_pan;
			n[1] -= nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x -= nRepCnt * fact_pan;
			n[0] -= nRepCnt * fact_pan;
		}
		break;

	// Tecla 1 NUMERIC KEYPAD: Fin segons configuració eixos en Polars
	case VK_NUMPAD1:
		if (Vis_Polar == POLARZ) {
			opvN.z -= nRepCnt * fact_pan;
			n[2] -= nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARY) {
			opvN.y -= nRepCnt * fact_pan;
			n[1] -= nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x -= nRepCnt * fact_pan;
			n[0] -= nRepCnt * fact_pan;
		}
		break;

	// Tecla PgUp
	case VK_PRIOR:
		fact_pan /= 2;
		if (fact_pan<0.125) fact_pan = 0.125;
		break;

	// Tecla PgUp - NUMERIC KEYPAD
	case VK_NUMPAD9 :
		fact_pan /= 2;
		if (fact_pan < 0.125) fact_pan = 0.125;
		break;

	// Tecla PgDown
	case VK_NEXT:
		fact_pan *= 2;
		if (fact_pan>2048) fact_pan = 2048;
		break;

	// Tecla PgDown - NUEMRIC KEYPAD
	case VK_NUMPAD3:
		fact_pan *= 2;
		if (fact_pan > 2048) fact_pan = 2048;
		break;

	default:
		if (transf)
		{	if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		else if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


// Teclat_Pan: Teclat pels moviments de Pan.
void CEntornVGIView::Teclat_Pan(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
	// Tecla cursor amunt
	case VK_UP:
		tr_cpv.y -= nRepCnt*fact_pan;
		if (tr_cpv.y<-100000) tr_cpv.y = 100000;
		break;

	// Tecla cursor avall
	case VK_DOWN:
		tr_cpv.y += nRepCnt*fact_pan;
		if (tr_cpv.y>100000) tr_cpv.y = 100000;
		break;

	// Tecla cursor esquerra
	case VK_LEFT:
		tr_cpv.x += nRepCnt*fact_pan;
		if (tr_cpv.x>100000) tr_cpv.x = 100000;
		break;

	// Tecla cursor dret
	case VK_RIGHT:
		tr_cpv.x -= nRepCnt*fact_pan;
		if (tr_cpv.x<-100000) tr_cpv.x = 100000;
		break;

	// Tecla PgUp
	case VK_PRIOR:
		fact_pan /= 2;
		if (fact_pan<0.125) fact_pan = 0.125;
		break;

	// Tecla PgUp, 9 NUMERIC KEYPAD
	case VK_NUMPAD9:
		fact_pan /= 2;
		if (fact_pan < 0.125) fact_pan = 0.125;
		break;

	// Tecla PgDown
	case VK_NEXT:
		fact_pan *= 2;
		if (fact_pan>2048) fact_pan = 2048;
		break;

	// Tecla PgDown, 3 NUMERIC KEYPAD
	case VK_NUMPAD3:
		fact_pan *= 2;
		if (fact_pan > 2048) fact_pan = 2048;
		break;

	// Tecla Insert: Fixar el desplaçament de pantalla (pan)
	case VK_INSERT:
		// Acumular desplaçaments de pan (tr_cpv) en variables fixes (tr_cpvF).
		tr_cpvF.x += tr_cpv.x;		tr_cpv.x = 0.0;
		if (tr_cpvF.x>100000) tr_cpvF.y = 100000;
		tr_cpvF.y += tr_cpv.y;		tr_cpv.y = 0.0;
		if (tr_cpvF.y>100000) tr_cpvF.y = 100000;
		tr_cpvF.z += tr_cpv.z;		tr_cpv.z = 0.0;
		if (tr_cpvF.z>100000) tr_cpvF.z = 100000;
		break;

	// Tecla 0 NUMERIC KEYPAD: Insert: Fixar el desplaçament de pantalla (pan)
	case VK_NUMPAD0:
		// Acumular desplaçaments de pan (tr_cpv) en variables fixes (tr_cpvF).
		tr_cpvF.x += tr_cpv.x;		tr_cpv.x = 0.0;
		if (tr_cpvF.x > 100000) tr_cpvF.y = 100000;
		tr_cpvF.y += tr_cpv.y;		tr_cpv.y = 0.0;
		if (tr_cpvF.y > 100000) tr_cpvF.y = 100000;
		tr_cpvF.z += tr_cpv.z;		tr_cpv.z = 0.0;
		if (tr_cpvF.z > 100000) tr_cpvF.z = 100000;
		break;

	// Tecla 0: Insert: Fixar el desplaçament de pantalla (pan)
	case '0':
		// Acumular desplaçaments de pan (tr_cpv) en variables fixes (tr_cpvF).
		tr_cpvF.x += tr_cpv.x;		tr_cpv.x = 0.0;
		if (tr_cpvF.x > 100000) tr_cpvF.y = 100000;
		tr_cpvF.y += tr_cpv.y;		tr_cpv.y = 0.0;
		if (tr_cpvF.y > 100000) tr_cpvF.y = 100000;
		tr_cpvF.z += tr_cpv.z;		tr_cpv.z = 0.0;
		if (tr_cpvF.z > 100000) tr_cpvF.z = 100000;
		break;

	// Tecla Delete: Inicialitzar el desplaçament de pantalla (pan)
	case VK_DELETE:
		// Inicialitzar els valors de pan tant de la variable tr_cpv com de la tr_cpvF.
		tr_cpv.x = 0.0;			tr_cpv.y = 0.0;			tr_cpv.z = 0.0;
		tr_cpvF.x = 0.0;		tr_cpvF.y = 0.0;		tr_cpvF.z = 0.0;
		break;

	default:
		if (transf)
		{	if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}

// Teclat_TransEscala: Teclat pels canvis del valor d'escala per X,Y,Z.
void CEntornVGIView::Teclat_TransEscala(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
// Modificar vector d'Escalatge per teclat (actiu amb Escalat únicament)
	// Tecla '+' NUMERIC KEYPAD  (augmentar tot l'escalat)
	case VK_ADD: // 107:
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x>8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y>8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z>8192) TG.VScal.z = 8192;
		break;

	// Modificar vector d'Escalatge per teclat (actiu amb Escalat únicament)
	// Tecla '+' (augmentar tot l'escalat)
	case '+':
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x > 8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

	// Tecla '-' (disminuir tot l'escalat)
	case VK_SUBTRACT: // 109:
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x<0.25) TG.VScal.x = 0.25;
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y<0.25) TG.VScal.y = 0.25;
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z<0.25) TG.VScal.z = 0.25;
		break;

	// Tecla cursor amunt ('8') - NUMERIC KEYPAD
	case VK_NUMPAD8: // 104:
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x>8192) TG.VScal.x = 8192;
		break;

	// Tecla cursor amunt ('8')
	case '8':
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x > 8192) TG.VScal.x = 8192;
		break;

	// Tecla cursor avall ('2') - NUMERIC KEYPAD
	case VK_NUMPAD2: // 98:
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x<0.25) TG.VScal.x = 0.25;
		break;

	// Tecla cursor avall ('2')
	case '2':
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
		break;

	// Tecla cursor esquerra ('4') - NUMERIC KEYPAD
	case VK_NUMPAD4: // 100:
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y<0.25) TG.VScal.y = 0.25;
		break;

	// Tecla cursor esquerra ('4')
	case '4':
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
		break;

	// Tecla cursor dret ('6') - NUMERIC KEYPAD
	case VK_NUMPAD6: // 102:
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y>8192) TG.VScal.y = 8192;
		break;

	// Tecla cursor dret ('6')
	case '6':
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192) TG.VScal.y = 8192;
		break;

	// Tecla HOME ('7') - NUMERIC KEYPAD
	case 103:
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z>8192) TG.VScal.z = 8192;
		break;

	// Tecla HOME
	case VK_HOME:
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

	// Tecla HOME ('7')
	case '7':
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

	// Tecla END ('1') - NUMERIC KEYPAD
	case VK_NUMPAD1: // 97:
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z<0.25) TG.VScal.z = 0.25;
		break;

	// Tecla END ('1')
	case '1':
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
		break;

	// Tecla INSERT
	case VK_INSERT:
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x>8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y>8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z>8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x<0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y<0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z<0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x<-100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x>10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y<-100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y>10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z<-100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z>10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla 0 NUMERIC KEYPAD, INSERT
	case VK_NUMPAD0:
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla 0, INSERT
	case '0':
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla Delete: Esborrar les Transformacions Geomètriques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geomètriques i de pan en variables fixes.
		TGF.VScal.x = 1.0;		TGF.VScal.y = 1.0;;		TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;		TG.VScal.y = 1.0;		TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;		TGF.VRota.y = 0.0;		TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;		TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;		TGF.VTras.y = 0.0;		TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	default:
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}

// Teclat_TransRota: Teclat pels canvis del valor del vector de l'angle de rotació per X,Y,Z.
void CEntornVGIView::Teclat_TransRota(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
	// Tecla cursor amunt ('8') - NUMERIC KEYPAD
	case VK_NUMPAD8: // 104:
		TG.VRota.x += nRepCnt*fact_Rota;
		if (TG.VRota.x >= 360) TG.VRota.x -= 360;
		break;

	// Tecla cursor amunt ('8')
	case '8':
		TG.VRota.x += nRepCnt * fact_Rota;
		if (TG.VRota.x >= 360) TG.VRota.x -= 360;
		break;

	// Tecla cursor avall ('2') - NUMERIC KEYPAD
	case VK_NUMPAD2: // 98:
		TG.VRota.x -= nRepCnt*fact_Rota;
		if (TG.VRota.x<0.0) TG.VRota.x += 360;
		break;

	// Tecla cursor avall ('2') - NUMERIC KEYPAD
	case '2':
		TG.VRota.x -= nRepCnt * fact_Rota;
		if (TG.VRota.x < 0.0) TG.VRota.x += 360;
		break;

	// Tecla cursor esquerra ('4') - NUMERIC KEYPAD
	case VK_NUMPAD4: //100:
		TG.VRota.y -= nRepCnt*fact_Rota;
		if (TG.VRota.y<0.0) TG.VRota.y += 360;
		break;

	// Tecla cursor esquerra ('4')
	case '4':
		TG.VRota.y -= nRepCnt * fact_Rota;
		if (TG.VRota.y < 0.0) TG.VRota.y += 360;
		break;

	// Tecla cursor dret ('6') - NUMERIC KEYPAD
	case VK_NUMPAD6: // 102:
		TG.VRota.y += nRepCnt*fact_Rota;
		if (TG.VRota.y >= 360) TG.VRota.y -= 360;
		break;

	// Tecla cursor dret ('6')
	case '6':
		TG.VRota.y += nRepCnt * fact_Rota;
		if (TG.VRota.y >= 360) TG.VRota.y -= 360;
		break;

	// Tecla Return NUMERIC KEYPAD
	case VK_RETURN:
		TG.VRota.y += nRepCnt * fact_Rota;
		if (TG.VRota.y >= 360.0) TG.VRota.y -= 360.0;
		break;

	// Tecla HOME ('7') - NUMERIC KEYPAD
	case VK_NUMPAD7: //103:
		TG.VRota.z += nRepCnt*fact_Rota;
		if (TG.VRota.z >= 360) TG.VRota.z -= 360;
		break;

	// Tecla HOME ('7')
	case '7': //103:
		TG.VRota.z += nRepCnt * fact_Rota;
		if (TG.VRota.z >= 360) TG.VRota.z -= 360;
		break;

	// Tecla END ('1') - NUMERIC KEYPAD
	case VK_NUMPAD1: //97:
		TG.VRota.z -= nRepCnt*fact_Rota;
		if (TG.VRota.z<0.0) TG.VRota.z += 360;
		break;

	// Tecla END ('1')
	case '1':
		TG.VRota.z -= nRepCnt * fact_Rota;
		if (TG.VRota.z < 0.0) TG.VRota.z += 360;
		break;

	// Tecla PgUp ('9') - NUMERIC KEYPAD
	case VK_NUMPAD9: // 105:
		fact_Rota /= 2;
		if (fact_Rota<1) fact_Rota = 1.0;
		break;

	// Tecla PgUp
	case VK_PRIOR:
		fact_Rota /= 2;
		if (fact_Rota < 1) fact_Rota = 1.0;
		break;

	// Tecla PgUp ('9')
	case '9':
		fact_Rota /= 2;
		if (fact_Rota < 1) fact_Rota = 1.0;
		break;

	// Tecla PgDown ('3') - NUMERIC KEYPAD
	case VK_NUMPAD3: //99:
		fact_Rota *= 2;
		if (fact_Rota>90) fact_Rota = 90;
		break;

	// Tecla PgDown ('3') - NUMERIC KEYPAD
	case VK_NEXT:
		fact_Rota *= 2;
		if (fact_Rota > 90) fact_Rota = 90;
		break;

	// Tecla PgDown ('3')
	case '3':
		fact_Rota *= 2;
		if (fact_Rota > 90) fact_Rota = 90;
		break;

// Modificar vector d'Escalatge per teclat (actiu amb Rotació)
	// Tecla '+' (augmentar escalat)
	case VK_ADD: //107:
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x>8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y>8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z>8192) TG.VScal.z = 8192;
		break;

	// Modificar vector d'Escalatge per teclat (actiu amb Rotació)
	// Tecla '+' (augmentar escalat)
	case '+':
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x > 8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

	// Tecla '-' (disminuir escalat)
	case VK_SUBTRACT: //109:
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x<0.25) TG.VScal.x = 0.25;
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y<0.25) TG.VScal.y = 0.25;
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z<0.25) TG.VScal.z = 0.25;
		break;

	// Tecla Insert: Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
	case VK_INSERT:
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x>8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y>8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z>8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x<0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y<0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z<0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x<-100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x>10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y<-100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y>10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z<-100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z>10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla 0 NUMERIC KEYPAD, Insert: Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
	case VK_NUMPAD0:
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla Insert: Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
	case '0':
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla Delete: Esborrar les Transformacions Geomètriques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geomètriques i de pan en variables fixes.
		TGF.VScal.x = 1.0;	TGF.VScal.y = 1.0;;	TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;		TG.VScal.y = 1.0;		TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;	TGF.VRota.y = 0.0;	TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;		TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;	TGF.VTras.y = 0.0;	TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla Espaiador
	case VK_SPACE:
		rota = !rota;
		trasl = !trasl;
		break;

	default:
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


// Teclat_TransTraslada: Teclat pels canvis del valor de traslació per X,Y,Z.
void CEntornVGIView::Teclat_TransTraslada(UINT nChar, UINT nRepCnt)
{
	GLdouble vdir[3] = { 0, 0, 0 };
	double modul = 0;

	switch (nChar)
	{
	// Tecla cursor amunt ('8') - NUMERIC KEYPAD
	case VK_NUMPAD8: // 104:
		TG.VTras.x -= nRepCnt*fact_Tras;
		if (TG.VTras.x<-100000) TG.VTras.x = 100000;
		break;

	// Tecla cursor amunt ('8')
	case '8':
		TG.VTras.x -= nRepCnt * fact_Tras;
		if (TG.VTras.x < -100000) TG.VTras.x = 100000;
		break;


	// Tecla cursor avall ('2') - NUMERIC KEYPAD
	case VK_NUMPAD2: // 98:
		TG.VTras.x += nRepCnt*fact_Tras;
		if (TG.VTras.x>10000) TG.VTras.x = 100000;
		break;

	// Tecla cursor avall ('2')
	case '2':
		TG.VTras.x += nRepCnt * fact_Tras;
		if (TG.VTras.x > 10000) TG.VTras.x = 100000;
		break;

	// Tecla cursor esquerra ('4') - NUMERIC KEYPAD
	case VK_NUMPAD4: // 100:
		TG.VTras.y -= nRepCnt*fact_Tras;
		if (TG.VTras.y<-100000) TG.VTras.y = -100000;
		break;

	// Tecla cursor esquerra ('4')
	case '4':
		TG.VTras.y -= nRepCnt * fact_Tras;
		if (TG.VTras.y < -100000) TG.VTras.y = -100000;
		break;

	// Tecla cursor dret ('6') - NUMERIC KEYPAD
	case VK_NUMPAD6: // 102:
		TG.VTras.y += nRepCnt*fact_Tras;
		if (TG.VTras.y>100000) TG.VTras.y = 100000;
		break;

	// Tecla cursor dret ('6')
	case '6':
		TG.VTras.y += nRepCnt * fact_Tras;
		if (TG.VTras.y > 100000) TG.VTras.y = 100000;
		break;

	// Tecla HOME ('7') - NUMERIC KEYPAD
	case VK_NUMPAD7: //103:
		TG.VTras.z += nRepCnt*fact_Tras;
		if (TG.VTras.z>100000) TG.VTras.z = 100000;
		break;

	// Tecla HOME ('7')
	case VK_HOME: // 103:
		TG.VTras.z += nRepCnt * fact_Tras;
		if (TG.VTras.z > 100000) TG.VTras.z = 100000;
		break;

	// Tecla HOME ('7')
	case '7':
		TG.VTras.z += nRepCnt * fact_Tras;
		if (TG.VTras.z > 100000) TG.VTras.z = 100000;
		break;

	// Tecla END ('1') - NUMERIC KEYPAD
	case VK_NUMPAD1: // 97:
		TG.VTras.z -= nRepCnt*fact_Tras;
		if (TG.VTras.z<-100000) TG.VTras.z = -100000;
		break;

	// Tecla END ('1')
	case '1':
		TG.VTras.z -= nRepCnt * fact_Tras;
		if (TG.VTras.z < -100000) TG.VTras.z = -100000;
		break;

	// Tecla PgUp ('9') - NUMERIC KEYPAD
	case VK_NUMPAD9: // 105:
		fact_Tras /= 2;
		if (fact_Tras<1) fact_Tras = 1;
		break;

	// Tecla PgUp
	case VK_PRIOR:
		fact_Tras /= 2;
		if (fact_Tras < 1) fact_Tras = 1;
		break;

	// Tecla PgUp ('9')
	case '9':
		fact_Tras /= 2;
		if (fact_Tras < 1) fact_Tras = 1;
		break;

	// Tecla PgDown ('3') - NUMERIC KEYPAD
	case VK_NUMPAD3: //99:
		fact_Tras *= 2;
		if (fact_Tras>100000) fact_Tras = 100000;
		break;

	// Tecla PgDown ('3')
	case VK_NEXT:
		fact_Tras *= 2;
		if (fact_Tras > 100000) fact_Tras = 100000;
		break;

	// Tecla PgDown ('3')
	case '3':
		fact_Tras *= 2;
		if (fact_Tras > 100000) fact_Tras = 100000;
		break;

// Modificar vector d'Escalatge per teclat (actiu amb Traslació)
	// Tecla '+' (augmentar escalat)
	case VK_ADD: // 107:
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x>8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y>8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z>8192) TG.VScal.z = 8192;
		break;

	// Modificar vector d'Escalatge per teclat (actiu amb Traslació)
	// Tecla '+' (augmentar escalat)
	case '+':
		TG.VScal.x = TG.VScal.x * 2;
		if (TG.VScal.x > 8192) TG.VScal.x = 8192;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192) TG.VScal.y = 8192;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192) TG.VScal.z = 8192;
		break;

	// Tecla '-' (disminuir escalat)
	case VK_SUBTRACT: // 109:
		TG.VScal.x = TG.VScal.x / 2;
		if (TG.VScal.x<0.25) TG.VScal.x = 0.25;
		TG.VScal.y = TG.VScal.y / 2;
		if (TG.VScal.y<0.25) TG.VScal.y = 0.25;
		TG.VScal.z = TG.VScal.z / 2;
		if (TG.VScal.z<0.25) TG.VScal.z = 0.25;
		break;

	// Tecla INSERT
	case VK_INSERT:
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x>8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y>8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z>8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x<0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y<0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z<0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x<-100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x>10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y<-100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y>10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z<-100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z>10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla 0 NUMERIC KEYPAD, INSERT
	case VK_NUMPAD0:
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla 0, INSERT
	case '0':
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192)		TGF.VScal.x = 8192;
		if (TGF.VScal.y > 8192)		TGF.VScal.y = 8192;
		if (TGF.VScal.z > 8192)		TGF.VScal.z = 8192;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;	TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360)		TGF.VRota.x -= 360; 		if (TGF.VRota.x < 0) TGF.VRota.x += 360;
		if (TGF.VRota.y >= 360)		TGF.VRota.y -= 360;		if (TGF.VRota.y < 0) TGF.VRota.y += 360;
		if (TGF.VRota.z >= 360)		TGF.VRota.z -= 360;		if (TGF.VRota.z < 0) TGF.VRota.z += 360;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;					TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;	TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000)		TGF.VTras.x = 100000;		if (TGF.VTras.x > 10000) TGF.VTras.x = 100000;
		if (TGF.VTras.y < -100000)		TGF.VTras.y = 100000;		if (TGF.VTras.y > 10000) TGF.VTras.y = 100000;
		if (TGF.VTras.z < -100000)		TGF.VTras.z = 100000;		if (TGF.VTras.z > 10000) TGF.VTras.z = 100000;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla Delete: Esborrar les Transformacions Geomètriques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geomètriques i de pan en variables fixes.
		TGF.VScal.x = 1.0;		TGF.VScal.y = 1.0;;		TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;		TG.VScal.y = 1.0;		TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;		TGF.VRota.y = 0.0;		TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;		TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;		TGF.VTras.y = 0.0;		TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla Espaiador
	case VK_SPACE:
		rota = !rota;
		trasl = !trasl;
		break;

	default:
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


// Teclat_Grid: Teclat pels desplaçaments dels gridXY, gridXZ i gridYZ.
void CEntornVGIView::Teclat_Grid(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
	// Key Up cursor ('8') - NUMERIC KEYPAD
	case VK_NUMPAD8: // 104:
		hgrid.x -= nRepCnt*PAS_GRID;
		break;

	// Key Up cursor ('8')
	case '8':
		hgrid.x -= nRepCnt * PAS_GRID;
		break;

	// Key Down cursor ('2') - NUMERIC KEYPAD
	case VK_NUMPAD2: // 98:
		hgrid.x += nRepCnt*PAS_GRID;
		break;

	// Key Down cursor ('2') - NUMERIC KEYPAD
	case '2':
		hgrid.x += nRepCnt * PAS_GRID;
		break;

	// Key Left cursor ('4') - NUMERIC KEYPAD
	case VK_NUMPAD4: // 100:
		hgrid.y -= nRepCnt*PAS_GRID;
		break;

	// Key Left cursor ('4')
	case '4':
		hgrid.y -= nRepCnt * PAS_GRID;
		break;

	// Key Right cursor ('6') - NUMERIC KEYPAD
	case VK_NUMPAD6: // 102:
		hgrid.y += nRepCnt*PAS_GRID;
		break;

	// Key Right cursor ('6')
	case '6':
		hgrid.y += nRepCnt * PAS_GRID;
		break;

	// Key HOME ('7') - NUMERIC KEYPAD
	case VK_NUMPAD7: // 103:
		hgrid.z += nRepCnt*PAS_GRID;
		break;

	// Key HOME
	case VK_HOME:
		hgrid.z += nRepCnt * PAS_GRID;
		break;

	// Key HOME ('7')
	case '7':
		hgrid.z += nRepCnt * PAS_GRID;
		break;

	// Key END ('1') - NUMERIC KEYPAD
	case VK_NUMPAD1: // 97:
		hgrid.z -= nRepCnt*PAS_GRID;
		break;

	// Key END ('1') - NUMERIC KEYPAD
	case '1':
		hgrid.z -= nRepCnt * PAS_GRID;
		break;

		// Key grid ('G')
	case 'G':
		sw_grid = !sw_grid;
		break;

	default:
		break;
	}
}


// Teclat_PasCorbes: Teclat per incrementar-Decrementar el pas de dibuix de les corbes (pas_CS).
void CEntornVGIView::Teclat_PasCorbes(UINT nChar, UINT nRepCnt)
{
// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	switch (nChar)
	{
	// Tecla '+' (incrementar pas_CS)
	case 107:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > 0.5) pas_CS = 0.5;
		if (objecte == C_BSPLINE) {
			deleteVAOList(CRV_BSPLINE);		//Eliminar VAO anterior.
			//Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
			Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
			}
			else if (objecte == C_BEZIER) {
				deleteVAOList(CRV_BEZIER);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_BEZIER, load_Bezier_Curve_VAO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
				Set_VAOList(CRV_BEZIER, load_Bezier_Curve_EBO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
				}
			else if (objecte == C_LEMNISCATA) {
				deleteVAOList(CRV_LEMNISCATA3D);		//Eliminar VAO anterior.
				Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_EBO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.
				}
			else if (objecte == C_HERMITTE) {
				deleteVAOList(CRV_HERMITTE);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
				Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
				}
			else if (objecte == C_CATMULL_ROM) {
				deleteVAOList(CRV_CATMULL_ROM);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
				Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
				}
		break;

	// Tecla '-' (decrementar pas_CS)
	case 109:
		pas_CS = pas_CS / 2;
		if (pas_CS < 0.0125) pas_CS = 0.00625;
		if (objecte == C_BSPLINE) {
			deleteVAOList(CRV_BSPLINE);		//Eliminar VAO anterior.
			//Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
			Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
			}
			else if (objecte == C_BEZIER) {
				deleteVAOList(CRV_BEZIER);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_BEZIER, load_Bezier_Curve_VAO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
				Set_VAOList(CRV_BEZIER, load_Bezier_Curve_EBO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
			}
			else if (objecte == C_LEMNISCATA) {
				deleteVAOList(CRV_LEMNISCATA3D);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_VAO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.
				Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_EBO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.	
			}
			else if (objecte == C_HERMITTE) {
				deleteVAOList(CRV_HERMITTE);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
				Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
				}
			else if (objecte == C_CATMULL_ROM) {
				deleteVAOList(CRV_CATMULL_ROM);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
				Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
				}
			break;

	// Tecla PgUp ('9') (incrementar pas_CS)
	case VK_PRIOR:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > 0.5) pas_CS = 0.5;
		if (objecte == C_BSPLINE) {
			deleteVAOList(CRV_BSPLINE);		//Eliminar VAO anterior.
			//Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
			Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
			}
			else if (objecte == C_BEZIER) {
				deleteVAOList(CRV_BEZIER);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_BEZIER, load_Bezier_Curve_VAO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
				Set_VAOList(CRV_BEZIER, load_Bezier_Curve_EBO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
				}
			else if (objecte == C_LEMNISCATA) {
				deleteVAOList(CRV_LEMNISCATA3D);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_EBO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.
				Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_EBO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.
				}
			else if (objecte == C_HERMITTE) {
				deleteVAOList(CRV_HERMITTE);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
				Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
				}
			else if (objecte == C_CATMULL_ROM) {
				deleteVAOList(CRV_CATMULL_ROM);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
				Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
				}
		break;

	// Tecla PgDown ('3') (decrementar pas_CS)
	case VK_NEXT:
		pas_CS = pas_CS / 2;
		if (pas_CS < 0.0125) pas_CS = 0.00625;
		if (objecte == C_BSPLINE) {
			deleteVAOList(CRV_BSPLINE);		//Eliminar VAO anterior.
			//Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
			Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
			}
			else if (objecte == C_BEZIER) {
				deleteVAOList(CRV_BEZIER);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_BEZIER, load_Bezier_Curve_VAO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
				Set_VAOList(CRV_BEZIER, load_Bezier_Curve_EBO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
				}
			else if (objecte == C_LEMNISCATA) {
				deleteVAOList(CRV_LEMNISCATA3D);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_VAO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.
				Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_EBO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.
				}
			else if (objecte == C_HERMITTE) {
				deleteVAOList(CRV_HERMITTE);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
				Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
				}
			else if (objecte == C_CATMULL_ROM) {
				deleteVAOList(CRV_CATMULL_ROM);		//Eliminar VAO anterior.
				//Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
				Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
				}
		break;

	default:
		if (transf)
		{	if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		else if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL RATOLI                              */
/* ------------------------------------------------------------------------- */

// OnLButtonDown: Funció que es crida quan s'apreta el botó esquerra del mouse.
void CEntornVGIView::OnLButtonDown(UINT nFlags, CPoint point)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado

// Entorn VGI: Detectem en quina posició s'ha apretat el botó esquerra del
//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoEAvall
	m_ButoEAvall = true;
	m_PosEAvall = point;
	m_EsfeEAvall = OPV;

	CView::OnLButtonDown(nFlags, point);
}


// OnLButtonDown: Funció que es crida quan deixem d'apretar el botó esquerra del mouse.
void CEntornVGIView::OnLButtonUp(UINT nFlags, CPoint point)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
// Entorn VGI: Desactivem flag m_ButoEAvall quan deixem d'apretar botó esquerra del mouse.
	m_ButoEAvall = false;

// OPCIÓ VISTA-->SATÈLIT: Càlcul increment desplaçament del Punt de Vista
	if ((satelit) && (projeccio != ORTO))
	{	//m_EsfeIncEAvall.R = m_EsfeEAvall.R - OPV.R;
		if (camera == CAM_ESFERICA) {
			m_EsfeIncEAvall.alfa = 0.01f * (OPV.alfa - m_EsfeEAvall.alfa); //if (abs(m_EsfeIncEAvall.alfa)<0.01) { if ((m_EsfeIncEAvall.alfa)>0.0) m_EsfeIncEAvall.alfa = 0.01 else m_EsfeIncEAvall.alfa=0.01}
			m_EsfeIncEAvall.beta = 0.01f * (OPV.beta - m_EsfeEAvall.beta);
			if (abs(m_EsfeIncEAvall.beta) < 0.01) {
				if ((m_EsfeIncEAvall.beta) > 0.0) m_EsfeIncEAvall.beta = 0.01;
				else m_EsfeIncEAvall.beta = 0.01;
				}
			}
		else if (camera == CAM_GEODE) {
					m_EsfeIncEAvall.alfa = 0.01f * (OPV_G.alfa - m_EsfeEAvall.alfa); //if (abs(m_EsfeIncEAvall.alfa)<0.01) { if ((m_EsfeIncEAvall.alfa)>0.0) m_EsfeIncEAvall.alfa = 0.01 else m_EsfeIncEAvall.alfa=0.01}
					m_EsfeIncEAvall.beta = 0.01f * (OPV_G.beta - m_EsfeEAvall.beta);
					if (abs(m_EsfeIncEAvall.beta) < 0.01) {
					if ((m_EsfeIncEAvall.beta) > 0.0) m_EsfeIncEAvall.beta = 0.01;
						else m_EsfeIncEAvall.beta = 0.01;
						}
					}
		if ((m_EsfeIncEAvall.R == 0.0) && (m_EsfeIncEAvall.alfa == 0.0) && (m_EsfeIncEAvall.beta == 0.0)) KillTimer(WM_TIMER);
		else SetTimer(WM_TIMER, 10, NULL);
	}

	CView::OnLButtonUp(nFlags, point);
}


// OnLButtonDown: Funció que es crida quan s'apreta el botó dret del mouse.
void CEntornVGIView::OnRButtonDown(UINT nFlags, CPoint point)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
// Entorn VGI: Detectem en quina posició s'ha apretat el botó esquerra del
//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoDAvall
	m_ButoDAvall = true;
	m_PosDAvall = point;

	CView::OnRButtonDown(nFlags, point);
}


// OnLButtonDown: Funció que es crida quan deixem d'apretar el botó dret del mouse.
void CEntornVGIView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
// Entorn VGI: Desactivem flag m_ButoEAvall quan deixem d'apretar botó esquerra del mouse.
	m_ButoDAvall = false;

//	ClientToScreen(&point);
//	OnContextMenu(this, point);

// Si fullscreen afegir menú per a restaurar Pantalla Normal en posició del cursor.
	if (fullscreen) {
		ClientToScreen(&point);
		if (ContextMenu->GetSubMenu(0)){
			CMenu *pSubMenu = ContextMenu->GetSubMenu(0);

			if (fullscreen)	pSubMenu->CheckMenuItem(ID_VISTA_FULLSCREEN, MF_CHECKED);
			else pSubMenu->CheckMenuItem(ID_VISTA_FULLSCREEN, MF_UNCHECKED);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, this);
		}
	}
}


// OnMouseMove: Funció que es crida quan es mou el mouse. La utilitzem per la 
//				  Visualització Interactiva amb les tecles del mouse apretades per 
//				  modificar els paràmetres de P.V. (R,angleh,anglev) segons els 
//				  moviments del mouse.
//      PARAMETRES: - nFlags: Flags que controlen si el botó es apretat o no.
//					- point: Estructura (x,y) que dóna la posició del mouse 
//							 (coord. pantalla) quan el botó s'ha apretat.
void CEntornVGIView::OnMouseMove(UINT nFlags, CPoint point)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0.0, 0.0, 0.0 };

// TODO: Add your message handler code here and/or call default
	if (m_ButoEAvall && mobil && projeccio != CAP)
	{	
// Entorn VGI: Determinació dels angles (en graus) segons l'increment
//				horitzontal i vertical de la posició del mouse per càmeres Esfèrica i Geode.
		CSize gir = m_PosEAvall - point;
		m_PosEAvall = point;
		if (camera == CAM_ESFERICA)
		{	// Càmera Esfèrica
			OPV.beta = OPV.beta - gir.cx / 2.0;
			OPV.alfa = OPV.alfa + gir.cy / 2.0;

			// Entorn VGI: Control per evitar el creixement desmesurat dels angles.
			while (OPV.alfa >= 360.0)	OPV.alfa = OPV.alfa - 360.0;
			while (OPV.alfa < 0.0)		OPV.alfa = OPV.alfa + 360.0;
			while (OPV.beta >= 360.0)	OPV.beta = OPV.beta - 360.0;
			while (OPV.beta < 0.0)		OPV.beta = OPV.beta + 360.0;
		}
		else { // Càmera Geode
				OPV_G.beta = OPV_G.beta + gir.cx / 2.0;
				OPV_G.alfa = OPV_G.alfa + gir.cy / 2.0;
				// Entorn VGI: Control per evitar el creixement desmesurat dels angles
				while (OPV_G.alfa >= 360.0)	OPV_G.alfa = OPV_G.alfa - 360.0;
				while (OPV_G.alfa < 0.0f)		OPV_G.alfa = OPV_G.alfa + 360.0;
				while (OPV_G.beta >= 360.0)		OPV_G.beta = OPV_G.beta - 360.0;
				while (OPV_G.beta < 0.0)		OPV_G.beta = OPV_G.beta + 360.0;
		}
		InvalidateRect(NULL, false);
	}
	else if (m_ButoEAvall && camera == CAM_NAVEGA && (projeccio != CAP && projeccio != ORTO)) // Opció Navegació
	{
// Entorn VGI: Canviar orientació en opció de Navegació
		CSize girn = m_PosEAvall - point;
		angleZ = girn.cx / 2.0;
// Entorn VGI: Control per evitar el creixement desmesurat dels angles.
		if (angleZ >= 360.0) angleZ = angleZ - 360.0;
		if (angleZ < 0.0)	angleZ = angleZ + 360.0;

		// Entorn VGI: Segons orientació dels eixos Polars (Vis_Polar)
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			n[0] = n[0] - opvN.x;
			n[1] = n[1] - opvN.y;
			n[0] = n[0] * cos(angleZ * PI / 180.0) - n[1] * sin(angleZ * PI / 180.0);
			n[1] = n[0] * sin(angleZ * PI / 180.0) + n[1] * cos(angleZ * PI / 180.0);
			n[0] = n[0] + opvN.x;
			n[1] = n[1] + opvN.y;
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			n[2] = n[2] - opvN.z;
			n[0] = n[0] - opvN.x;
			n[2] = n[2] * cos(angleZ * PI / 180.0) - n[0] * sin(angleZ * PI / 180.0);
			n[0] = n[2] * sin(angleZ * PI / 180.0) + n[0] * cos(angleZ * PI / 180.0);
			n[2] = n[2] + opvN.z;
			n[0] = n[0] + opvN.x;
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			n[1] = n[1] - opvN.y;
			n[2] = n[2] - opvN.z;
			n[1] = n[1] * cos(angleZ * PI / 180.0) - n[2] * sin(angleZ * PI / 180.0);
			n[2] = n[1] * sin(angleZ * PI / 180.0) + n[2] * cos(angleZ * PI / 180.0);
			n[1] = n[1] + opvN.y;
			n[2] = n[2] + opvN.z;
		}

		m_PosEAvall = point;
		InvalidateRect(NULL, false);
	}

// Entorn VGI: Transformació Geomètrica interactiva pels eixos X,Y boto esquerra del mouse.
	else {
		bool transE = transX || transY;
		if (m_ButoEAvall && transE && transf)
		{
			// Calcular increment
			CSize girT = m_PosEAvall - point;
			if (transX)
			{	long int incrT = girT.cx;
				if (trasl)
				{	TG.VTras.x += incrT*fact_Tras;
					if (TG.VTras.x<-100000.0) TG.VTras.x = 100000.0;
					if (TG.VTras.x>100000.0) TG.VTras.x = 100000.0;
				}
				else if (rota)
				{	TG.VRota.x += incrT*fact_Rota;
					while (TG.VRota.x >= 360.0) TG.VRota.x -= 360.0;
					while (TG.VRota.x < 0.0) TG.VRota.x += 360.0;
				}
				else if (escal)
				{	if (incrT<0) incrT = -1 / incrT;
					TG.VScal.x = TG.VScal.x*incrT;
					if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
					if (TG.VScal.x > 8192.0) TG.VScal.x = 8192;
				}
			}
			if (transY)
			{	long int incrT = girT.cy;
				if (trasl)
					{	TG.VTras.y += incrT*fact_Tras;
						if (TG.VTras.y<-100000.0) TG.VTras.y = 100000;
						if (TG.VTras.y>100000.0) TG.VTras.y = 100000;
						}
				else if (rota)
					{	TG.VRota.y += incrT*fact_Rota;
						while (TG.VRota.y >= 360.0) TG.VRota.y -= 360;
						while (TG.VRota.y < 0.0) TG.VRota.y += 360;
						}
				else if (escal)
					{	if (incrT <= 0.0) {	if (incrT >= -2) incrT = -2;
											incrT = 1 / Log2(-incrT);
											}
						  else incrT = Log2(incrT);
						TG.VScal.y = TG.VScal.y*incrT;
						if (TG.VScal.y<0.25) TG.VScal.y = 0.25;
						if (TG.VScal.y>8192) TG.VScal.y = 8192.0;
					}
			}
			m_PosEAvall = point;
			InvalidateRect(NULL, false);
		}
	}

// Entorn VGI: Determinació del desplaçament del pan segons l'increment
//				vertical de la posició del mouse (tecla dreta apretada).
	if (m_ButoDAvall && pan && (projeccio != CAP && projeccio != ORTO))
	{
		CSize zoomincr = m_PosDAvall - point;
		long int incrx = zoomincr.cx;
		long int incry = zoomincr.cy;

// Desplaçament pan vertical
		tr_cpv.y -= incry*fact_pan;
		if (tr_cpv.y > 100000.0) tr_cpv.y = 100000.0;
		  else if (tr_cpv.y<-100000) tr_cpv.y = -100000.0;

// Desplaçament pan horitzontal
		tr_cpv.x += incrx*fact_pan;
		if (tr_cpv.x > 100000.0) tr_cpv.x = 100000;
		  else if (tr_cpv.x <- 100000.0) tr_cpv.x = -100000;

		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}
// Determinació del paràmetre R segons l'increment
//   vertical de la posició del mouse (tecla dreta apretada)
	//else if (m_ButoDAvall && zzoom && (projeccio!=CAP && projeccio!=ORTO))
	else if (m_ButoDAvall && (zzoom || zzoomO) && (projeccio != CAP))
	{	CSize zoomincr = m_PosDAvall - point;
		long int incr = zoomincr.cy / 1.0;
		if (camera == CAM_ESFERICA) {	// Càmera Esfèrica
										OPV.R = OPV.R + incr;
										if (OPV.R < 0.25) OPV.R = 0.25;
									}
		else { // Càmera Geode
				OPV_G.R = OPV_G.R + incr;
				if (OPV_G.R < 0.0) OPV_G.R = 0.0;
			}
		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}
	else if (m_ButoDAvall && camera == CAM_NAVEGA && (projeccio != CAP && projeccio != ORTO))
	{	// Avançar en opció de Navegació
		if (m_PosDAvall != point)
		{	CSize zoomincr = m_PosDAvall - point;

			double incr = zoomincr.cy / 2;

			vdir[0] = n[0] - opvN.x;
			vdir[1] = n[1] - opvN.y;
			vdir[2] = n[2] - opvN.z;
			modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
			vdir[0] = vdir[0] / modul;
			vdir[1] = vdir[1] / modul;
			vdir[2] = vdir[2] / modul;

			// Entorn VGI: Segons orientació dels eixos Polars (Vis_Polar)
			if (Vis_Polar == POLARZ) { // (X,Y,Z)
				opvN.x += incr * vdir[0];
				opvN.y += incr * vdir[1];
				n[0] += incr * vdir[0];
				n[1] += incr * vdir[1];
				}
			else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
				opvN.z += incr * vdir[2];
				opvN.x += incr * vdir[0];
				n[2] += incr * vdir[2];
				n[0] += incr * vdir[0];
				}
			else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
				opvN.y += incr * vdir[1];
				opvN.z += incr * vdir[2];
				n[1] += incr * vdir[1];
				n[2] += incr * vdir[2];
				}

			m_PosDAvall = point;
			InvalidateRect(NULL, false);
		}
	}

// Entorn VGI: Transformació Geomètrica interactiva per l'eix Z amb boto dret del mouse.
	else if (m_ButoDAvall && transZ && transf)
	{
		// Calcular increment
		CSize girT = m_PosDAvall - point;
		long int incrT = girT.cy;
		if (trasl)
		{	TG.VTras.z += incrT*fact_Tras;
			if (TG.VTras.z < -100000.0) TG.VTras.z = 100000.0;
			if (TG.VTras.z > 100000.0) TG.VTras.z = 100000.0;
		}
		else if (rota)
		{	incrT = girT.cx;
			TG.VRota.z += incrT*fact_Rota;
			while (TG.VRota.z >= 360.0) TG.VRota.z -= 360.0;
			while (TG.VRota.z<0) TG.VRota.z += 360.0;
		}
		else if (escal)
		{	if (incrT <= 0.0) {	if (incrT >= -2) incrT = -2;
								incrT = 1 / Log2(-incrT);
							  }
			  else incrT = Log2(incrT);
			TG.VScal.z = TG.VScal.z*incrT;
			if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
			if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		}

		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}

// Do not call CView::OnPaint() for painting messages
	CView::OnMouseMove(nFlags, point);
}

// OnMouseWheel: Funció que es crida quan es mou el rodet del mouse. La utilitzem per la 
//				  Visualització Interactiva per modificar el paràmetre R de P.V. (R,angleh,anglev) 
//				  segons el moviment del rodet del mouse.
//      PARAMETRES: - nFlags: Flags que controlen si el botó es apretat o no.
//					- zDelta: Unitats de desplaçament del rodet del mouse.
//					- pt: Estructura (x,y) que dóna la posició del mouse 
//							 (coord. pantalla) quan el botó s'ha apretat.
BOOL CEntornVGIView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0.0, 0.0, 0.0 };

// Funció de zoom quan està activada la funció pan o les T. Geomètriques
	if ((zzoom || zzoomO) || (transX) || (transY) || (transZ))
	{	if (camera == CAM_ESFERICA) {	// Càmera Esfèrica
										OPV.R = OPV.R + zDelta / 4.0;
										if (OPV.R < 0.25) OPV.R = 0.25;
										InvalidateRect(NULL, false);
									}
		else if (camera == CAM_GEODE)
			{	// Càmera Geode
				OPV_G.R = OPV_G.R + zDelta / 4.0;
				if (OPV_G.R < 0.0) OPV_G.R = 0.0;
				InvalidateRect(NULL, false);
			}
	}
	else if (camera == CAM_NAVEGA)
	{	vdir[0] = n[0] - opvN.x;
		vdir[1] = n[1] - opvN.y;
		vdir[2] = n[2] - opvN.z;
		modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
		vdir[0] = vdir[0] / modul;
		vdir[1] = vdir[1] / modul;
		vdir[2] = vdir[2] / modul;

		// Entorn VGI: Segons orientació dels eixos Polars (Vis_Polar)
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			opvN.x += (zDelta / 4.0) * vdir[0];
			opvN.y += (zDelta / 4.0) * vdir[1];
			n[0] += (zDelta / 4.0) * vdir[0];
			n[1] += (zDelta / 4.0) * vdir[1];
			}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			opvN.z += (zDelta / 4.0) * vdir[2];
			opvN.x += (zDelta / 4.0) * vdir[0];
			n[2] += (zDelta / 4.0) * vdir[2];
			n[0] += (zDelta / 4.0) * vdir[0];
			}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			opvN.y += (zDelta / 4.0) * vdir[1];
			opvN.z += (zDelta / 4.0) * vdir[2];
			n[1] += (zDelta / 4.0) * vdir[1];
			n[2] += (zDelta / 4.0) * vdir[2];
			}
		InvalidateRect(NULL, false);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


/* ------------------------------------------------------------------------- */
/*					     TIMER (ANIMACIÓ)									 */
/* ------------------------------------------------------------------------- */
void CEntornVGIView::OnTimer(UINT_PTR nIDEvent)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	if (anima)	{
		// Codi de tractament de l'animació quan transcorren els ms. del crono.

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
		}
	else if (satelit)	{	// OPCIÓ SATÈLIT: Increment OPV segons moviments mouse.
		if (camera == CAM_ESFERICA) {
			//OPV.R = OPV.R + m_EsfeIncEAvall.R;
			OPV.alfa = OPV.alfa + m_EsfeIncEAvall.alfa;
			while (OPV.alfa > 360.0) OPV.alfa = OPV.alfa - 360;	while (OPV.alfa < 0.0) OPV.alfa = OPV.alfa + 360;
			OPV.beta = OPV.beta + m_EsfeIncEAvall.beta;
			while (OPV.beta > 360.0) OPV.beta = OPV.beta - 360;	while (OPV.beta < 0) OPV.beta = OPV.beta + 360.0;
			}
		else if (camera == CAM_GEODE) {
				//OPV.R = OPV.R + m_EsfeIncEAvall.R;
				OPV_G.alfa = OPV_G.alfa + m_EsfeIncEAvall.alfa * 0.5;
				while (OPV_G.alfa > 360.0) OPV_G.alfa = OPV_G.alfa - 360.0;	
				while (OPV_G.alfa < 0) OPV_G.alfa = OPV_G.alfa + 360.0;
				OPV_G.beta = OPV_G.beta + m_EsfeIncEAvall.beta * 0.5;
				while (OPV_G.beta > 360.0) OPV_G.beta = OPV.beta - 360.0;
				while (OPV_G.beta < 0.0) OPV_G.beta = OPV_G.beta + 360.0;
				}

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
		}

	CView::OnTimer(nIDEvent);
}


/* ------------------------------------------------------------------------- */
/*   RECURSOS DE MENU (persianes) DE L'APLICACIO:                            */
/*					1. ARXIUS												 */
/*					4. CÀMERA: Esfèrica (Mobil, Zoom, ZoomO, Satelit), Navega*/
/*					5. VISTA: Pan, Eixos i Grid							     */
/*					6. PROJECCIÓ                                             */
/*					7. OBJECTE					                             */
/*					8. TRANSFORMA											 */
/*					9. OCULTACIONS											 */
/*				   10. IL.LUMINACIÓ											 */
/*				   11. LLUMS												 */
/*				   12. SHADERS												 */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/*					1. ARXIUS 												 */
/* ------------------------------------------------------------------------- */

// Obrir fitxer Fractal
void CEntornVGIView::OnArxiuObrirFractal()
{
// TODO: Agregue aquí su código de controlador de comandos
// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.MNT)
	CFileDialog openMunt(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("MNT Files(*.mnt)|*.mnt|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openMunt.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openMunt.GetPathName();

// Entorn VGI: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char *nomfitx = CString2Char(nom);

// Entorn VGI: Variable de tipus char *nomfitx conté el nom del fitxer seleccionat

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// OnArchivoObrirFitxerObj: Obrir fitxer en format gràfic OBJ
void CEntornVGIView::OnArxiuObrirFitxerObj()
{
// TODO: Agregue aquí su código de controlador de comandos
//if (ObOBJ != NULL) delete ObOBJ;

	objecte = OBJOBJ;	textura = true;		tFlag_invert_Y = false;

// Entorn VGI: Obrir diàleg de lectura de fitxer
	CFileDialog openOBJ(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("OBJ Files(*.obj)|*.obj|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openOBJ.DoModal() != IDOK)	return;  // stay with old data file
	else nom = openOBJ.GetPathName();

// Entorn VGI: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char *nomfitx = CString2Char(nom);

// i carreguem
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL

	if (ObOBJ == NULL) ObOBJ = ::new COBJModel;
	else { // Si instància ja s'ha utilitzat en un objecte OBJ
		ObOBJ->netejaVAOList_OBJ();		// Netejar VAO, EBO i VBO
		ObOBJ->netejaTextures_OBJ();	// Netejar buffers de textures
		}

	int error = ObOBJ->LoadModel(nomfitx);			// Carregar objecte OBJ amb textura com a varis VAO's

//	Pas de paràmetres textura al shader
	glUniform1i(glGetUniformLocation(shader_programID, "textur"), textura);
	glUniform1i(glGetUniformLocation(shader_programID, "flag_invert_y"), tFlag_invert_Y);

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Desactivem contexte OpenGL

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}

// Obrir fitxer que conté paràmetres Font de Llum (fitxers .lght)
void CEntornVGIView::OnArxiuObrirFitxerFontLlum()
{
// TODO: Agregue aquí su código de controlador de comandos
 
// Entorn VGI: Obrir diàleg de lectura de fitxer
	CFileDialog openSourcL(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("LGHT Files(*.lght)|*.lght|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openSourcL.DoModal() != IDOK)	return;  // stay with old data file
	else nom = openSourcL.GetPathName();

// Entorn VGI: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	bool err=llegir_FontLlum(nomfitx); // Llegir Fitxer de Paràmetres Font de Llum

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// Obrir fitxers del SkyBox
void CEntornVGIView::OnArxiuObrirSkybox()
{
// TODO: Agregue aquí su código de controlador de comandos
	CString folderPath;
	std::vector<std::string> faces;

// Entorn VGI: Obrir diàleg de lectura de directori (carpeta) on estan els fitxers del skybox
	CFolderPickerDialog folderPickerDialog(NULL, OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, this,
		sizeof(OPENFILENAME));

// EntornVGI: En variable folderPath tenim la carpeta que contçe els fitxers de Skybox
	if (folderPickerDialog.DoModal() == IDOK) folderPath = folderPickerDialog.GetPathName();

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega VAO Skybox Cube
	if (skC_VAOID.vaoId == 0) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);

// Càrrega del fitxer right (+X <--> posx <--> right)
	CString facesCS = folderPath;
	facesCS += "\\right.jpg";
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString(facesCS);
	// construct a std::string using the LPCSTR input
	std::string facesS1(pszConvertedAnsiString);
	faces.push_back(facesS1);

// Càrrega del fitxer left (-X <--> negx <--> left)
	facesCS = folderPath;
	facesCS += "\\left.jpg";
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString2(facesCS);
	// Construct a std::string using the LPCSTR input
	std::string facesS2(pszConvertedAnsiString2);
	faces.push_back(facesS2);

// Càrrega del fitxer top (+Y <--> posy <--> top)
	facesCS = folderPath;
	facesCS += "\\top.jpg";
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString3(facesCS);
	// Construct a std::string using the LPCSTR input
	std::string facesS3(pszConvertedAnsiString3);
	faces.push_back(facesS3);

// Càrrega del fitxer bottom (-Y <--> negy <--> bottom)
	facesCS = folderPath;
	facesCS += "\\bottom.jpg";
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString4(facesCS);
	// Construct a std::string using the LPCSTR input
	std::string facesS4(pszConvertedAnsiString4);
	faces.push_back(facesS4);

// Càrrega del fitxer front (+Z <--> posz <--> front)
	facesCS = folderPath;
	facesCS += "\\front.jpg";
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString5(facesCS);
	// Construct a std::string using the LPCSTR input
	std::string facesS5(pszConvertedAnsiString5);
	faces.push_back(facesS5);

// Càrrega del fitxer back (-Z <--> negz <--> back)
	facesCS = folderPath;
	facesCS += "\\back.jpg";
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString6(facesCS);
	// construct a std::string using the LPCSTR input
	std::string facesS6(pszConvertedAnsiString6);
	faces.push_back(facesS6);

/*
		if (!cubemapTexture)
		{	// load Skybox textures
			// -------------
			std::vector<std::string> faces =
			{ ".\\textures\\skybox\\right.jpg",
				".\\textures\\skybox\\left.jpg",
				".\\textures\\skybox\\top.jpg",
				".\\textures\\skybox\\bottom.jpg",
				".\\textures\\skybox\\front.jpg",
				".\\textures\\skybox\\back.jpg"
			};
*/
	cubemapTexture = loadCubemap(faces);
//	}

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


/* ------------------------------------------------------------------------- */
/*					4. CÀMERA: Esfèrica (Mobil, Zoom, ZoomO, Satelit), Navega*/
/* ------------------------------------------------------------------------- */
// CÀMERA: Mode Esfèrica (Càmera esfèrica en polars-opció booleana)
void CEntornVGIView::OnCameraEsferica()
{
// TODO: Agregue aquí su código de controlador de comandos
	camera = CAM_ESFERICA;

// Inicialitzar paràmetres Càmera Esfèrica
	OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;				// Origen PV en esfèriques
	mobil = true;		zzoom = true;		satelit = false;
	Vis_Polar = POLARZ;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateCameraEsferica(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (camera == CAM_ESFERICA) 	pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// CÀMERA--> ESFERICA: Mobil. Punt de Vista Interactiu (opció booleana)
void CEntornVGIView::OnVistaMobil()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != ORTO) || (projeccio != CAP) && (camera == CAM_ESFERICA || camera==CAM_GEODE))  mobil = !mobil;
// Desactivació de Transformacions Geomètriques via mouse 
//		si Visualització Interactiva activada.	
	if (mobil) {
		transX = false;	transY = false; transZ = false;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaMobil(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (mobil) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// CÀMERA--> ESFERICA: Zoom. Zoom Interactiu (opció booleana)
void CEntornVGIView::OnVistaZoom()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio == PERSPECT) && (camera == CAM_ESFERICA || camera == CAM_GEODE)) zzoom = !zzoom;
// Desactivació de Transformacions Geomètriques via mouse 
//		si Zoom activat.
	if (zzoom) {
		transX = false;	transY = false;	transZ = false;
		zzoomO = false;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaZoom(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (zzoom) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// CÀMERA--> ESFERICA: Zoom Orto. Zoom Interactiu en Ortogràfica (opció booleana)
void CEntornVGIView::OnVistaZoomOrto()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio == ORTO) || (projeccio == AXONOM) && (camera == CAM_ESFERICA || camera == CAM_GEODE)) zzoomO = !zzoomO;
	// Desactivació de Transformacions Geomètriques via mouse 
	//	si Zoom activat
	if (zzoomO) {
		zzoom = false;
		transX = false;	transY = false;	transZ = false;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateVistaZoomOrto(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (zzoomO) pCmdUI->SetCheck(1);
}


// CÀMERA--> ESFERICA: Satèlit. Vista interactiva i animada en que increment de movimen és activat per mouse (opció booleana)
void CEntornVGIView::OnVistaSatelit()
{
	// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != CAP && projeccio != ORTO) && (camera == CAM_ESFERICA || camera == CAM_GEODE)) satelit = !satelit;
	if (satelit) {	mobil = true;
					m_EsfeIncEAvall.alfa = 0.0;
					m_EsfeIncEAvall.beta = 0.0;
				}
	bool testA = anima;									// Testejar si hi ha alguna animació activa apart de Satèlit.
	if ((!satelit) && (!testA)) KillTimer(WM_TIMER);	// Si es desactiva Satèlit i no hi ha cap animació activa es desactiva el Timer.

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaSatelit(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (satelit) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// CÀMERA--> ESFERICA: Polars Eix X cap amunt per a Visualització Interactiva
void CEntornVGIView::OnVistaPolarsX()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (projeccio != CAP) Vis_Polar = POLARX;

// EntornVGI: Inicialitzar la càmera en l'opció NAVEGA (posició i orientació eixos)
	if (camera == CAM_NAVEGA) {
		opvN.x = 0.0;	opvN.y = 10.0;	opvN.z = 0.0;	 // opvN = (0,10,0)
		n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
		angleZ = 0.0;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaPolarsX(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (Vis_Polar == POLARX) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// CÀMERA--> ESFERICA: Polars Eix Y cap amunt per a Visualització Interactiva
void CEntornVGIView::OnVistaPolarsY()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (projeccio != CAP) Vis_Polar = POLARY;

// EntornVGI: Inicialitzar la càmera en l'opció NAVEGA (posició i orientació eixos)
	if (camera == CAM_NAVEGA) {
		opvN.x = 0.0;	opvN.y = 0.0;	opvN.z = 10.0; // opvN = (0,0,10)
		n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
		angleZ = 0.0;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaPolarsY(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (Vis_Polar == POLARY) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// CÀMERA--> ESFERICA: Polars Eix Z cap amunt per a Visualització Interactiva
void CEntornVGIView::OnVistaPolarsZ()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (projeccio != CAP) Vis_Polar = POLARZ;

// EntornVGI: Inicialitzar la càmera en l'opció NAVEGA (posició i orientació eixos)
	if (camera == CAM_NAVEGA) {
		opvN.x = 10.0;	opvN.y = 0.0;	opvN.z = 0.0; // opvN = (10,0,0)
		n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
		angleZ = 0.0;
		}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaPolarsZ(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (Vis_Polar == POLARZ) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// CÀMERA--> NAVEGA:  Mode de navegació sobre un pla amb botons de teclat o de mouse (nav) (opció booleana)
void CEntornVGIView::OnVistaNavega()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (projeccio != ORTO) 
	{	camera = CAM_NAVEGA;
		// Desactivació de zoom, mobil, Transformacions Geomètriques via mouse i pan 
		//		si navega activat
		mobil = false;	zzoom = false;	satelit = false;
		transX = false;	transY = false;	transZ = false;
		//pan = false;
		tr_cpv.x = 0.0;		tr_cpv.y = 0.0;		tr_cpv.z = 0.0;		// Inicialitzar a 0 desplaçament de pantalla
		tr_cpvF.x = 0.0;	tr_cpvF.y = 0.0;	tr_cpvF.x = 0.0;	// Inicialitzar a 0 desplaçament de pantalla

		// Incialitzar variables Navega segons configuració eixos en Polars
		if (Vis_Polar == POLARZ) {
			opvN.x = 10.0;	opvN.y = 0.0;	opvN.z = 0.0; // opvN = (10,0,0)
			n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
			angleZ = 0.0;
		}
		else if (Vis_Polar == POLARY) {
			opvN.x = 0.0;	opvN.y = 0.0;	opvN.z = 10.0; // opvN = (10,0,0)
			n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
			angleZ = 0.0;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x = 0.0;	opvN.y = 10.0;	opvN.z = 0.0;	 // opvN = (0,10,0)
			n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
			angleZ = 0.0;
		}
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaNavega(CCmdUI* pCmdUI)
{
	// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (camera == CAM_NAVEGA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// Tornar a lloc d'origen
void CEntornVGIView::OnVistaOrigennavega()
{
	// TODO: Agregue aquí su código de controlador de comandos
	if (camera == CAM_NAVEGA) {	n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
								opvN.x = 10.0;	opvN.y = 0.0;		opvN.z = 0.0;
								angleZ = 0.0;
							}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// CÀMERA--> GEODE:  Mode de navegació centrat a l'origent mirant un punt en coord. esfèriques (R,alfa,beta) (opció booleana)
void CEntornVGIView::OnCameraGeode()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (projeccio != ORTO || projeccio != CAP) camera = CAM_GEODE;

// Inicialitzar paràmetres Càmera Geode
	OPV_G.R = 0.0;		OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;				// Origen PV en esfèriques
	mobil = true;		zzoom = true;		satelit = false;	pan = false;
	Vis_Polar = POLARZ;
	llumGL[5].encesa = true;

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL
	glFrontFace(GL_CW);
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL); // Desactivem contexte OpenGL

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateCameraGeode(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (camera == CAM_GEODE) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


void CEntornVGIView::OnCameraOrigenGeode()
{
// TODO: Agregue aquí su código de controlador de comandos
// Inicialitzar paràmetres Càmera Geode
	OPV_G.R = 0.0;	OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;				// Origen PV en esfèriques
	mobil = true;	zzoom = true;		zzoomO = false;		 satelit = false;	pan = false;
	Vis_Polar = POLARZ;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


/* -------------------------------------------------------------------------------- */
/*					5. VISTA: Pantalla Completa, Pan i Eixos	                    */
/* -------------------------------------------------------------------------------- */
// VISTA: FullScreen (Pantalla Completa-opció booleana)
void CEntornVGIView::OnVistaFullscreen()
{
// TODO: Agregue aquí su código de controlador de comandos

	if (!fullscreen)
		{	// I note that I go to full-screen mode
			fullscreen = true;
			// Remembers the address of the window in which the view was placed (probably a frame)
			saveParent = this->GetParent();
			// Assigns a view to a new parent - desktop
			this->SetParent(GetDesktopWindow());
			CRect rect; // it's about the dimensions of the desktop-desktop
			GetDesktopWindow()->GetWindowRect(&rect);
			// I set the window on the desktop
			MoveWindow(rect);
		}
	else {	// Switching off the full-screen mode
			fullscreen = false;
			// Assigns an old parent view
			this->SetParent(saveParent);
			CRect rect; // It's about the dimensions of the desktop-desktop
			// Get client screen dimensions
			saveParent->GetClientRect(&rect);
			// Changes the position and dimensions of the specified window.
			MoveWindow(rect, FALSE);
		}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaFullscreen(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (fullscreen) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// VISTA: Mode de Desplaçament horitzontal i vertical per pantalla del Punt de Vista (pan) (opció booleana)
void CEntornVGIView::OnVistaPan()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != ORTO) || (projeccio != CAP)) pan = !pan;
// Desactivació de Transformacions Geomètriques via mouse i navega si pan activat
	if (pan)	{	mobil = true;		zzoom = true;
					transX = false;		transY = false;		transZ = false;
					//navega = false;
				}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaPan(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (pan) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}

// Tornar a lloc d'origen
void CEntornVGIView::OnVistaOrigenpan()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (pan) {	fact_pan = 1;
				tr_cpv.x = 0;	tr_cpv.y = 0;	tr_cpv.z = 0;
			}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}



// VISTA: Visualitzar eixos coordenades món (opció booleana)
void CEntornVGIView::OnVistaEixos()
{
// TODO: Agregue aquí su código de controlador de comandos
	eixos = !eixos;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateVistaEixos(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (eixos) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// SKYBOX: Visualitzar Skybox en l'escena (opció booleana)
void CEntornVGIView::OnVistaSkyBox()
{
// TODO: Agregue aquí su código de controlador de comandos
	SkyBoxCube = !SkyBoxCube;

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega Shader Skybox
	if (!skC_programID) skC_programID = shader_SkyBoxC.loadFileShaders(".\\shaders\\skybox.VERT", ".\\shaders\\skybox.FRAG");

// Càrrega VAO Skybox Cube
	if (skC_VAOID.vaoId==0) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);

	if (!cubemapTexture)
	{	// load Skybox textures
		// -------------
		std::vector<std::string> faces =
		{ ".\\textures\\skybox\\right.jpg",
			".\\textures\\skybox\\left.jpg",
			".\\textures\\skybox\\top.jpg",
			".\\textures\\skybox\\bottom.jpg",
			".\\textures\\skybox\\front.jpg",
			".\\textures\\skybox\\back.jpg"
		};
		cubemapTexture = loadCubemap(faces);
	}

// Entorn VGI: Activació el contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateVistaSkyBox(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (SkyBoxCube) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					6. PROJECCIÓ                                             */
/* ------------------------------------------------------------------------- */

// PROJECCIÓ: Perspectiva
void CEntornVGIView::OnProjeccioPerspectiva()
{
// TODO: Agregue aquí su código de controlador de comandos
	projeccio = PERSPECT;
	mobil = true;			zzoom = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}

void CEntornVGIView::OnUpdateProjeccioPerspectiva(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (projeccio == PERSPECT) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					7. OBJECTE					                             */
/* ------------------------------------------------------------------------- */

// OBJECTE: Cap objecte
void CEntornVGIView::OnObjecteCap()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = CAP;

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL

	netejaVAOList();						// Neteja Llista VAO.

	// Entorn VGI: Eliminar buffers de textures previs del vector texturesID[].
	for (int i = 0; i < NUM_MAX_TEXTURES; i++) {
		if (texturesID[i]) {	glDeleteTextures(1, &texturesID[i]);
								texturesID[i] = 0;
							}
	}

// Entorn VGI: Alliberar memòria i textures objecte OBJ, si creades.
	if (ObOBJ != NULL) {
		ObOBJ->netejaVAOList_OBJ();		// Netejar VAO, EBO i VBO
		ObOBJ->netejaTextures_OBJ();	// Netejar buffers de textures
	}

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Desactivem contexte OpenGL

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteCap(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == CAP) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE: Cub
void CEntornVGIView::OnObjecteCub()
{
// TODO: Agregue aquí su código de controlador de comandos

	objecte = CUB;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	
	netejaVAOList();											// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

	//Set_VAOList(GLUT_CUBE, loadglutSolidCube_VAO(1.0));	// Genera VAO de cub mida 1 i el guarda a la posició GLUT_CUBE.
	Set_VAOList(GLUT_CUBE, loadglutSolidCube_EBO(1.0));		// Genera EBO de cub mida 1 i el guarda a la posició GLUT_CUBE.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteCub(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == CUB) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// OBJECTE: Cub RGB
void CEntornVGIView::OnObjecteCubRGB()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = CUB_RGB;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	netejaVAOList();						// Neteja Llista VAO.

	//Set_VAOList(GLUT_CUBE_RGB, loadglutSolidCubeRGB_VAO(1.0));	// Genera VAO de cub mida 1 i el guarda a la posició GLUT_CUBE_RGB.
	Set_VAOList(GLUT_CUBE_RGB, loadglutSolidCubeRGB_EBO(1.0));	// Genera EBO de cub mida 1 i el guarda a la posició GLUT_CUBE_RGB.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteCubRGB(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == CUB_RGB) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Esfera
void CEntornVGIView::OnObjecteEsfera()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = ESFERA;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

	//Set_VAOList(GLU_SPHERE, loadgluSphere_VAO(1.0, 30,30)); // // Genera VAO d'esfera radi 1 i el guarda a la posició GLUT_CUBE_RGB.
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(1.0, 30, 30));

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteEsfera(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == ESFERA) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}

// OBJECTE Tetera
void CEntornVGIView::OnObjecteTetera()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = TETERA;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

	//if (Get_VAOId(GLUT_TEAPOT) != 0) deleteVAOList(GLUT_TEAPOT);
	Set_VAOList(GLUT_TEAPOT, loadglutSolidTeapot_VAO()); //Genera VAO tetera mida 1 i el guarda a la posició GLUT_TEAPOT.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteTetera(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == TETERA) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// --------------- OBJECTES COMPLEXES

// OBJECTE ARC
void CEntornVGIView::OnObjecteArc()
{
	CColor color_Mar;

	color_Mar.r = 0.5;	color_Mar.g = 0.4; color_Mar.b = 0.9; color_Mar.a = 1.0;
// TODO: Agregue aquí su código de controlador de comandos
	objecte = ARC;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega dels VAO's per a construir objecte ARC
	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

	//if (Get_VAOId(GLUT_CUBE) != 0) deleteVAOList(GLUT_CUBE);
	Set_VAOList(GLUT_CUBE, loadglutSolidCube_EBO(1.0));		// Càrrega Cub de costat 1 com a EBO a la posició GLUT_CUBE.

	//if (Get_VAOId(GLU_SPHERE) != 0) deleteVAOList(GLU_SPHERE);
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(0.5, 20, 20));	// Càrrega Esfera a la posició GLU_SPHERE.

	//if (Get_VAOId(GLUT_TEAPOT) != 0) deleteVAOList(GLUT_TEAPOT);
	Set_VAOList(GLUT_TEAPOT, loadglutSolidTeapot_VAO());		// Carrega Tetera a la posició GLUT_TEAPOT.

	//if (Get_VAOId(MAR_FRACTAL_VAO) != 0) deleteVAOList(MAR_FRACTAL_VAO);
	Set_VAOList(MAR_FRACTAL_VAO, loadSea_VAO(color_Mar));		// Carrega Mar a la posició MAR_FRACTAL_VAO.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteArc(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == ARC) pCmdUI->SetCheck(1);
	 else pCmdUI->SetCheck(0);
}


// OBJECTE Tie
void CEntornVGIView::OnObjecteTie()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = TIE;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega dels VAO's per a construir objecte TIE
	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

	//if (Get_VAOId(GLU_CYLINDER) != 0) deleteVAOList(GLU_CYLINDER);
	Set_VAOList(GLUT_CYLINDER, loadgluCylinder_EBO(5.0f, 5.0f, 0.5f, 6, 1));// Càrrega cilindre com a VAO.

	//if (Get_VAOId(GLU_DISK) != 0)deleteVAOList(GLU_DISK);
	Set_VAOList(GLU_DISK, loadgluDisk_EBO(0.0f, 5.0f, 6, 1));	// Càrrega disc com a VAO

	//if (Get_VAOId(GLU_SPHERE) != 0)deleteVAOList(GLU_SPHERE);
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(10.0f, 80, 80));	// Càrrega disc com a VAO

	//if (Get_VAOId(GLUT_USER1) != 0)deleteVAOList(GLUT_USER1);
	Set_VAOList(GLUT_USER1, loadgluCylinder_EBO(5.0f, 5.0f, 2.0f, 6, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_CUBE) != 0)deleteVAOList(GLUT_CUBE);
	Set_VAOList(GLUT_CUBE, loadglutSolidCube_EBO(1.0));			// Càrrega cub com a EBO

	//if (Get_VAOId(GLUT_TORUS) != 0)deleteVAOList(GLUT_TORUS);
	Set_VAOList(GLUT_TORUS, loadglutSolidTorus_EBO(1.0, 5.0, 20, 20));

	//if (Get_VAOId(GLUT_USER2) != 0)deleteVAOList(GLUT_USER2);	
	Set_VAOList(GLUT_USER2, loadgluCylinder_EBO(1.0f, 0.5f, 5.0f, 60, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_USER3) != 0)deleteVAOList(GLUT_USER3);
	Set_VAOList(GLUT_USER3, loadgluCylinder_EBO(0.35f, 0.35f, 5.0f, 80, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_USER4) != 0)deleteVAOList(GLUT_USER4);
	Set_VAOList(GLUT_USER4, loadgluCylinder_EBO(4.0f, 2.0f, 10.25f, 40, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_USER5) != 0) deleteVAOList(GLUT_USER5);
	Set_VAOList(GLUT_USER5, loadgluCylinder_EBO(1.5f, 4.5f, 2.0f, 8, 1)); // Càrrega cilindre com a VAO

	//if (Get_VAOId(GLUT_USER6) != 0) deleteVAOList(GLUT_USER6);
	Set_VAOList(GLUT_USER6, loadgluDisk_EBO(0.0f, 1.5f, 8, 1)); // Càrrega disk com a VAO

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteTie(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == TIE) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}

// ----------------- OBJECTES CORBES BEZIER, LEMNISCATA i B-SPLINE


// OBJECTE Corba Bezier
void CEntornVGIView::OnObjeteCorbaBezier()
{
// TODO: Agregue aquí su código de controlador de comandos

	nom = "";
	objecte = C_BEZIER;		sw_material[4] = true;

// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openSpline(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("SPL Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openSpline.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openSpline.GetPathName();

// Entorn VGI: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char *nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega dels VAO's per a construir la corba Bezier
	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

// Definir Esfera EBO per a indicar punts de control de la corba
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(5.0, 20, 20));	// Genera esfera i la guarda a la posició GLUT_CUBE.

// Definir Corba Bezier com a VAO
	//Set_VAOList(CRV_BEZIER, load_Bezier_Curve_VAO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.
	Set_VAOList(CRV_BEZIER, load_Bezier_Curve_EBO(npts_T, PC_t, pas_CS, false)); // Genera corba i la guarda a la posició CRV_BEZIER.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjeteCorbaBezier(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_BEZIER) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// OBJECTE Corba Lemniscata 3D
void CEntornVGIView::OnObjecteCorbaLemniscata()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = C_LEMNISCATA;		sw_material[4] = true;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega dels VAO's per a construir la corba Bezier
	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

// Definr Corba Lemniscata 3D com a VAO
	//Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_VAO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.
	Set_VAOList(CRV_LEMNISCATA3D, load_Lemniscata3D_EBO(800, pas_CS * 20.0)); // Genera corba i la guarda a la posició CRV_LEMNISCATA3D.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteCorbaLemniscata(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_LEMNISCATA) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// OBJECTE Corba Hermitte
void CEntornVGIView::OnObjecteCorbaHermitte()
{
// TODO: Agregue aquí su código de controlador de comandos
	nom = "";
	objecte = C_HERMITTE;	sw_material[4] = true;

// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openHermit(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("CRV Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openHermit.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openHermit.GetPathName();

// Entorn VGI: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega dels VAO's per a construir la corba BSpline
	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

// Definir Esfera EBO per a indicar punts de control de la corba
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(5.0, 20, 20));	// Guarda (vaoId, vboId, nVertexs) a la posició GLUT_CUBE.

// Definir Corba HERMITTE com a VAO o EBO
	//Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_HERMITTE.
	Set_VAOList(CRV_HERMITTE, load_Hermitte_Curve_EBO(npts_T, PC_t, pas_CS));	// Genera corba i la guarda a la posició CRV_HERMITTE.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}


void CEntornVGIView::OnUpdateObjecteCorbaHermitte(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_HERMITTE) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Corba Catmull Rom (interpolació per punts)
void CEntornVGIView::OnObjecteCorbaCatmullRom()
{
// TODO: Agregue aquí su código de controlador de comandos
	nom = "";
	objecte = C_CATMULL_ROM;	sw_material[4] = true;

// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openHermit(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("CRV Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openHermit.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openHermit.GetPathName();

// Entorn VGI: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega dels VAO's per a construir la corba BSpline
	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

// Definir Esfera EBO per a indicar punts de control de la corba
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(5.0, 20, 20));	// Guarda (vaoId, vboId, nVertexs) a la posició GLUT_CUBE.

// Definir Corba CATMULL ROM com a VAO o EBO
	//Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_CATMULL_ROM.
	Set_VAOList(CRV_CATMULL_ROM, load_CatmullRom_Curve_EBO(npts_T, PC_t, pas_CS));	// Genera corba i la guarda a la posició CRV_CATMULL_ROM.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}

void CEntornVGIView::OnUpdateObjecteCorbaCatmullRom(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_CATMULL_ROM) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Corba B-Spline
void CEntornVGIView::OnObjecteCorbaBSpline()
{
// TODO: Agregue aquí su código de controlador de comandos
	nom = "";
	objecte = C_BSPLINE;	sw_material[4] = true;

// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openSpline(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("CRV Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openSpline.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openSpline.GetPathName();

// Entorn VGI: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char *nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega dels VAO's per a construir la corba BSpline
	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

// Definir Esfera EBO per a indicar punts de control de la corba
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(5.0, 20, 20));	// Guarda (vaoId, vboId, nVertexs) a la posició GLUT_CUBE.

// Definr Corba BSpline com a VAO
	//Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_VAO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.
	Set_VAOList(CRV_BSPLINE, load_BSpline_Curve_EBO(npts_T, PC_t, pas_CS)); // Genera corba i la guarda a la posició CRV_BSPLINE.

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateObjecteCorbaBSpline(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_BSPLINE) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// OBJECTE Punts de Control: Activació de la visualització dels Punts de control de les Corbes (OPCIÓ BOOLEANA)
void CEntornVGIView::OnObjectePuntsControl()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_Punts_Control = !sw_Punts_Control;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjectePuntsControl(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_Punts_Control) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE --> CORBES: Activar o desactivar visualització Triedre de Frenet Punts de control de les corbes 
//					Lemniscata, Bezier i BSpline
void CEntornVGIView::OnCorbesTriedreFrenet()
{
// TODO: Agregue aquí su código de controlador de comandos
	dibuixa_TriedreFrenet = !dibuixa_TriedreFrenet;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateCorbesTriedreFrenet(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (dibuixa_TriedreFrenet) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Matriu Primitives
void CEntornVGIView::OnObjecteMatriuPrimitives()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = MATRIUP;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteMatriuPrimitives(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == MATRIUP) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Matriu Primitives VAO
void CEntornVGIView::OnObjecteMatriuPrimitivesVBO()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = MATRIUP_VAO;

//	---- Entorn VGI: ATENCIÓ!!. Canviar l'escala per a centrar la vista (Ortogràfica)

//  ---- Entorn VGI: ATENCIÓ!!. Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega dels VAO's per a construir objecte ARC
	netejaVAOList();						// Neteja Llista VAO.

// Posar color objecte (col_obj) al vector de colors del VAO.
	SetColor4d(col_obj.r, col_obj.g, col_obj.b, col_obj.a);

	//if (Get_VAOId(GLUT_CUBE) != 0) deleteVAOList(GLUT_CUBE);
	Set_VAOList(GLUT_CUBE, loadglutSolidCube_EBO(1.0f));

	//if (Get_VAOId(GLUT_TORUS) != 0)deleteVAOList(GLUT_TORUS);
	Set_VAOList(GLUT_TORUS, loadglutSolidTorus_EBO(2.0, 3.0, 20, 20));

	//if (Get_VAOId(GLUT_SPHERE) != 0) deleteVAOList(GLU_SPHERE);
	Set_VAOList(GLU_SPHERE, loadgluSphere_EBO(1.0, 20, 20));

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	
// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateObjecteMatriuPrimitivesVBO(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == MATRIUP_VAO) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					8. TRANSFORMA											 */
/* ------------------------------------------------------------------------- */

// TRANSFORMA: TRASLACIÓ
void CEntornVGIView::OnTransformaTraslacio()
{
// TODO: Agregue aquí su código de controlador de comandos
	trasl = !trasl;
	rota = false;
	if (trasl) escal = true;
	transf = trasl || rota || escal;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateTransformaTraslacio(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (trasl) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


void CEntornVGIView::OnTransformaOrigentraslacio()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (trasl)
	{	fact_Tras = 1;
		TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFORMA: ROTACIÓ
void CEntornVGIView::OnTransformaRotacio()
{
// TODO: Agregue aquí su código de controlador de comandos
	rota = !rota;
	trasl = false;
	if (rota) escal = true;
	transf = trasl || rota || escal;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaRotacio(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (rota) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


void CEntornVGIView::OnTransformaOrigenrotacio()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (rota)
	{	fact_Rota = 90;
		TG.VRota.x = 0;		TG.VRota.y = 0;		TG.VRota.z = 0;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFORMA: ESCALAT
void CEntornVGIView::OnTransformaEscalat()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((!rota) && (!trasl)) escal = !escal;
	transf = trasl || rota || escal;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaEscalat(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (escal) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


void CEntornVGIView::OnTransformaOrigenescalat()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (escal) { TG.VScal.x = 1;	TG.VScal.y = 1;	TG.VScal.z = 1; }

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFOMA: Mòbil Eix X? (opció booleana).
void CEntornVGIView::OnTransformaMobilx()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (transf)
	{	transX = !transX;
		if (transX) {	mobil = false;	zzoom = false;
						pan = false;	//navega = false;
					}
			else if ((!transY) && (!transZ)){	mobil = true;
												zzoom = true;
											}
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaMobilx(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (transX) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// TRANSFOMA: Mòbil Eix Y? (opció booleana).
void CEntornVGIView::OnTransformaMobily()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (transf)
	{	transY = !transY;
		if (transY) {	mobil = false;	zzoom = false;
						pan = false;	//navega = false;
					}
			else if ((!transX) && (!transZ)){	mobil = true;
												zzoom = true;
											}
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaMobily(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (transY) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// TRANSFOMA: Mòbil Eix Z? (opció booleana).
void CEntornVGIView::OnTransformaMobilz()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (transf)
	{	transZ = !transZ;
		if (transZ) {	mobil = false;	zzoom = false;
						pan = false;	//navega = false;
					}
		else if ((!transX) && (!transY)) {	mobil = true;
											zzoom = true;
										}
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateTransformaMobilz(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (transZ) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					9. OCULTACIONS											 */
/* ------------------------------------------------------------------------- */

void CEntornVGIView::OnOcultacionsFrontFaces()
{
// TODO: Agregue aquí su código de controlador de comandos
	front_faces = !front_faces;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateOcultacionsFrontFaces(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (front_faces) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// OCULTACIONS: Test de Visibilitat? (opció booleana).
void CEntornVGIView::OnOcultacionsTestvis()
{
// TODO: Agregue aquí su código de controlador de comandos
	test_vis = !test_vis;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateOcultacionsTestvis(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (test_vis) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// OCULTACIONS: Z-Buffer? (opció booleana).
void CEntornVGIView::OnOcultacionsZbuffer()
{
// TODO: Agregue aquí su código de controlador de comandos
	oculta = !oculta;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateOcultacionsZbuffer(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (oculta) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					10. IL.LUMINACIÓ											 */
/* ------------------------------------------------------------------------- */

// IL.LUMINACIÓ Font de llum fixe? (opció booleana).
void CEntornVGIView::OnIluminacioLlumfixe()
{
// TODO: Agregue aquí su código de controlador de comandos
	ifixe = !ifixe;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioLlumfixe(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ifixe) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// IL.LUMINACIÓ: Mantenir iluminades les Cares Front i Back
void CEntornVGIView::OnIluminacio2Sides()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilum2sides = !ilum2sides;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateIluminacio2Sides(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilum2sides) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}



// ILUMINACIÓ PUNTS
void CEntornVGIView::OnIluminacioPunts()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilumina = PUNTS;
	test_vis = false;		oculta = false;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioPunts(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilumina == PUNTS) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ FILFERROS
void CEntornVGIView::OnIluminacioFilferros()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilumina = FILFERROS;
	test_vis = false;		oculta = false;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioFilferros(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilumina == FILFERROS) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ PLANA
void CEntornVGIView::OnIluminacioPlana()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilumina = PLANA;
	test_vis = false;		oculta = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioPlana(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilumina == PLANA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ SUAU
void CEntornVGIView::OnIluminacioSuau()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilumina = SUAU;
	test_vis = false;		oculta = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateIluminacioSuau(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilumina == SUAU) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL / COLOR: Activació i desactivació de la reflectivitat pròpia del material com a color.
void CEntornVGIView::OnMaterialReflmaterial()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[4] = !sw_material[4];
	sw_il = true;

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
// Pas màscara llums
	glUniform1i(glGetUniformLocation(shader_programID, "sw_material"), sw_material[4]);
// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateMaterialReflMaterial(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[4])	pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL EMISSIÓ: Activació i desactivació de la reflectivitat pròpia del material.
void CEntornVGIView::OnMaterialEmissio()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[0] = !sw_material[0];

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
// Pas màscara llums
	glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[0]"), sw_material[0]);
// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateMaterialEmissio(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[0])	pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL AMBIENT: Activació i desactivació de la reflectivitat ambient del material.
void CEntornVGIView::OnMaterialAmbient()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[1] = !sw_material[1];

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
// Pas màscara llums
	glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[1]"), sw_material[1]);
// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateMaterialAmbient(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[1])	pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL DIFUSA: Activació i desactivació de la reflectivitat difusa del materials.
void CEntornVGIView::OnMaterialDifusa()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[2] = !sw_material[2];

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
// Pas màscara llums
	glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[2]"), sw_material[2]);
// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateMaterialDifusa(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[2])	pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL ESPECULAR: Activació i desactivació de la reflectivitat especular del material.
void CEntornVGIView::OnMaterialEspecular()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[3] = !sw_material[3];

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
// Pas màscara llums
	glUniform1i(glGetUniformLocation(shader_programID, "sw_intensity[3]"), sw_material[3]);
// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateMaterialEspecular(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[3])	pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ: Textures?: Activació (TRUE) o desactivació (FALSE) de textures.
void CEntornVGIView::OnIluminacioTextures()
{
// TODO: Agregue aquí su código de controlador de comandos
	textura = !textura;

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

//	Pas de textura al shader
	glUniform1i(glGetUniformLocation(shader_programID, "texture"), textura);

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioTextures(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (textura) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// IL.LUMINACIÓ->TEXTURA TEXTURES PREDEFINIDES
// TEXTURA Fusta
// TEXTURA Marbre
// TEXTURA Metall


// ILUMINACIÓ --> TEXTURA: Càrrega fitxer textura per llibreria SOIL
void CEntornVGIView::OnIluminacioTexturaFitxerimatge()
{
	//GLboolean err;

// TODO: Agregue aquí su código de controlador de comandos
	CString nomf;
	t_textura = TEXTURA_FITXERIMA;		tFlag_invert_Y = true;
	textura = true;

// Obrir diàleg de lectura de fitxer
	CFileDialog openTextur(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("JPG Files(*.jpg)|*.jpg|PNG Files(*.png)|*.png|BMP Files (*.bmp)|*bmp|All Files (*.*)|*.*||"));;

	if (openTextur.DoModal() != IDOK)	return;                 // stay with old data file
	else nomf = openTextur.GetPathName();

// Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers textura
	char *nomfitx = CString2Char(nomf);

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Entorn VGI: Eliminar buffers de textures previs del vector texturesID[].
	for (int i = 0; i < NUM_MAX_TEXTURES; i++) {
		if (texturesID[i]) {
			//err = glIsTexture(texturesID[i]);
			glDeleteTextures(1, &texturesID[i]);
			//err = glIsTexture(texturesID[i]);
			texturesID[i] = 0;
		}
	}

// EntornVGI: Carregar fitxer textura i definir buffer de textura.Identificador guardat a texturesID[0].
	texturesID[0] = loadIMA_SOIL(nomfitx);

//	Pas de textura al shader
	glUniform1i(glGetUniformLocation(shader_programID, "texture0"), GLint(0));
	glUniform1i(glGetUniformLocation(shader_programID, "flag_invert_y"), tFlag_invert_Y);

// Desactivació contexte OpenGL: Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioTexturaFitxerimatge(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (t_textura == TEXTURA_FITXERIMA) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ --> TEXTURA: FLAG_INVERT_Y Inversió coordenada t de textura (1-cty) per a textures SOIL (TRUE) 
//			o no (FALSE) per a objectes 3DS i OBJ.
void CEntornVGIView::OnIluminacioTexturaFlagInvertY()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (textura) tFlag_invert_Y = !tFlag_invert_Y;

// Entorn VGI: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

//	Pas de paràmetres textura al shader
	glUniform1i(glGetUniformLocation(shader_programID, "flag_invert_y"), tFlag_invert_Y);

// Desactivació contexte OpenGL: Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateIluminacioTexturaFlagInvertY(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (tFlag_invert_Y) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					11. LLUMS												 */
/* ------------------------------------------------------------------------- */

// LLUMS: Activació / Desactivació llum ambient 
void CEntornVGIView::OnLlumsLlumambient()
{
// TODO: Agregue aquí su código de controlador de comandos
	llum_ambient = !llum_ambient;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateLlumsLlumambient(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llum_ambient) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// LLUMS: Activació /Desactivació llum 0 (GL_LIGHT0)
void CEntornVGIView::OnLlumsLlum0()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[0].encesa = !llumGL[0].encesa;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum0(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[0].encesa) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 1 (GL_LIGHT1)
void CEntornVGIView::OnLlumsLlum1()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[1].encesa = !llumGL[1].encesa;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum1(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[1].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 2 (GL_LIGHT2)
void CEntornVGIView::OnLlumsLlum2()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[2].encesa = !llumGL[2].encesa;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum2(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[2].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 3 (GL_LIGHT3)
void CEntornVGIView::OnLlumsLlum3()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[3].encesa = !llumGL[3].encesa;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum3(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[3].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 4 (GL_LIGHT4)
void CEntornVGIView::OnLlumsLlum4()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[4].encesa = !llumGL[4].encesa;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum4(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[4].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}



// LLUMS-->ON/OFF: Activació /Desactivació llum 5 (GL_LIGHT5)
void CEntornVGIView::OnLlumsLlum5()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[5].encesa = !llumGL[5].encesa;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum5(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[5].encesa) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 6 (GL_LIGHT6)
void CEntornVGIView::OnLlumsLlum6()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[6].encesa = !llumGL[6].encesa;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum6(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[6].encesa) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 7 (GL_LIGHT7)
void CEntornVGIView::OnLlumsLlum7()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[7].encesa = !llumGL[7].encesa;
	sw_il = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateLlumsLlum7(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[7].encesa) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					12. SHADERS												 */
/* ------------------------------------------------------------------------- */

// SHADER FLAT
void CEntornVGIView::OnShadersFlat()
{
// TODO: Agregue aquí su código de controlador de comandos
	shader = FLAT_SHADER;	ilumina = SUAU;
	test_vis = false;		oculta = true;

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	// Elimina shader anterior
	shaderLighting.DeleteProgram();
	// Càrrega Flat shader
	shader_programID = shaderLighting.loadFileShaders(".\\shaders\\flat_shdrML.vert", ".\\shaders\\flat_shdrML.frag");

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateShadersFlat(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader == FLAT_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// SHADER GOURAUD
void CEntornVGIView::OnIluminacioGouraud()
{
// TODO: Agregue aquí su código de controlador de comandos
	shader = GOURAUD_SHADER;	ilumina = SUAU;
	test_vis = false;			oculta = true;

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Elimina shader anterior
	shaderLighting.DeleteProgram();
// Càrrega Gouraud shader 
	shader_programID = shaderLighting.loadFileShaders(".\\shaders\\gouraud_shdrML.vert", ".\\shaders\\gouraud_shdrML.frag");

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioGouraud(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader == GOURAUD_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// SHADER PHONG
void CEntornVGIView::OnIluminacioPhong()
{
// TODO: Agregue aquí su código de controlador de comandos
	shader = PHONG_SHADER;	ilumina = SUAU;
	test_vis = false;		oculta = true;

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Elimina shader anterior
	shaderLighting.DeleteProgram();
// Càrrega Phong Shader
	shader_programID = shaderLighting.loadFileShaders(".\\shaders\\phong_shdrML.vert", ".\\shaders\\phong_shdrML.frag");

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateIluminacioPhong(CCmdUI* pCmdUI)
{
	// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader == PHONG_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// SHADERS: Càrrega Fitxers Shader (.vert, .frag)
void CEntornVGIView::OnShaderLoadFiles()
{
// TODO: Agregue aquí su código de controlador de comandos
	CString nomVert, nomFrag;

	shader = FILE_SHADER;	ilumina = SUAU;
	test_vis = false;		oculta = true;

// Càrrega fitxer VERT
// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.VERT)
	CFileDialog openVert(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("VERT Files(*.vert)|*.vert|All Files (*.*)|*.*||"));;

	if (openVert.DoModal() != IDOK)
		return;                 // stay with old data file
	else nomVert = openVert.GetPathName();
// Entorn VGI: Variable de tipus CString 'nomVert' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
//	char *nomfitxV = CString2Char(nomVert);

// Conversió CString nomFrag a--> std::string nomFragS
	std::string nomVertS = CString2String(nomVert);

// Càrrega fitxer FRAG
// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.VERT)
	CFileDialog openFrag(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("FRAG Files(*.frag)|*.frag|All Files (*.*)|*.*||"));;

	if (openFrag.DoModal() != IDOK)
		return;                 // stay with old data file
	else nomFrag = openFrag.GetPathName();
	// Entorn VGI: Variable de tipus CString 'nomFrag' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char *nomFragS = CString2Char(nomFrag);

	// Conversió CString nomFrag a--> std::string nomFragS
	//std::string nomFragS = CString2String(nomFrag);

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	GLuint newShaderID = 0;
// Elimina shader anterior
	shaderLighting.DeleteProgram();
	newShaderID = shaderLighting.loadFileShaders(nomVertS, nomFragS);
// Càrrega shaders dels fitxers
	if (!newShaderID) AfxMessageBox(_T("GLSL_Error. Fitxers .vert o .frag amb errors"));
	else shader_programID = newShaderID;

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornVGIView::OnUpdateShaderLoadFiles(CCmdUI *pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader == FILE_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// Escriure Binary Program actual en fitxer .bin
void CEntornVGIView::OnShaderPBinaryWrite()
{
// TODO: Agregue aquí su código de controlador de comandos
	CString nomPBinary;

// Càrrega fitxer .BIN
// Entorn VGI: Obrir diàleg d'escriptura de fitxer (fitxers (*.bin)
	CFileDialog openVert(FALSE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("BIN Files(*.bin)|*.bin|All Files (*.*)|*.*||"));;

	if (openVert.DoModal() != IDOK)
		return;                 // stay with old data file
	else nomPBinary = openVert.GetPathName();
// Entorn VGI: Variable de tipus CString 'nomPBinary' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//	compatible amb  les funcions de càrrega de fitxers fractals
	char *nomFitxerPB = CString2Char(nomPBinary);

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Entorn VGI: To retrieve the compiled Binary Program shader code and write it to a file
	GLint formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	GLint *binaryFormats = new GLint[formats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);
	
	GLint length = 0;
	glGetProgramiv(shader_programID, GL_PROGRAM_BINARY_LENGTH, &length);

// Retrieve the binary code
	std::vector<GLubyte> buffer(length);
	GLenum *Formats= 0;
	glGetProgramBinary(shader_programID, length, NULL, (GLenum*)Formats, buffer.data());

// Write the binary to a binary file
	FILE* sb;
	sb = fopen(nomFitxerPB, "wb");
	fwrite(buffer.data(), length, 1, sb);
	fclose(sb);

// MISSATGE DE FITXER BEN GRAVAT o MAL GRAVAT
	//AfxMessageBox(_T("Fitxer ben gravat"));

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
}


// Llegir Binary Program de fitxer .bin i instalar i definir com actual.
void CEntornVGIView::OnShaderPBinaryRead()
{
// TODO: Agregue aquí su código de controlador de comandos
	CString nomPBinary;
	FILE* fd;

	shader = PROG_BINARY_SHADER;		ilumina = SUAU;	
	test_vis = false;					oculta = true;

// Càrrega fitxer .BIN
// Entorn VGI: Obrir diàleg de lectura de fitxer (fitxers (*.bin)
	CFileDialog openVert(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("BIN Files(*.bin)|*.bin|All Files (*.*)|*.*||"));;

	if (openVert.DoModal() != IDOK)
		return;                 // stay with old data file
	else nomPBinary = openVert.GetPathName();
// Entorn VGI: Variable de tipus CString 'nomPBinary' conté el nom del fitxer seleccionat

// Entorn VGI: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char* nomFitxerPB = CString2Char(nomPBinary);

// Entorn VGI: To read de Shader Program from a file and install it
	GLint filelength = 0;
	GLenum format = 0;

/* Retrieve the binary code per a obtenir valor variable format
	std::vector<GLubyte> buff(filelength);
	GLint longitut = 0;
	glGetProgramBinary(shader_programID, longitut, NULL, &format, buff.data());
*/

// Entorn VGI: Read from a binary file
	FILE* sb;
	sb = fopen(nomFitxerPB, "rb");
	if (!sb) {	AfxMessageBox(_T("GLSL_Error. Unable to open file"));
				return;
			}

	// Get file length
	fseek(sb, 0, SEEK_END);
	filelength = ftell(sb);
	fseek(sb, 0, SEEK_SET);

	std::vector<GLubyte> buffer(filelength+1); // Allocatem buffer amb mida de Binary Program
	fclose(sb);

	sb = fopen(nomFitxerPB, "rb");
	fread(buffer.data(), filelength, 1, sb);
	fclose(sb);

// Entorn VGI: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	
	// Install shader binary
	GLint formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);

	GLuint shader_BinProgramID = glCreateProgram();
	glProgramBinary(shader_BinProgramID, formats, buffer.data(), filelength);
	
	//glLinkProgram(shader_BinProgramID); // Linkedició del program.
// Check for success/failure
	GLint status;
	glGetProgramiv(shader_BinProgramID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		// Llista error de linkedició del Shader Program
		GLint maxLength = 0;

		glGetProgramiv(shader_BinProgramID, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(shader_BinProgramID, maxLength, &maxLength, &errorLog[0]);

		AfxMessageBox(_T("Error Linkedicio Shader Binary Program"));
		//fprintf(stderr, "%s \n", "Error Linkedicio Shader Program");

		// Volcar missatges error a fitxer GLSL_Error.LINK
		if ((fd = fopen("GLSL_Error.LINK", "w")) == NULL)
		{
			AfxMessageBox(_T("GLSL_Error.LINK was not opened"));
			//fprintf(stderr, "%s \n", "GLSL_Error.LINK was not opened");
		}
		for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
		fclose(fd);
		glDeleteProgram(shader_BinProgramID);		// Don't leak the program.
		}
	else {	
			//shaderLighting.DeleteProgram();	// Eliminar shader anterior
			shader_programID = shader_BinProgramID; // Assignar nou Binary Program com l'actual.
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glUseProgram(shader_programID);			// Activa shader llegit.
		}

// Entorn VGI: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

}

void CEntornVGIView::OnUpdateShaderPBinaryRead(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader == PROG_BINARY_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}



/* ------------------------------------------------------------------------- */
/*   RECURSOS DE BOTONS (ToolBar) DE L'APLICACIO                             */
/* ------------------------------------------------------------------------- */
// Incloure funcions de tractament de botons


// ---------------- Entorn VGI: Funcions locals a EntornVGIView.cpp

// Log2: Càlcul del log base 2 de num
int CEntornVGIView::Log2(int num)
{
	int tlog;

	if (num >= 8192) tlog = 13;
	else if (num >= 4096) tlog = 12;
	else if (num >= 2048) tlog = 11;
	else if (num >= 1024) tlog = 10;
	else if (num >= 512) tlog = 9;
	else if (num >= 256) tlog = 8;
	else if (num >= 128) tlog = 7;
	else if (num >= 64) tlog = 6;
	else if (num >= 32) tlog = 5;
	else if (num >= 16) tlog = 4;
	else if (num >= 8) tlog = 3;
	else if (num >= 4) tlog = 2;
	else if (num >= 2) tlog = 1;
	else tlog = 0;

	return tlog;
}


// CString2char: Funció de conversió de variable CString a char * per a noms de fitxers 
char * CEntornVGIView::CString2Char(CString entrada)
{
//char * par_sortida=" ";
// Variable de tipus CString 'entrada' conté un string tipus CString
//-------------------------------------------------------------------------------------
// Conversió de la variable CString entrada a la variable char *sortida, 
//	compatible amb la funció Carregar3DS, de càrrega de fitxers 3DS
//	char * nomfitx = (char *)(LPCTSTR)nom;

// Conversió variable w_char --> char *
//	char *nomf="  ";
//	wcstombs(nomf,strFileName.GetBuffer(3),90);
//	char *nomf = reinterpret_cast<char *> (nom.GetBuffer(3));

	size_t origsize = wcslen(entrada.GetBuffer(3)) + 1;
	size_t convertedChars = 0;

// Use a multibyte string to append the type of string
// to the new string before displaying the result.
	char strConcat[] = " (char *)";
	size_t strConcatsize = (strlen(strConcat) + 1) * 2;

// Allocate two bytes in the multibyte output string for every wide
// character in the input string (including a wide character
// null). Because a multibyte character can be one or two bytes,
// you should allot two bytes for each character. Having extra
// space for the new string is not an error, but having
// insufficient space is a potential security problem.
	const size_t newsize = origsize * 2;
// The new string will contain a converted copy of the original
// string plus the type of string appended to it.
//	char *nomfitx = new char[newsize + strConcatsize];
	char *par_sortida = new char[newsize + strConcatsize];

// Put a copy of the converted string into nstring
	wcstombs_s(&convertedChars, par_sortida, newsize, entrada.GetBuffer(3), _TRUNCATE);
// append the type of string to the new string.
//----------------------------------------------------------------------------------

// Variable de tipus char *nomfitx conté el nom del fitxer seleccionat
	return par_sortida;
}


// Refl_MaterialOff: Desactivar Reflexió de Material
void CEntornVGIView::Refl_MaterialOff()
{
	sw_material_old[0] = sw_material[0];	sw_material[0] = false;
	sw_material_old[1] = sw_material[1];	sw_material[1] = false;
	sw_material_old[2] = sw_material[2];	sw_material[2] = false;
	sw_material_old[3] = sw_material[3];	sw_material[3] = false;
}

// Refl_MaterialOn: Activar Reflexió de Material
void CEntornVGIView::Refl_MaterialOn()
{
	sw_material[0] = sw_material_old[0];
	sw_material[1] = sw_material_old[1];
	sw_material[2] = sw_material_old[2];
	sw_material[3] = sw_material_old[3];
}


// -------------------- FUNCIONS CORBES B-SPLINE, BEZIER i HERMITTE

// llegir_ptsC: Llegir punts de control de corba (spline o Bezier) d'un fitxer .crv. 
//				Retorna el nombre de punts llegits en el fitxer.
//int llegir_pts(CString nomf)
int CEntornVGIView::llegir_ptsC(char *nomf)
{
	int i, j;
	FILE *fd;

// Inicialitzar vector punts de control de la corba spline
	for (i = 0; i < MAX_PATCH_CORBA; i = i++)
	{
		PC_t[i].x = 0.0;
		PC_t[i].y = 0.0;
		PC_t[i].z = 0.0;
	}

//	ifstream f("altinicials.dat",ios::in);
//    f>>i; f>>j;
	if ((fd = fopen(nomf, "rt")) == NULL)
	{	LPCWSTR texte1 = reinterpret_cast<LPCWSTR> ("ERROR:");
		LPCWSTR texte2 = reinterpret_cast<LPCWSTR> ("File .crv was not opened");
		//MessageBox(NULL, texte1, texte2, MB_OK);
		MessageBox(texte1, texte2, MB_OK);
		//AfxMessageBox("file was not opened");
		return false;
	}
	fscanf(fd, "%d \n", &i);
	if (i == 0) return false;
	else {
		for (j = 0; j < i; j = j++)
		{	//fscanf(fd, "%f", &corbaSpline[j].x);
			//fscanf(fd, "%f", &corbaSpline[j].y);
			//fscanf(fd, "%f \n", &corbaSpline[j].z);
			fscanf(fd, "%lf %lf %lf \n", &PC_t[j].x, &PC_t[j].y, &PC_t[j].z);

		}
	}
	fclose(fd);

	return i;
}


// -------------------- FUNCIONS FITXER FONTS DE LLUM (*.lght)

// llegir_FontLlum: Llegir fitxer .lght que conté paràmetres de la font de lluym i-èssima. 
//				Retorna booleà a TRUE si s'ha fet la lectura correcte, FALSE en cas contrari.
//bool llegir_FontLlum(CString nomf)
bool CEntornVGIView::llegir_FontLlum(char* nomf)
{
	int i, j;
	FILE* fd;

	//	ifstream f("altinicials.dat",ios::in);
	//    f>>i; f>>j;
	if ((fd = fopen(nomf, "rt")) == NULL)
	{
		LPCWSTR texte1 = reinterpret_cast<LPCWSTR> ("ERROR:");
		LPCWSTR texte2 = reinterpret_cast<LPCWSTR> ("File .lght was not opened");
		//MessageBox(NULL, texte1, texte2, MB_OK);
		MessageBox(texte1, texte2, MB_OK);
		//AfxMessageBox("file was not opened");
		return false;
	}
	fscanf(fd, "%i \n", &i);
	if (i < 0 || i >= NUM_MAX_LLUMS) return false;
	else {
		fscanf(fd, "%i \n", &j); // Lectura llum encesa
		if (j == 0) llumGL[i].encesa = false; else llumGL[i].encesa = true;
		fscanf(fd, "%lf %lf %lf %lf\n", &llumGL[i].posicio.x, &llumGL[i].posicio.y, &llumGL[i].posicio.z, &llumGL[i].posicio.w);
		fscanf(fd, "%lf %lf %lf \n", &llumGL[i].difusa.r, &llumGL[i].difusa.g, &llumGL[i].difusa.b);
		fscanf(fd, "%lf %lf %lf \n", &llumGL[i].especular.r, &llumGL[i].especular.g, &llumGL[i].especular.b);
		fscanf(fd, "%lf %lf %lf \n", &llumGL[i].atenuacio.a, &llumGL[i].atenuacio.b, &llumGL[i].atenuacio.c);
		fscanf(fd, "%i \n", &j); // Lectura booleana font de llum restringida.
		if (j == 0) llumGL[i].restringida = false; else llumGL[i].restringida = true;
		fscanf(fd, "%lf %lf %lf \n", &llumGL[i].spotdirection.x, &llumGL[i].spotdirection.y, &llumGL[i].spotdirection.z);
		fscanf(fd, "%f \n", &llumGL[i].spotcoscutoff);
		fscanf(fd, "%f \n", &llumGL[i].spotexponent);
		}
	fclose(fd);

	return true;
}

// Conversió CString --> std::string
std::string CEntornVGIView::CString2String(const CString& cString)
{
	std::string strStd;

	for (int i = 0; i < cString.GetLength(); ++i)
	{
		if (cString[i] <= 0x7f)
			strStd.append(1, static_cast<char>(cString[i]));
		else
			strStd.append(1, '?');
	}
	return strStd;
}

void CEntornVGIView::OnProjeccioOrtografica()
{
	// TODO: Agregue aquí su código de controlador de comandos
	projeccio = ORTO;
	mobil = false;
	zzoom = false;
	InvalidateRect(NULL, false);
}

void CEntornVGIView::OnUpdateProjeccioOrtografica(CCmdUI* pCmdUI)
{
	// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (projeccio == ORTO)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CEntornVGIView::OnProjeccioAxonometrica()
{
	// TODO: Agregue aquí su código de controlador de comandos
	projeccio = AXONOM;
	mobil = true;
	zzoom = true;
}

void CEntornVGIView::OnUpdateProjeccioAxonometrica(CCmdUI* pCmdUI)
{
	// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (projeccio == AXONOM)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}
