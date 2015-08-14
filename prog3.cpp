/*************************************************************************//**
 * @file 
 *
 * @mainpage Prog3 - WVX Files
 * 
 * @section course_section Course Information 
 *
 * @author Johnathan Ackerman
 * 
 * @date November 18, 2013
 * 
 * @par Professor: 
 *         Roger Schrader
 * 
 * @par Course: 
 *         CSC-250 - M001-2013FA -  2:00-3:00 PM MWTHF
 * 
 * @par Location: 
 *         Classroom Building - 166
 *
 * @section program_section Program Information 
 * 
 * @details The program loops though a given directory from the commandline
 *		looking for a pattern, also from the commandline. If anyfiles match
 *		the pattern, they are placed in a .WVX playlist
 *
 * If there are any subdirectories in the given directory, it will recurrsively
 *		open the subdirectory and any subdirectory in that and check for files
 *		with the pattern, and making more playlists.
 *
 * @section compile_section Compiling and Usage 
 *
 * @par Compiling Instructions: 
 *      The program expects a starting directory and a pattern to look for
 * 
 * @par Usage: 
   @verbatim  
   c:\> Prog3.exe <Directory> <Pattern>
   d:\> c:\bin\Prog3.exe 
   @endverbatim 
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 * 
 * @bug if the pattern is .mp3 it will add .mp33 to the playlist as well
 * @bug Outputs the usage statment if the directory ends with '\'
 * 
 * @par Modifications and Development Timeline: 
   @verbatim 
   Date          Modification 
   ------------  -------------------------------------------------------------- 
   Nov 13, 2013  Started Program
   Nov 14, 2013  Attempted to write a Playlist loop
   Nov 15, 2013  removed playlist loop switched to C++ Strings
   Nov 16, 2013  Added Doxygen
   Nov 17, 2013  significant prograss made on the write playlist function
   Nov 18, 2013	 finished write playlist function, wrote recursive loop 
				 Program turned in
   @endverbatim
 *
 *****************************************************************************/
#include <iostream>
#include <direct.h>
#include <io.h>
#include <cstring>
#include <string>
#include <fstream>
using namespace std;

 /*****************************************************************************
 *                         Function Prototypes
 *****************************************************************************/
void writePlaylist(string pattern);
void openDirectory(string pattern);

/**************************************************************************//**
 * @author  Johnny Ackerman
 * 
 * @par Description: checks commandline aurgments and calls the openDirectory
 * function, Also initiats the dir variable for the directory name, and the
 * pattern variable for the pattern check
 * 
 * 
 * @param[in]      argc - Number of aurments from the commandline
 * @param[out]     argv - an array of commandline aurgments
 * 
 * @returns 0 program ran successful.
 * @returns -2 Used incorrectly
 * 
 *****************************************************************************/
int main(int argc, char *argv[])
{
	//initialization
	char dir[_MAX_PATH];//holds directory starting name
	string pattern;//holds the pattern for playlist check
	int i = 0; //used for lower for loop

	//checks commandline aurgments
	if ( argc != 3 )
	{
		cout << "usage: prog3.exe <directory> <pattern>" << endl;
		cout << "<directory> = the starting directory to be searched" << endl;
		cout << "<pattern> = the pattern of the files being looked for to be"
			<< " put into the playlist" << endl;
		return -2;
	}

	//set pattern to command aurgement
	pattern = string(argv[2]);//No error checking
	
	//changes to first directory
	strcpy_s( dir, argv[1] );
	if(_chdir(dir) == 0 )
		cout << "Program changed directories sucessfully" << endl;
	else
		cout << "unable to change to the directory: " << dir << endl;

	//calls recursive function to write playlists and look though sub
	//directories
	openDirectory(pattern);

	return 0;
}

/**************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: 
 * tests all files in the directory against a pattern. If any files match a
 * playlist is created using those files.
 * 
 * @param[in]      pattern - The ending pattern used to check for files
 * 
 * 
 *****************************************************************************/
