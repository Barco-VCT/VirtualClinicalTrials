#include "misc.h"

#include <stdio.h>
#include <boost/filesystem.hpp>

#include <FileFinder/FileFinder.h>
#include <direct.h>

using namespace std;

void printm(MAT * S, const char * caption)
{
	int w, h;
	printf( "------------------------%s------------------------\n", caption);
	for (w = 0; w < S->m; w ++) {
		for (h = 0; h < S->n; h ++) {
			printf( "%f ",S->me [w][h]);
		}
		printf( "\n");
	}
	printf( "------------------------%s------------------------\n", caption);
}

void printv(VEC * v, const char * caption)
{
	int i;
	printf( "------------------------%s------------------------\n", caption);
	for (i=0;i<v->dim;i++)
		printf( "%f ",v->ve[i]);
	printf( "------------------------%s------------------------\n", caption);
}

char misc :: tchar_to_char( TCHARMISC t_ch )
{
#ifdef _UNICODE
  char ch = (char)t_ch;
  return ch;
#else
  return t_ch;
#endif
}

TCHARMISC misc :: char_to_tchar( char ch )
{
#ifdef _UNICODE
  TCHARMISC t_ch = (TCHARMISC)ch;
  return t_ch;
#else
  return ch;
#endif
}

string misc :: tstring_to_string( const TCHARMISC* t_ch )
{
#ifdef _UNICODE
  std::string str;
  while( 1 ) {
    str += (char)*t_ch ;
  if ( (char)*t_ch++ == '\0' )
    return str;
  }
#else
  std::string str = std::string( (char *)t_ch );
  return str;
#endif
}

string misc :: tstring_to_string( const tstring & t_str )
{
#ifdef _UNICODE
    char* buffer = new char[t_str.size() + 1];
    wcstombs( buffer, t_str.c_str(), t_str.size() );
    buffer[t_str.size()] = 0;
    std::string str = buffer;
    delete [] buffer;
    return str;
#else
    return t_str;
#endif
}

tstring misc :: string_to_tstring( const char* ch )
{
#ifdef _UNICODE
  std::wstring wstr;
  while( 1 ) {
    wstr += (TCHARMISC)*ch ;
  if ( (TCHARMISC)*ch++ == '\0' )
    return wstr;
  }
#else
  std::string str = string( ch );
  return str;
#endif
}

tstring misc :: string_to_tstring( const string & str )
{
#ifdef _UNICODE
    wchar_t* buffer = new wchar_t[str.size() + 1];
    mbstowcs( buffer, str.c_str(), str.size() );
    buffer[str.size()] = 0;
    std::wstring wstr = buffer;
    delete [] buffer;
    return wstr;
#else
    return str;
#endif
}

void misc::containerToMAT (
    MAT * matTo, DSContainer * containerFrom, int numComponent) 
{
  // Assume that the container already contains correct information
  // Instantiation of the variables
  unsigned long w = -1, h = -1;
  unsigned int d = -1;
  float element [3];
  int frame_id, pixel_id;
  enum ComponentType element_type;

  // Initialisation of the variables
  containerFrom->GetComponentNumberOfColumnElementsPerFrame(numComponent, &w);
  containerFrom->GetComponentNumberOfRowElementsPerFrame(numComponent, &h);
  containerFrom->GetComponentNumberOfFrames(numComponent, &d);
  containerFrom->GetComponentElementType(numComponent, &element_type);

  // Test if the dimensions of matTo are correct - so that we don't have GP faults
  // Check if the number of rows is equal to the number of image pixels and each row has one element only.
  if (matTo->m != w*h*d || matTo->n != 1) 
  {
    return;
  }

  // Copy the elements of the input component to the output matrix
  for (frame_id = 0; frame_id < d; frame_id++) 
  { 
    for (pixel_id = 0; pixel_id < w*h; pixel_id++) 
	{
      containerFrom->GetComponentElement(numComponent, pixel_id, frame_id, &element);
	  if (TYPE_FLOAT  == element_type || TYPE_IMAGE_JND1 == element_type ) 
	  {
        matTo->me[frame_id*w*h+pixel_id][0] = element[0];			
      } 
	  else if (element_type == TYPE_IMAGE_XYZ) 
	  {
        matTo->me[frame_id*w*h+pixel_id][0] = element[1];			
      }
    }
  }
}

