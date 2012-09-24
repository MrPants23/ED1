#include "PrintPage.h"


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
//#include "../EDUtilities/GLee.h"
//#include <gl/gl.h>
#include <stdio.h>

namespace EDUtilities
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	// PrintPage::PrintString
	PrintPage::PrintString::PrintString() 
	{ 
		color = float3(1.0f, 1.0f, 1.0f); 
		string = 0; 
	}

	PrintPage::PrintString::PrintString(const PrintString &ps)
	{
		*this = ps;
	}

	PrintPage::PrintString::PrintString(PrintString &&ps)
	{
		*this = std::move(ps);
	}

	PrintPage::PrintString &PrintPage::PrintString::operator=(const PrintString &rps)
	{
		if(this != &rps)
		{
			string = 0;
			color = rps.color;
			if(rps.string)
				SetString(rps.string);
		}
		return *this;
	}

	PrintPage::PrintString &PrintPage::PrintString::operator=( PrintString&& rps)
	{
		if(this != &rps)
		{
			string = std::move(rps.string);
			color = std::move(rps.color);
			rps.string = 0;
		}
		return *this;
	}

	void PrintPage::PrintString::SetString(const char *_string)
	{
		delete [] string;
		const size_t length = strlen(_string)+1;

		string = new char[length];
		strcpy_s(string, length, _string);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// PrintPage::PrintSolution
	PrintPage::PrintSolution::PrintSolution(void)
	{
		numLabs = 0;
	}

	PrintPage::PrintSolution::PrintSolution(const char *_string, const int _numLabs)
	{
		numLabs = _numLabs;
		classTitle.SetString(_string);
	}

	PrintPage::PrintSolution::~PrintSolution(void)
	{
	}

	PrintPage::PrintSolution::PrintSolution(const PrintSolution &ps)
	{
		*this = ps;
	}

	PrintPage::PrintSolution::PrintSolution(PrintSolution &&ps)
	{
		*this = std::move(ps);
	}



	PrintPage::PrintSolution &PrintPage::PrintSolution::operator=(const PrintSolution &rps)
	{
		if(this != &rps)
		{
			classTitle = rps.classTitle;
			numLabs = rps.numLabs;
			solutions = rps.solutions;
		}
		return *this;
	}

	PrintPage::PrintSolution &PrintPage::PrintSolution::operator=(PrintSolution &&rps)
	{
		if(this != &rps)
		{
			classTitle = std::move(rps.classTitle);
			numLabs = std::move(rps.numLabs);
			solutions = std::move(rps.solutions);
		}
		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// PrintPage
	PrintPage::PrintPage(void)
	{
		yRasterUpdate = 21;
	}

	PrintPage::~PrintPage(void)
	{
	}

	void PrintPage::SetPageTitle(const char *_pageTitle)
	{
		pageTitle.SetString(_pageTitle);
	}

	void PrintPage::Render(ID3DXFont *fontPtr)
	{
		if(!fontPtr)
			return;

		int fYRasPos = 1;
        RECT rct;
        rct.left=2;
        rct.right=780;
        rct.top=fYRasPos;
        rct.bottom=rct.top+20;

        D3DCOLOR color = D3DCOLOR_COLORVALUE(1, 1, 1, 1);
        fontPtr->DrawTextA(0, pageTitle.string, -1, &rct, 0, color);

		fYRasPos += yRasterUpdate;
		size_t numStrings = strings.size();
		for(size_t i = 0; i < numStrings; ++i)
		{
            RECT rct;
            rct.left=2;
            rct.right=780;
            rct.top=fYRasPos;
            rct.bottom=rct.top+20;

			fYRasPos += yRasterUpdate;
            color = D3DCOLOR_COLORVALUE(strings[i].color.r, strings[i].color.g, strings[i].color.b, 1);
            fontPtr->DrawTextA(0, strings[i].string, -1, &rct, 0, color);
		}
	}

	size_t PrintPage::AddClass(const char *_string, int const numLabs)
	{
		// Loop through and find the current index it resides.
		size_t const uNumClasses = classSolutions.size();
		for(unsigned int uClass = 0; uClass < uNumClasses; ++uClass)
			if(0 == strcmp(_string, classSolutions[uClass].classTitle.string))
				return uClass;

		classSolutions.push_back(PrintSolution(_string, numLabs));

		size_t const uCurrent = classSolutions.size()-1;
		classSolutions[uCurrent].solutions.resize(numLabs);

		char buffer[12] = {0};
		std::list< std::list<PrintString> >::iterator solutionsIter = 
			classSolutions[uCurrent].solutions.begin();
		for(unsigned int uSolution = 0; uSolution < (unsigned int)numLabs; ++uSolution)
		{
			sprintf_s(buffer, "Lab: %i", uSolution+1);
			(*solutionsIter).push_back(PrintString(buffer, float3(1, 1, 1)));
			++solutionsIter;
			memset(buffer, NULL, sizeof(char) * 12);
		}

		return uCurrent;
	}

	void PrintPage::AddSolution(const char *szClass, int const labIndex, float3 color, const char *_string)
	{
		if(NULL == _string)
			return;

		for(unsigned int uClass = 0; uClass < NUM_CLASSES; ++uClass)
		{
			if(0 == strcmp(szClass, classSolutions[uClass].classTitle.string))
			{
				if(labIndex < 0 || (unsigned int)labIndex >= classSolutions[uClass].solutions.size())
					return;
				
				std::list< std::list<PrintString> >::iterator solutionsIter = 
					classSolutions[uClass].solutions.begin();
				for(int i = 0; i < labIndex; ++i)
					++solutionsIter;

				(*solutionsIter).push_back(PrintString(_string, color));
			}
		}
	}

	void PrintPage::RenderSolutions(ID3DXFont *fontPtr, const char *_string, int const labIndex)
	{
		int fYRasPos = 1;
        RECT rct;
        rct.left=2;
        rct.right=780;
        rct.top=fYRasPos;
        rct.bottom=rct.top+20;

        D3DCOLOR color = D3DCOLOR_COLORVALUE(1, 1, 1, 1);
        fontPtr->DrawTextA(0, pageTitle.string, -1, &rct, 0, color);

		fYRasPos += yRasterUpdate;

		if(NULL == _string)
			return;

		for(unsigned int uClass = 0; uClass < NUM_CLASSES; ++uClass)
		{
			if(0 == strcmp(_string, classSolutions[uClass].classTitle.string))
			{
				if(labIndex < 0 || (unsigned int)labIndex >= classSolutions[uClass].solutions.size())
					return;

				rct.top=fYRasPos;
				rct.bottom=rct.top+20;
				fYRasPos += yRasterUpdate;
				color = D3DCOLOR_COLORVALUE(1, 1, 1, 1);
				fontPtr->DrawTextA(0, classSolutions[uClass].classTitle.string, -1, &rct, 0, color);

				std::list< std::list<PrintString> >::iterator solutionsIter = classSolutions[uClass].solutions.begin();
				for(int i = 0; i < labIndex; ++i)
					++solutionsIter;

				std::list<PrintString>::iterator stringIter = (*solutionsIter).begin();
				std::list<PrintString>::iterator stringIterEnd = (*solutionsIter).end();
				//for(unsigned int uSolution = 0; uSolution < size; ++uSolution)
				for(; stringIter != stringIterEnd; ++stringIter)
				{
					RECT rct;
					rct.left=2;
					rct.right=780;
					rct.top=fYRasPos;
					rct.bottom=rct.top+20;

					fYRasPos += yRasterUpdate;
					color = D3DCOLOR_COLORVALUE((*stringIter).color.r,
												(*stringIter).color.g,
												(*stringIter).color.b, 1);
					fontPtr->DrawTextA(0, (*stringIter).string, -1, &rct, 0, color);
				}
			}
		}
	}

	size_t PrintPage::GetNumSolutions(const char *_string) const
	{
		for(unsigned int uClass = 0; uClass < NUM_CLASSES; ++uClass)
			if(0 == strcmp(_string, classSolutions[uClass].classTitle.string))
				return classSolutions[uClass].solutions.size();

		return -1;
	}
}