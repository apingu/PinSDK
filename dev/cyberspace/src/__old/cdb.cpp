#include "CDB.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
CDB::CDB()
{


}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
CDB::~CDB()
{


}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CDB::Load(char *filename)
{


}
//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CDB::PutField(int col, char* data)
{
    PDATA d(data);
    FIELD.Put(col, &d);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
bool CDB::Save(char *filename)
{
    FILE *Sf;
    int  Info_Field_Size = sizeof(DWORD);
    int  DB_H_SIZE       = Info_Field_Size * 4;

    //open write file
    if((Sf = fopen(filename, "wb")) == NULL)
    {
        return false;
    }
    //file title
    char Title[4] = "CDB" ;
    fwrite(Title, Info_Field_Size, 1, Sf);

    //check code
    fwrite(&KEY, sizeof(KEY), 1, Sf);

    //field count
    int FieldCounts = FIELD.size();

    //field Length
    int FieldLength = 0;
    for(int i = 0; i < FieldCounts; i++)
    {
        FieldLength = FieldLength + FIELD(i);
    }

    //conpute data start address offset
    int DataStartOffSet = DB_H_SIZE + ((FieldCounts + 1) * 4 ) + FieldLength;

    //write data start address offset
    fwrite(&DataStartOffSet, Info_Field_Size, 1, Sf);

    //space for comment offset
    fseek(Sf, 12, SEEK_SET);


//==fields======================================================================
    //write field counts
    fwrite(&FieldCounts, Info_Field_Size, 1, Sf);

    //fiels offset
    int FieldOffset = DB_H_SIZE + (FieldCounts + 1) * Info_Field_Size;
    for(int j = 1; j <= FieldCounts; j++)
    {
        fwrite(&FieldOffset, Info_Field_Size, 1, Sf);
        FieldOffset = FieldOffset + strlen(FIELD(j));
    }
    fwrite(&FieldOffset, Info_Field_Size, 1, Sf);

    //field name
    for(int j = 1 ; j <= FieldCounts; j++)
    {
        int fieldsize = strlen(FIELD(j));
        fwrite(FIELD(j), fieldsize, 1, Sf);
     }
//==end fields==================================================================


//==Data========================================================================

    int RowCount = (DATA.Row_Size());
    fwrite(&RowCount, 4, 1, Sf);

    fclose(Sf);
    return true;


//==end Data====================================================================


//==comment=====================================================================

//==end comment=================================================================
    fclose(Sf);

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void CDB::CreateField(int index, char Name)
{


}

void Load_H_File(char *Hfilename)
{
	FILE* f;

	if((f = fopen(Hfilename, "rb")) == NULL)
		return ;

    int filesize = filelength(fileno(f));
	char* fileptr = new char[filesize];
	fread(fileptr, filesize, 1, f);

	char String[_MAX_STRING_SIZE][_MAX_STRING_LEN];
	int size = DivStr(fileptr, filesize, String);

	m_fieldsize.ReAllot(size);

	bool StartStruct = false;
	int  field = 0;

	for(int StrIndex = 0; StrIndex < size + 1; StrIndex++)
	{
		if(strcmp(String[StrIndex], "{") == 0)
		{
			StrIndex++;
			StartStruct = true;
		}
		if(StartStruct)
		{
			char Item[_MAX_ITEMS][_MAX_LENGTH];
			strcpy(Item[1], String[StrIndex]);
			int typesize = SelectTypeSize(Item[1]);
			m_fieldsize[field] = typesize;
			StrIndex++;
			DivChar(String[StrIndex], ';');
			strcpy(Item[0], String[StrIndex]);
		    AddFileListItem(field, Item, 2);
			field++;
		}
		if(strcmp(String[StrIndex + 1], "};") == 0)
			break;
	}

}


void Save_H_File(char *Hfilename)
{



}