void misc::frameToMAT (
    MAT * matTo, DSContainer * containerFrom, int numComponent, int numFrame) 
{

  // Assume that the container already contains correct information
  // Instantiation of the variables
  unsigned long w = -1, h = -1;
  float element [3];
  int i;
  enum ComponentType element_type;

  // Initialisation of the variables
  containerFrom->GetComponentNumberOfColumnElementsPerFrame(numComponent, &w);
  containerFrom->GetComponentNumberOfRowElementsPerFrame(numComponent, &h);
  containerFrom->GetComponentElementType(numComponent, &element_type);

  // Test if the dimensions of matTo are correct - so that we don't have GP faults
  // Check if the number of rows is equal to the number of image pixels and each row has one element only.
  if(matTo->m != w*h || matTo->n != 1) {
    return;
  }

  // Copy the elements of the input component to the output matrix
  for (i = 0; i < matTo->m; i ++) 
  {
	containerFrom->GetComponentElement(numComponent, i, numFrame, &element);
	if (TYPE_FLOAT  == element_type || TYPE_IMAGE_JND1 == element_type ) 
	{
      matTo->me[i][0] = element[0];			
    } else if (element_type == TYPE_IMAGE_XYZ) {
      matTo->me[i][0] = element[1];			
    }
  }
}

std::string misc::BackSlash2ForwardSlash(const std::string & str, bool append_path_delimiter)
{
  std::string str_new = std::string(str);
  while(str_new.find_last_of('\\') != std::string::npos) 
  {
    str_new = str_new.replace(str_new.find_last_of('\\'), 1, "/");
  }
  if (append_path_delimiter && str_new[str_new.length()]!='/')
    str_new += "/"; // append path delimiter

  return str_new;
}

bool misc::DirectoryExists(const std::string & path)
{	
  boost::filesystem::path dirpath(path.c_str());  

  if (boost::filesystem::exists(dirpath) && boost::filesystem::is_directory(dirpath))
    return true;

  dirpath = ExtractDirectoryPath(path);
  
  return (boost::filesystem::exists(dirpath) && boost::filesystem::is_directory(dirpath));
}

bool misc::MakeDirectory(const std::string & path)
{
  std::string dir = ExtractDirectoryPath(path);
  const boost::filesystem::path dirpath(dir.c_str());

  if (boost::filesystem::exists(dirpath) && boost::filesystem::is_directory(dirpath))
    return true;

  try 
  {
	  _mkdir(dir.c_str()); 
  }
  catch (const boost::filesystem::basic_filesystem_error<boost::filesystem::path> & my_err) 
  { 
	  ErrorLogger::logError("[misc::MakeDirectory] Directory creation failed");
    return false;
  }

  return true;
}

std::string misc::ExtractDirectoryPath(const std::string & path)
{
  int len=path.length();
  std::string dirpath = path;
  while (len > 0 && 
    (dirpath[len-1] != '/' && dirpath[len-1] != '\\'))
  {
    dirpath.erase(--len);
  }
  dirpath.erase(--len);
  return dirpath;
}

vector <std::string> misc :: getFilePathList( 
  const std::string & dirpath, const std::string & search_criterion, 
  MevicLogger * pLogger) 
{
  std::string filepath, filename;
  vector <std::string> toReturn;
  // FileFinder object, providing directory browsing functionality
	FileFinder fileFinder; 
	bool create_state, // State of the initialization (create parameter); true if OK (done and successfully initialized), false if not OK (not yet done or error)
       found_state;  // State of the files browsing; true if OK (file found), false if not OK (no more files found)

  // initialize search; target search criterium (wildcard): all files matching *.bin
	create_state = fileFinder.initSearch(dirpath, search_criterion);
	if (!create_state) 
	{
		pLogger->logError("misc::getFullPath: Unable to initialize file search.");
		return toReturn;
	}

	// Find first file
	found_state = fileFinder.findFirst(filename);
	if (!found_state) 
	{	
		pLogger->logError("misc::getFullPath: Invalid file (file not found)");
		return toReturn;
	}

  // Process
  while ( found_state ) 
  {
    // build the full path of the filename
    filepath = dirpath + filename;
    // add filename to the output list
    toReturn.push_back(filepath);
		// Find next file
		found_state = fileFinder.findNext(filename);
  }

  // return the full list of filenames in the given folder
  if (pLogger!=NULL) 
  {
    for ( int i = 0 ; i < toReturn.size(); i++ ) 
	{
      pLogger->logProgress(toReturn.at(i));
    }
  }

  return toReturn;

}

