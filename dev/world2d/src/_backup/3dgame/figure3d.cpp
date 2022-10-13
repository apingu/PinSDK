//
//
//
//
//
//
//
//
//
//
//
//
//
//                                               Copyright    (C)    2000    Pin
//
#include "Figure3d.h"


//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
CP3DFigure::CP3DFigure()
{
	m_pModelNow = NULL;
	//m_curModel = 0;
}


//=============================================================================
//  Name : Distructure
//  Desc :
//=============================================================================
CP3DFigure::~CP3DFigure()
{
	for (Puint i = 0; i < m_pModelVec.size(); i++)
		delete m_pModelVec[i];
	m_pModelVec.clear();
	m_pModelNow = NULL;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
CPModel* CP3DFigure::Model(Pvoid)
{
	return m_pModelNow;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void CP3DFigure::AddModel(CPModel* model)
{
	m_pModelVec.push_back(model);
	m_pModelNow = model;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pint CP3DFigure::LoadModel(Pcstr szFilename)
{
	CPModel* model = new CPModel;

	if (!model->Load(szFilename))
		return FALSE;
	AddModel(model);
	return TRUE;
}










///////////////////////////////////////////////////////////////////////////////
//


//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
CP3DAction::CP3DAction()
{
	//m_nHAnimCurSeqs =  0;
	//m_nHAnimBlend   = -1;
	m_pHAnimNow = NULL;
}


//=============================================================================
//  Name : Distructure
//  Desc :
//=============================================================================
CP3DAction::~CP3DAction()
{
	for (Puint i = 0; i < m_pHAnimSeqs.size(); i++)
		delete m_pHAnimSeqs[i];
	m_pHAnimSeqs.clear();
	m_pHAnimNow = NULL;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
CP3DAction::operator CPAnim*()
{
	//if( m_nHAnimCurSeqs < m_pHAnimSeqs.size() )
	//    return m_pHAnimSeqs[m_nHAnimCurSeqs];
	return m_pHAnimNow;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
Pint CP3DAction::LoadAnim(Pcstr szFilename)
{
	CPAnim* anim = new CPAnim;

	if (!anim->Load(szFilename))
		return FALSE;
	anim->Transform(Model());
	m_pHAnimSeqs.push_back(anim);
	m_pHAnimNow = anim;
	return TRUE;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
Pint CP3DAction::Do(Puint curAnim)
{
	if (curAnim >= m_pHAnimSeqs.size())
		return FALSE;

	//m_nHAnimCurSeqs = curAnim;
	//m_nHAnimBlend = curAnim;
	//m_pHAnimSeqs[m_nHAnimCurSeqs]->Transform( Model() );
	//m_pHAnimNow = new CPAnim;
	//m_pHAnimNow->BeginBlend( Model() );
	//m_pHAnimNow->BeginBlend( Model(), m_pHAnimSeqs[m_nHAnimCurSeqs], m_pHAnimSeqs[m_nHAnimBlend] );
	m_pHAnimNow = m_pHAnimSeqs[curAnim];
	m_pHAnimNow->Transform(Model());

	return TRUE;
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
Pvoid CP3DAction::UpdateAnim(Pfloat delta)
{
	if (m_pHAnimNow != NULL)
		m_pHAnimNow->Update(Model(), delta);
	return;
}


