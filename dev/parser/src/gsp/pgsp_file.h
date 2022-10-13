//
//
//
//   <ojbect>
//  @indexkey =
//  attribute0=
//  attribute1=
//   </ojbect>
//
//
//
//
//
//              Copyright (C) 2000 Pin
//
#ifndef PGSPFILE_H
#define PGSPFILE_H

#include <string>
#include <vector>

#include <pf-def.h>
#include <template/delist.inl>


typedef void* GspObject;
class GspNode;

class GspReadCallback
{
    friend class GspNode;
    
public:
    GspReadCallback();
    ~GspReadCallback();
    
    GspObject          GetParentOject ( void );
    GspNode*           GetCurNode     ( void );
    virtual GspObject  OnCreateObject ( const tchar_t* name, const tchar_t* description, GspObject parent ) = 0;
    virtual void       OnReadAttribute( const tchar_t* name, GspObject object, const tchar_t* attribute, const tchar_t* oper, const tchar_t* value ) = 0;
    virtual void       OnCloseObject  ( const tchar_t* name, GspObject object ) = 0;
    
    //public:
private:

    GspNode*               m_CurNode;
    std::vector<GspObject> m_GspObjList;
    
};



///////////////////////////////////////////////////////////////////////////////////////
//
enum GspCommandType
{
    GCT_FEATURE,
    GCT_ATTRIBUTE,
};

class GspNode
{
public:

    GspNode( const tchar_t* style, const tchar_t* description = NULL );
    ~GspNode();
    
    void        Clear           ( void );
    
    int         SetIndexKey     ( const tchar_t* attr, const tchar_t* oper );
    
    GspNode*    GetParent       ( void );
    GspNode*    FindChildNode   ( const tchar_t* attr, const tchar_t* oper );
    int         ReplaceNode     ( GspNode* src, GspNode* det );
    uint_t      GetChildrenCount( void ) { return ( uint_t )child_nodes_.size(); }
    GspNode*    operator[]      ( uint_t i );
    
    void        AttachChildNode ( GspNode* child );
    void        DetachChildNode ( GspNode* child );

    //add new command
    //void  AttachCommand(const tchar_t* attr, const tchar_t* oper, const tchar_t* value);
    //del exist command
    //void  DetachCommand(const tchar_t* attr, const tchar_t* oper);
    
    const tchar_t* GetStyle    ( void );
    //get the value of attribute + operator
    const tchar_t* GetValue    ( const tchar_t* attr, const tchar_t* oper );
    
    void  WriteAnnotation      ( const tchar_t* descriptor );
    
    //write one command line in file, if command had exist, the value will be overwrite and return true,
    // else it will add a new command and return fale
    bool  WriteCommand         ( const tchar_t* name, const tchar_t* oper, const tchar_t* value );
    //erase one command line in file, if command had exist, the value will be erase and return true,
    // else it will return false
    bool  EraseCommand         ( const tchar_t* name, const tchar_t* oper );
    
    bool  DoPass               ( GspReadCallback* gcb );
    
    int   GetDepth             ( void );

protected:

    int   Save                 ( void* stream );

private:

    class Command
    {
    public:
        Command() {}
        ~Command() {}
        _tstring m_feature;
        _tstring m_operator;
        _tstring m_value;
    };

private:

    uint_t                    m_Tabsize;
    _tstring              m_Style;
    _tstring              m_Description;
    //_tstring m_Annotation;
    
    std::vector<_tstring> m_AnnotationLine;
    //find this key first
    uint_t  m_IndexKey;
    std::vector<Command>    m_FeatureLine;
    
    GspNode* m_Parent;
    std::vector<GspNode*>   child_nodes_;
};


///////////////////////////////////////////////////////////////////////////////////////
//
class CPGspParser : public GspNode//, private pfziplib::PFile
{

public:

    CPGspParser();
    ~CPGspParser();
    
    //read function
    int   Load          ( const tchar_t* filename );
    int   LoadFromString( const tchar_t* pString );
    int   Save          ( const tchar_t* filename );
    
    
    //void Open(const tchar_t* filename);
    //void CreateObject(const tchar_t* name);
    //void CloseObject(void);
    //void SetReadAttribute(const tchar_t* attribute, const tchar_t* value);
    //void Close(void);
    
private:

    struct GSPItem
    {
        tchar_t name[256];
        void* data;
    };
    
    struct GSPValue
    {
        _tstring string;
        double data;
    };
    
    pfstl::delist<GSPItem>  m_ObjectList;
    pfstl::delist<GSPValue> m_ValueList;
    int GetDepth  ( void ) { return m_ObjectList.size(); }
    
};

#endif

