//
//
//
//
//
//
//
//
//              Copyright (C) 2000 Pin
//

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-stdlib.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "pgsp_file.h"

const   tchar_t   GspFileVersion[ 512 ] = { "Gsp_version_0.5" };
typedef tchar_t   ScriptWord[ 512 ];


///////////////////////////////////////////////////////////////////////////////////////
//
//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspReadCallback::GspReadCallback()
{


}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspReadCallback::~GspReadCallback()
{


}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspObject GspReadCallback::GetParentOject( void )
{
    if( !m_GspObjList.empty() )
    {
        return m_GspObjList[ m_GspObjList.size() - 1 ];
    }
    return NULL;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspNode* GspReadCallback::GetCurNode( void )
{
    return m_CurNode;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
GspNode::GspNode()
{
    m_Tabsize = 4;
    //find this key first
    m_IndexKey=0;
    m_Parent=NULL;
    m_ChildIndex=0;
}
*/

//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspNode::GspNode( const tchar_t* style, const tchar_t* description )
{
    m_Tabsize = 4;
    //find this key first
    m_IndexKey = 0;
    m_Parent = NULL;
    if( style != NULL )
    {
        m_Style = style;
    }
    if( ( description != NULL ) && ( strcmp( description, "" ) != 0 ) )
    {
        m_Description = description;
    }
    else
    {
        m_Description.clear();
    }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspNode::~GspNode()
{
    Clear();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void GspNode::Clear( void )
{
    for( uint_t i = 0; i < child_nodes_.size(); i++ )
    {
        child_nodes_[ i ]->Clear();
        SAFE_DELETE( child_nodes_[ i ] );
    }

    child_nodes_.clear();

    for( unsigned int i = 0; i < m_FeatureLine.size(); i++ )
    {
        m_FeatureLine[ i ].m_value.clear();
        m_FeatureLine[ i ].m_feature.clear();
        m_FeatureLine[ i ].m_operator.clear();
    }
    m_FeatureLine.clear();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int GspNode::GetDepth( void )
{
    uint_t depth = 0;
    GspNode* parent = this;
    while( ( parent = parent->m_Parent ) != NULL )
    {
        depth++;
    }
    return depth;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspNode* GspNode::operator[] ( uint_t i )
{
    if( i < child_nodes_.size() )
    {
        return child_nodes_[ i ];
    }
    return NULL;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int GspNode::Save( void* stream )
{
    tchar_t buf[ _MAX_MSG ] = { " " };
    int depth = GetDepth();

    //write <XXXXXX>
    if( depth != 0 )
    {
        memset( buf, ' ', depth * m_Tabsize );
        _pfwrite( buf, depth, m_Tabsize, _pffile( stream ) );
    }

    if( !m_Description.empty() )
    {
        char descbuff[ _MAX_MSG ];
        strcpy( descbuff, m_Description.c_str() );
        strextractstring( descbuff, " \"" );
        _stprintf( buf, "<%s description=\"%s\" >\n", m_Style.c_str(), descbuff );
    }
    else
    {
        _stprintf( buf, "<%s>\n", m_Style.c_str() );
    }

    _pfwrite( buf, strlen( buf ), 1, _pffile( stream ) );

    //strcpy(buf, " ");
    //Pfwrite("", depth+1, m_Tabsize, stream);
    for( uint_t a = 0; a < m_AnnotationLine.size(); a++ )
    {
        memset( buf, ' ', ( depth + 1 )*m_Tabsize );
        _pfwrite( buf, depth + 1, m_Tabsize, _pffile( stream ) );

        if( ( ( ( m_AnnotationLine[ a ].c_str() )[ 0 ] != '/' ) &&
            ( ( m_AnnotationLine[ a ].c_str() )[ 1 ] != '/' ) ) ||
            ( ( ( m_AnnotationLine[ a ].c_str() )[ 0 ] != '<' ) &&
            ( ( m_AnnotationLine[ a ].c_str() )[ 1 ] != '!' ) &&
            ( ( m_AnnotationLine[ a ].c_str() )[ 2 ] != '-' ) &&
            ( ( m_AnnotationLine[ a ].c_str() )[ 3 ] != '-' ) ) )
        {
            _stprintf( buf, "<!--%s-->\n", m_AnnotationLine[ a ].c_str() );
        }
        else
        {
            _stprintf( buf, "%s\n", m_AnnotationLine[ a ].c_str() );
        }

        _pfwrite( buf, strlen( buf ), 1, _pffile( stream ) );
    }

    for( uint_t i = 0; i < m_FeatureLine.size(); i++ )
    {
        Command comm = m_FeatureLine[ i ];
        memset( buf, ' ', ( depth + 1 )*m_Tabsize );
        _pfwrite( buf, depth + 1, m_Tabsize, _pffile( stream ) );
        _stprintf( buf, "%s %s %s\n", m_FeatureLine[ i ].m_feature.c_str(), m_FeatureLine[ i ].m_operator.c_str(), m_FeatureLine[ i ].m_value.c_str() );
        _pfwrite( buf, strlen( buf ), 1, _pffile( stream ) );
    }

    for( uint_t j = 0; j < child_nodes_.size(); j++ )
    {
        child_nodes_[ j ]->Save( _pffile( stream ) );
    }

    if( depth != 0 )
    {
        memset( buf, ' ', depth * m_Tabsize );
        _pfwrite( buf, depth, m_Tabsize, _pffile( stream ) );
    }
    _stprintf( buf, "</%s>\n", m_Style.c_str() );
    _pfwrite( buf, strlen( buf ), 1, _pffile( stream ) );

    //Pfclose(stream);
    return 1;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspNode* GspNode::GetParent( void )
{
    return m_Parent;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int GspNode::SetIndexKey( const tchar_t* name, const tchar_t* oper )
{
    for( unsigned int i = 0; i < m_FeatureLine.size(); i++ )
    {
        if( ( m_FeatureLine[ i ].m_feature == name ) && ( m_FeatureLine[ i ].m_operator == oper ) )
        {
            return m_IndexKey = i;
        }
    }
    return -1;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
GspNode* GspNode::FindChildNode( const tchar_t* name, const tchar_t* oper )
{
    //找自己
    uint_t i = 0;
    /*
    if((m_FeatureLine[m_IndexKey].m_feature == name) &&
        (m_FeatureLine[m_IndexKey].m_operator == oper))
    {
        return this;
    }

    for(i = 0; i<m_FeatureLine.size(); i++)
    {
        if((m_FeatureLine[i].m_feature == name) &&
            (m_FeatureLine[i].m_operator == oper))
        {
            return this;
        }
    }
    */

    GspNode* childnode = NULL;

    //search indexkey
    for( i = 0; i < child_nodes_.size(); i++ )
    {
        childnode = child_nodes_[ i ];
        if( ( childnode->m_FeatureLine[ childnode->m_IndexKey ].m_feature == name ) && ( childnode->m_FeatureLine[ childnode->m_IndexKey ].m_operator == oper ) )
        {
            return childnode;
        }
    }

    //search all
    for( i = 0; i < child_nodes_.size(); i++ )
    {
        childnode = child_nodes_[ i ];
        for( unsigned c = 0; c < childnode->m_FeatureLine.size(); c++ )
        {
            if( ( childnode->m_FeatureLine[ c ].m_feature == name ) && ( childnode->m_FeatureLine[ c ].m_operator == oper ) )
            {
                return childnode;
            }
        }
    }

    for( i = 0; i < child_nodes_.size(); i++ )
    {
        GspNode* node = child_nodes_[ i ]->FindChildNode( name, oper );
        if( node != NULL )
        {
            return node;
        }
    }
    return NULL;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int GspNode::ReplaceNode( GspNode* src, GspNode* det )
{
    uint_t i = 0;

    //search indexkey
    for( i = 0; i < child_nodes_.size(); i++ )
    {
        if( src == child_nodes_[ i ] )
        {
            det->m_Parent = child_nodes_[ i ]->m_Parent;
            child_nodes_[ i ] = det;
            return 1;
        }
    }

    //search all
    for( i = 0; i < child_nodes_.size(); i++ )
    {
        if( child_nodes_[ i ]->ReplaceNode( src, det ) == 1 )
        {
            return 1;
        }
    }

    return 0;

}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void GspNode::AttachChildNode( GspNode* child )
{
    child->m_Parent = this;
    child_nodes_.push_back( child );
    return;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void GspNode::DetachChildNode( GspNode* child )
{
    for( unsigned i = 0; i < child_nodes_.size(); i++ )
    {
        if( child_nodes_[ i ] == child )
        {
            child->m_Parent = NULL;
            child_nodes_.erase( child_nodes_.begin() + i );
            return;
        }
    }
    return;
}

/*
//=============================================================================
// NAME:
// DESC:
//=============================================================================
void GspNode::AttachCommand(const tchar_t* name, const tchar_t* oper, const tchar_t* valu)
{
    Command comm;
    comm.m_feature = name;
    comm.m_operator  = oper;
    comm.m_value  = valu;
    m_FeatureLine.push_back(comm);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void GspNode::DetachCommand(const tchar_t* name, const tchar_t* oper)
{
 for(unsigned int i = 0; i<m_FeatureLine.size(); i++)
    {
        if((m_FeatureLine[i].m_feature == name) &&
            (m_FeatureLine[i].m_operator == oper))
        {
            m_FeatureLine.erase(m_FeatureLine.begin() + i);
            return;
        }
    }
}
*/

const tchar_t* GspNode::GetStyle( void )
{
    return m_Style.c_str();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
const tchar_t* GspNode::GetValue( const tchar_t* name, const tchar_t* oper )
{
    for( unsigned int i = 0; i < m_FeatureLine.size(); i++ )
    {
        if( ( m_FeatureLine[ i ].m_feature == name ) && ( m_FeatureLine[ i ].m_operator == oper ) )
        {
            return m_FeatureLine[ i ].m_value.c_str();
        }
    }
    return NULL;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================

void GspNode::WriteAnnotation( const tchar_t* descriptor )
{
    m_AnnotationLine.push_back( descriptor );
    return;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool GspNode::WriteCommand( const tchar_t* name, const tchar_t* oper, const tchar_t* value )
{
    std::vector<Command>* pContainer = &m_FeatureLine;

    for( unsigned int i = 0; i < pContainer->size(); i++ )
    {
        if( ( ( *pContainer )[ i ].m_feature == name ) && ( ( *pContainer )[ i ].m_operator == oper ) )
        {
            ( *pContainer )[ i ].m_value = value;
            return true;
        }
    }

    Command comm;
    comm.m_feature = name;
    comm.m_operator = oper;
    if( value != NULL )
    {
        comm.m_value = value;
    }
    pContainer->push_back( comm );

    //AttachCommand(name, oper, value);
    return false;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool GspNode::EraseCommand( const tchar_t* name, const tchar_t* oper )
{
    std::vector<Command>* pContainer = &m_FeatureLine;

    for( unsigned int i = 0; i < pContainer->size(); i++ )
    {
        if( ( ( *pContainer )[ i ].m_feature == name ) && ( ( *pContainer )[ i ].m_operator == oper ) )
        {
            pContainer->erase( pContainer->begin() + i );
            return true;
        }
    }
    return false;
}



//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool GspNode::DoPass( GspReadCallback* gcb )
{
    if( gcb == NULL )
    {
        return false;
    }

    _tstring style_name = m_Style;
    GspObject object = NULL;
    //GspObject parent = gcb->GetParentOject();

    if( strcmp( style_name.c_str(), GspFileVersion ) != 0 )
    {
        gcb->m_CurNode = this;
        object = gcb->OnCreateObject( style_name.c_str(), m_Description.c_str(), gcb->GetParentOject() );
        //return false;

        gcb->m_GspObjList.push_back( object );

        for( unsigned int i = 0; i < m_FeatureLine.size(); i++ )
        {
            gcb->OnReadAttribute( style_name.c_str(), object,
                m_FeatureLine[ i ].m_feature.c_str(),
                m_FeatureLine[ i ].m_operator.c_str(),
                m_FeatureLine[ i ].m_value.c_str() );
        }
    }

    //int childrensize = child_nodes_.size();
    for( unsigned int cj = 0; cj < child_nodes_.size(); cj++ )
    {
        //int csize = child_nodes_.size();
        if( child_nodes_[ cj ]->DoPass( gcb ) == false )
        {
            return false;
        }
    }

    if( strcmp( style_name.c_str(), GspFileVersion ) != 0 )
    {
        gcb->m_CurNode = this;
        gcb->OnCloseObject( style_name.c_str(), object );
        gcb->m_GspObjList.erase( gcb->m_GspObjList.end() - 1 );
    }

    return true;
}


//=============================================================================
// NAME: <xxxxx>  ==>1  begin object
//   </xxxxx> ==>2  end object
//   //    ==>3  change next line
//   <?--  ==>3  change next line
//   @  ==>4  index key
//
// DESC:
//=============================================================================
int JudgKeywordType( _tstring str )
{
    if( str.empty() )
    {
        return -1;
    }

    //如果後面接 // 會盪
    int len = strlen( str.c_str() ) - 1;
    if( ( str[ 0 ] == '<' ) && ( str[ len ] == '>' ) )
    {
        if( strstr( str.c_str(), GspFileVersion ) != NULL )
        {
            return 5;
        }//version word, read next word
        if( str[ 1 ] != '/' )
        {
            return 1;
        }//create object
        return 2;//end object
    }

    // 註解 或不知道甚麼之類的
    if( ( str[ 0 ] == '/' ) && ( str[ 1 ] == '/' ) )
    {
        return 3;
    }
    if( ( str[ 0 ] == '<' ) && ( str[ 1 ] == '?' ) && ( str[ 1 ] == '-' ) && ( str[ 1 ] == '-' ) )
    {
        return 3;
    }
    if( str[ 0 ] == '@' )
    {
        return 4;
    }
    if( str[ 0 ] == 0X09 ) //tab
    {
        return 5;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//=============================================================================
// NAME:
// DESC:
//=============================================================================
CPGspParser::CPGspParser(): GspNode( GspFileVersion )
{
    //GspNode::GspNode(GspFileVersion);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
CPGspParser::~CPGspParser()
{

}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int CPGspParser::Load( const tchar_t* filename )
{
    //tchar_t gspword[512];
    uPfFile f;
    if( ( f = _tfopen( filename, "rb" ) ) == NULL )
    {
        return -1;
    }

    int filesize = _pffilelength( f );
    tchar_t* pFilebuff = new tchar_t[ filesize + 1 ];
    _pfread( pFilebuff, filesize, 1, f );
    pFilebuff[ filesize ] = 0;
    int ret = LoadFromString( pFilebuff );
    SAFE_DELETE_ARRAY( pFilebuff );
    return ret;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int CPGspParser::LoadFromString( const tchar_t* pString )
{
    Clear();

    GspNode* pnode = this;
    GspNode* cnode = NULL;

    tchar_t* pFilebuff = new tchar_t[ strlen( pString ) + 1 ];
    strcpy( pFilebuff, pString );

    tchar_t* gspword = strtok( pFilebuff, "\t\n\r" );
    while( gspword )
    {
        //f.readline(gspword, 512);
        // 要清除 前後的空白
        strextractstring( gspword );
        //sscanf(gspword, "%s", gspword);
        switch( JudgKeywordType( gspword ) )
        {
        case 0://read attribute
            {
                tchar_t feature[ 512 ], oper[ 512 ], objvalue[ 512 ];
                sscanf( gspword, "%s %s %s", feature, oper, objvalue );
                //f.readword(oper, 512);
                //f.readline(objvalue, 512);
                cnode->WriteCommand( feature, oper, objvalue );
                break;
            }
        case 1://create node
            {
                gspword[ strlen( gspword ) - 1 ] = '\0'; // erase >
                char elementname[ 512 ];
                strcpy( elementname, ( gspword + 1 ) ); // erase <

                char nodename[ 512 ] = { "" };
                char nodedescrption[ 512 ] = { "" };

                sscanf( elementname, "%s description=%s", nodename, nodedescrption );
                strextractstring( nodedescrption, " \"" );

                cnode = new GspNode( nodename, nodedescrption );
                pnode->AttachChildNode( cnode );
                pnode = cnode;

                break;
            }
        case 2://end node
            {
                pnode = cnode->GetParent();
                cnode = pnode;
                //if(pnode == NULL)///==>do something
                break;
            }
        case 3:
            {
                //f.gotonextline();
                break;
            }
        case 4:
            {
                tchar_t feature[ 512 ], oper[ 512 ], objvalue[ 512 ];
                sscanf( gspword, "@%s %s %s", feature, oper, objvalue );
                cnode->WriteCommand( feature, oper, objvalue ); //erase @
                cnode->SetIndexKey( feature, oper );
            }
        }
        gspword = strtok( NULL, "\t\n\r" );
    }

    SAFE_DELETE_ARRAY( pFilebuff );

    if( pnode != this )
    {
        Pf_ERROR( "CPGspParser::LoadFromString: gsp failure!!" );
        return -1;
    }
    return 1;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int CPGspParser::Save( const tchar_t* filename )
{
    void* stream = _tfopen( filename, "wb" );
    if( stream == NULL )
    {
        return -1;
    }

    GspNode::Save( stream );
    _pfclose( _pffile( stream ) );
    return 1;
}



/*
//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGspParser::Open(const tchar_t* filename)
{
 PFile::open(filename, "wb");
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGspParser::CreateObject(const tchar_t* Name)
{
 tchar_t tabspace[255];
 tchar_t Sword[255];
 makespace(tabspace, TAB_SPACE_SIZE * GetDepth());
 _stprintf(Sword, "%s<%s>\n", tabspace, Name);
 write(Sword, strlen(Sword), 1);

 GSPItem sobj;
 strcpy(sobj.name, Name);
 m_ObjectList.push_back(sobj);
 //m_Depth++;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGspParser::CloseObject(void)
{
 //m_Depth--;
 tchar_t tabspace[255];
 tchar_t Sword[255];
 makespace(tabspace, TAB_SPACE_SIZE * (GetDepth() - 1));
 _stprintf(Sword, "%s</%s>\n", tabspace, m_ObjectList.back().name);
 write(Sword, strlen(Sword), 1);
 m_ObjectList.erase(m_ObjectList.end());
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGspParser::SetReadAttribute(const tchar_t* Attribute, const tchar_t* Value)
{
    if(Value == NULL)
        return;
 tchar_t tabspace[255];
 tchar_t Sword[255];
 makespace(tabspace, TAB_SPACE_SIZE * GetDepth());
 _stprintf(Sword, "%s%s = %s\n", tabspace, Attribute, Value);
 write(Sword, strlen(Sword), 1);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGspParser::Close(void)
{
 PFile::close();
}
*/



///////////////////////////////////////////////////////////////////////////////////////
//


/*
class CPGSPFile : private IO::PFile
{
struct GSPItem
{
tchar_t name[512];
void*  data;
};

struct GSPValue
{
Util::String_t string;
double   data;
};

//int m_Depth;
pfstl::delist<GSPItem> m_ObjectList;
pfstl::delist<GSPValue> m_ValueList;


protected:

int GetDepth(void){ return m_ObjectList.size(); }

const tchar_t* GetParent(void);
void*   GetParentData(void);

public:

CPGSPFile();
~CPGSPFile();

GspNode m_RootNode;

void AddDefineValue(const tchar_t* filename);
void AddDefineValue(const tchar_t* name, double value);

virtual void OnInitiate(void){};

//write function
void Open   (const tchar_t* filename);
void CreateObject (const tchar_t* name);
void CloseObject  (void);
void SetReadAttribute(const tchar_t* attribute, const tchar_t* value);
void Close     (void);


//read function
int Load(const tchar_t* filename);

virtual WORD   GetVard(const tchar_t* Variable) { return (WORD) GetVarb(Variable); }
virtual int GetVari(const tchar_t* Variable) { return (int) GetVarb(Variable);  }
virtual float  GetVarf(const tchar_t* Variable) { return (float) GetVarb(Variable);}
virtual double GetVarb(const tchar_t* Variable);


virtual void*  OnCreateObject (const tchar_t* name, void* parent) = 0;
virtual void   OnReadAttribute(const tchar_t* name,
GspObject object,
const tchar_t* attribute,
const tchar_t* oper,
const tchar_t* value)=0;
virtual void OnCloseObject (const tchar_t* name, void* obj) = 0;
};
*/

/*
//=============================================================================
// NAME:
// DESC:
//=============================================================================
CPGSPFile::CPGSPFile() : m_RootNode(NULL)
{
 //m_Depth = 0;
}



//=============================================================================
// NAME:
// DESC:
//=============================================================================
CPGSPFile::~CPGSPFile()
{
 //m_Depth = 0;
}




//=============================================================================
// NAME:
// DESC:
//=============================================================================
const tchar_t* CPGSPFile::GetParent(void)
{
 int l = m_ObjectList.size() - 2;
 if (l < 0)
  return NULL;
 return m_ObjectList[l].name;
}


void* CPGSPFile::GetParentData(void)
{
 int l = m_ObjectList.size() - 2;
 if (l < 0)
  return NULL;
 return m_ObjectList[l].data;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGSPFile::AddDefineValue(const tchar_t* name, double value)
{
 GSPValue v;
 v.string = name;
 v.data = value;
 m_ValueList.push_back(v);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGSPFile::AddDefineValue(const tchar_t* filename)
{
 tchar_t gspword[255];

 open(filename);

 while (!eof())
 {
  readword(gspword, 255);
  if (strcmp(gspword, "#define") == 0)
  {
   tchar_t str[255], value[255];
   readword(str, 255);

   double v = 0;
   readword(value, 255);
   if (isdigit(value))// it is a digit
   {
    v = atof(value);
   }
   if (isxdigit(value))
   {
    v = (double) atox(value);
   }
   AddDefineValue(str, v);
  }
 }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
double CPGSPFile::GetVarb(const tchar_t* Variable)
{
 for (uint_t i = 0; i < m_ValueList.size(); i++)
 {
  if (strcmp(m_ValueList[i].string, Variable) == 0)
   return m_ValueList[i].data;
 }
 return 0;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int CPGSPFile::Load(const tchar_t* filename)
{
 tchar_t gspword[255];
 open(filename, "rb");

    GSPItem sobj;

 while (!eof())
 {
        readword(gspword, 255);
  switch (JudgKeywordType(gspword))
  {
    case 0://read attribute
   {
    tchar_t oper[255], objvalue[255];
    readword(oper, 255);
    if (strcmp(oper, "=") == 0)
    {
     readline(objvalue, 255);
     OnReadAttribute(m_ObjectList.back().name,
          m_ObjectList.back().data,
          gspword,
                                     oper,
          objvalue);
    }
    else
    {
     DebugMessage("read word failure!!", "format");
     return -1;
    }
    break;
   }
    case 1://create node
   {
                //cnode = new GspNode(gspword);
                //pnode->AttachChildNode(cnode);
                void* parentdata=NULL;
                 if(!m_ObjectList.empty())
                     parentdata= m_ObjectList.back().data;


    strcpy(sobj.name, (gspword + 1));
    sobj.name[strlen(sobj.name) - 1] = '\0';
    sobj.data = OnCreateObject(sobj.name, parentdata);
    if (sobj.data == NULL)
    {
     DebugMessage("create object failure!!", sobj.name);
     break;
    }
    m_ObjectList.push_back(sobj);
    //m_Depth++;
    break;
   }
    case 2://end node
   {
    OnCloseObject(m_ObjectList.back().name,
          m_ObjectList.back().data);
    m_ObjectList.erase(m_ObjectList.end());
    //m_Depth--;
    break;
   }
    case 3:
   {
    gotonextline();
    //gotoline(4);
    break;
   }
  }
 }

 if (GetDepth() != 0)
  DebugMessage("gsp failure!!", "format");

 //m_Depth=0;
 m_ObjectList.clear();

 return 1;
}


#define TAB_SPACE_SIZE 4
void makespace(tchar_t* buf, int size)
{
 buf[size] = '\0';
 memset(buf, 0X20, size);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGSPFile::Open(const tchar_t* filename)
{
 PFile::open(filename, "wb");
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGSPFile::CreateObject(const tchar_t* Name)
{
 tchar_t tabspace[255];
 tchar_t Sword[255];
 makespace(tabspace, TAB_SPACE_SIZE * GetDepth());
 _stprintf(Sword, "%s<%s>\n", tabspace, Name);
 write(Sword, strlen(Sword), 1);

 GSPItem sobj;
 strcpy(sobj.name, Name);
 m_ObjectList.push_back(sobj);
 //m_Depth++;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGSPFile::CloseObject(void)
{
 //m_Depth--;
 tchar_t tabspace[255];
 tchar_t Sword[255];
 makespace(tabspace, TAB_SPACE_SIZE * (GetDepth() - 1));
 _stprintf(Sword, "%s</%s>\n", tabspace, m_ObjectList.back().name);
 write(Sword, strlen(Sword), 1);
 m_ObjectList.erase(m_ObjectList.end());
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGSPFile::SetReadAttribute(const tchar_t* Attribute, const tchar_t* Value)
{
    if(Value == NULL)
        return;
 tchar_t tabspace[255];
 tchar_t Sword[255];
 makespace(tabspace, TAB_SPACE_SIZE * GetDepth());
 _stprintf(Sword, "%s%s = %s\n", tabspace, Attribute, Value);
 write(Sword, strlen(Sword), 1);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPGSPFile::Close(void)
{
 PFile::close();
}
*/
