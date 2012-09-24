#ifndef DLLEXPORTS_H
#define DLLEXPORTS_H

#ifdef __dll__
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif 	// __dll__

#include "AINavigation.h"
#include "NavDebugRender.h"

extern "C"
IMPEXP void* CreateNavClassInstance();
extern "C"
IMPEXP void* CreateDebugRenderClassInstance(AINavigation* navsystem, LPDIRECT3DDEVICE9 lpDevice);
extern "C"
IMPEXP void DeleteNavClassInstance(void* toDel);
extern "C"
IMPEXP void DeleteDebugRenderClassInstance(void* toDel);

typedef void* (*CreateNavigationSystem) ();
typedef void* (*CreateDebugRender)(AINavigation*, LPDIRECT3DDEVICE9);
typedef void (*Release) (void*);

//extern IMPEXP CreateNavigationSystem createNavSystem;
//extern IMPEXP CreateDebugRender createDebugRender;
//extern IMPEXP Release releaseNavSystem;
//extern IMPEXP Release releaseDebugRender;
//extern IMPEXP HINSTANCE hdll;

static CreateNavigationSystem createNavSystem = NULL;
static CreateDebugRender createDebugRender = NULL;
static Release releaseNavSystem = NULL;
static Release releaseDebugRender = NULL;
static HINSTANCE hdll = NULL;

static void InitNavigation()
{
	if(hdll == NULL)
	{
		hdll = LoadLibrary(TEXT("AINavigationManager.dll"));
		createNavSystem = (CreateNavigationSystem)GetProcAddress(hdll, "CreateNavClassInstance");
		createDebugRender = (CreateDebugRender)GetProcAddress(hdll, "CreateDebugRenderClassInstance");
		releaseNavSystem = (Release)GetProcAddress(hdll, "DeleteNavClassInstance");
		releaseDebugRender = (Release)GetProcAddress(hdll, "DeleteDebugRenderClassInstance");
	}
}

static void ShutdownNavigation()
{
	if(hdll != NULL)
		FreeLibrary(hdll);
}

#endif	// DLLEXPORTS_H