void writePlaylist(string pattern)
{
	//initiallization
	int pos = 0;//temporary postition variable for partial string deletion
	string temp;//temporary string to hold a modified media title
	string title;//temporary string to hold a modified directory title

	ofstream fout;//opens output file
	char *buffer;//holds non-modified directory title

	//sets to directory title
	buffer = _getcwd( NULL, 0 );
	
	

	//structure for a file/folder (fields: name, attrib, size, time_create, 
	//time_access, time_write)
	_finddata_t a_file;
	intptr_t dir_handle;

	
	//checks for the pattern
	dir_handle = _findfirst( pattern.c_str(), &a_file);
	if(dir_handle == -1 )
		return;

	//hold a modifiable directory title
	title = string(buffer);
	
	//cuts of the end of the title
	pos = title.find_last_not_of('\\');
	if( pos+1 != title.length())
	{
		temp.erase(pos+1, title.length()-1);
	}

	//cuts of the front of the title
	pos = title.find_last_of('\\');
	if( pos != string::npos)
	{
		title.erase(0, pos+1);
	}

	//opens playlist file
	fout.open( string("playlist.wvx") );

	//file header
	fout << "<ASX version = \"3.0\">" << endl;
	fout << "	<Title>";
	fout << title;//modified shortened title
	fout << "</Title>" << endl;
	fout << "	<Base href=\"";
	fout << string(buffer)/*unmodified directory title*/ << "\\\" />" << endl;

	//walks though each part of the directory
	do
	{
		if( (a_file.attrib & _A_SUBDIR) == 0 )
		{
			//cout << a_file.name << endl;

			//formating
			fout << "	<Entry>" << endl << "		<Title>";
			
			//sets modifiable file title
			temp = string(a_file.name);

			//deletes everything pass the first '.'
			pos = temp.find_first_of('.');
			if( pos != string::npos)
			{
				temp.erase(pos, temp.length());
			}

			//deletes everything before the first '-'
			pos = temp.find_first_of('-');
			if( pos != string::npos)
			{
				temp.erase(0, pos);
			}
			
			fout << temp/*outputs modified file title*/ << "</Title>" << endl;

			//checks if it is a picture file and adds a duration
			if( pattern == string(".jpg") || pattern == string(".png")
				|| pattern == string(".bmp") || pattern == string(".gif") 
				|| pattern == string(".pbm") || pattern == string(".pgm") 
				|| pattern == string(".ppm") || pattern == string(".jpeg")
				|| pattern == string(".tif") || pattern == string(".tiff") )
			{
				fout << "<Duration value=\"00:00:05\" />";
			}

			//formating
			fout << "		<Ref href=\"" << a_file.name<< "\" />" << endl 
				<< "	</Entry>" << endl;
		}
		else
		{
			//cout<< a_file.name << endl;//shows each file
			//outputs the names of the directories in a file
		}
	}while( _findnext( dir_handle, &a_file ) == 0 );

	//formating
	fout << "</ASX>" << endl;

	//closes the directory
	_findclose( dir_handle );
}

 /**************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: 
 * looks though a directory distiquishing files from subdirectories
 * 
 * @param[in]      pattern - The ending pattern used to check for files
 * 
 * 
 *****************************************************************************/
void openDirectory(string pattern)
{
	char *buffer;//holds the directory name
	buffer = _getcwd( NULL, 0 );
	

	//structure for a file/folder (fields: name, attrib, size, time_create, 
	//time_access, time_write)
	_finddata_t a_file;
	intptr_t dir_handle;

	//checks if the file is empty
	dir_handle = _findfirst( "*.*", &a_file);
	if(dir_handle == -1 )
		return;

	//walks though each part of the directory
	do
	{
		//checks if a file or if a directory
		if( (a_file.attrib & _A_SUBDIR) == 0 )
		{
			//writes playlist for files matching pattern
			writePlaylist(pattern);

			//cout << a_file.name << endl;//looks at actual files in directory
		}
		else
		{
			//cout<< a_file.name << endl;//shows each subdirectory

			//avoids infinite loop and backtracking
			if( string(a_file.name) != string(".") 
				&& string(a_file.name) != string("..") )
			{
				//opens next subdirectory
				if( _chdir( a_file.name ) == 0 )
				{
					//RECURSION!!!!
					openDirectory( pattern);
				}
			}
		}
	}while( _findnext( dir_handle, &a_file ) == 0 );

	//closes the directory
	_findclose( dir_handle );

}