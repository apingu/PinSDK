#ifndef  _PDB_H_
#define  _PDB_H_


#include <stdio.h>
#include "..\\Tools\\str.h"
#include "..\\Container\\ARRAY.H"
#include "..\\Container\\DynArray.H"

class CDB : public MESH<PDATA*>
{
    int KEY;

	typedef ARRAY<BYTE>    cell;
    typedef DynArray<cell> row;
	DynArray<row> m_DataTable;


public:

    CDB();
    ~CDB();

    
	void Load_H_File(char *Hfilename);
	void Save_H_File(char *Hfilename);

	void LoadPDB(char *filename);
    void SavePDB(char *filename);

    void writeField  (int col, char* data);           //write field name
	void writeData   (int col, int row, char* data);  //write data
	void writeComment(int col, int row, char* data);  //write comment

};

#endif
