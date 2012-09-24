#pragma once
#include "../EDMemoryManager/MMAllocator.h"
#include <string.h>
#include <list>
#include <vector>
#include "float3.h"

#define NUM_CLASSES 3

namespace EDUtilities
{
    /// A single page of text to be printed by the PrintConsole.
    /// \sa PrintConsole
	class PrintPage
	{
	private:
        /// The data defining a single string of text on a PrintPage
        /// \sa PrintPage
		class PrintString
		{
			friend PrintPage;
		private:
            /// The string to print.
			char *string;
            /// The color to print in.
			float3 color;
		public:
            /// Constructor sets variables to default state.
			PrintString();
            /// Copy constructor creates a new PrintString based on an existing one.
            /// \param ps - The PrintString to copy from.
			PrintString(const PrintString &ps);
            /// Move constructor creates a new PrintString based on an existing one.
            /// \param ps - The PrintString to copy from.
			PrintString(PrintString &&ps);

			template< typename T>
			PrintString(const char *_string, T&& _color) : string(0)
			{
				SetString(_string);
				color = std::forward<T>(_color);
			}

			//PrintString(const char *sz, float3&& color)
			//PrintString(const char *sz, float3& color)
			//PrintString(const char *sz, const float3& color)

			/// Copies one PrintString to another one.
            /// \param rps - The PrintString to copy from.
            /// \return Returns the result of the assignment.
            PrintString &operator=(const PrintString &rps);
			/// Moves one PrintString to another one.
            /// \param rps - The PrintString to copy from.
            /// \return Returns the result of the assignment.
            PrintString &operator=( PrintString&&rps);
            /// Cleans up the dynamic string.
			~PrintString() { delete string; string = 0; }
            /// Sets the string to be printed.
            /// \param sz - A null terminated C style string we want to copy from.
			void SetString(const char *_string);
		};

		class PrintSolution
		{
			friend PrintPage;
		private:
			/// Class
			PrintString classTitle;
			/// Number of labs for this class.
			unsigned int numLabs;
			/// Vector of strings to be rendered for each lab.
			std::list< std::list<PrintString> > solutions;
		public:
			/// Constructor sets variables to default state.
			PrintSolution(void);
			PrintSolution(const char *sz, const int _numLabs);
			/// Copy constructor creates a new PrintSolution based on an existing one.
			/// \param ps - The PrintSolution to copy from.
			PrintSolution(const PrintSolution &ps);
			PrintSolution( PrintSolution &&ps);
			/// Copies one PrintSolution to another one.
            /// \param rps - The PrintString to copy from.
            /// \return Returns the result of the assignment.
            PrintSolution &operator=(const PrintSolution &rps);
			/// Moves one PrintSolution to another one.
            /// \param rps - The PrintString to copy from.
            /// \return Returns the result of the assignment.
            PrintSolution &operator=( PrintSolution &&rps);
            /// Cleans up the dynamic string.
			~PrintSolution(void);
            /// Sets the string to be printed.
            /// \param sz - A null terminated C style string we want to copy from.
			void SetClassTitle(const char *_classTitle);
		};

        /// The amount of space desired between new strings printed.
		int yRasterUpdate;
        /// The title of this page.  This will be printed first.
		PrintString pageTitle;
        /// The strings to be printed on this page.
		std::vector<PrintString> strings;
		/// The lab solutions for each class
		std::vector<PrintSolution> classSolutions;
	public:
		PrintPage(void);
		~PrintPage(void);

        /// The maximum number of characters to be printed for a single string.
		static const int MaxStringLength = 128;
        /// Sets the title for this page
        /// \param sz - The string to set the title to
		void SetPageTitle(const char *_pageTitle);
        /// Adds a new string to be printed on this page this frame.
        /// \param color - The color the string should be printed in
        /// \param sz - The string to print
		template <typename T>
		void AddString(T &&_color, const char *_string)
		{
			strings.push_back(PrintString(_string, _color));
			size_t current = strings.size() -1;
		}
        /// Renders this page
        /// \param fontPtr - A pointer to the D3D font object to be used to print this page.
		void Render(ID3DXFont *fontPtr);
        /// Deletes all the strings added so far.  This method is called each frame.
		void ClearStrings() { strings.clear(); }


		size_t AddClass(const char *_string, int const numLabs);

		void AddSolution(const char *szClass, int const labIndex, float3 _color, const char *_string);
		void RenderSolutions(ID3DXFont *fontPtr, const char *_string, int const labIndex);
		size_t GetNumSolutions(const char *_string) const;
	};
}
