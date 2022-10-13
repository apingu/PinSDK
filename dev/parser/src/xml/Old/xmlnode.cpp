//////////////////////////////////////////////////////////
// File Name: xmlnode.cpp
//
// Author : Little Chio
// Created : 2002/7/15
// Last Modified : 2003/6/29
//
// Copyright (c) 2002-03, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#include "xmlnode.h"
#include <stdio.h>

#include <Windows.h>


int t1=0;
int t2=0;
int t3=0;
int t4=0;
//int t5=0;
//int t6=0;
//int t7=0;
int t8=0;
//int t9=0;


int a0=0;
int a1=0;
int a2=0;
int a3=0;
int a4=0;
int a5=0;

//===========================
//  class XMLNode
//===========================

bool XMLNode::ReadAttribute(basestring name)
{
	char* ptempname = new char[name.size()+1];
	strcpy(ptempname, name.c_str());

	if(name[name.size()-1]==_T('/'))
	{
		ptempname[name.size()-1]=0;
	}

	char* p=strtok(ptempname, " =\"\r\n");
	if(p!=NULL)
	{
		m_Name=ptempname;
	}

	while(p=strtok(NULL, " ="))
	{
		XMLAttribute attri;
		if(p) {attri.m_Name=p;}
		// get attribue value
		p=strtok(NULL, "=\"");
		if(p) {attri.m_Value=p;}
		m_Attriutes.push_back(attri);
	}
	delete [] ptempname;

	return true;
}

// Read XML's tag from stream. 'isbegin' indicates if the tag is begin tag or end tag
bool XMLNode::ReadTag(InStream &ins, basestring &name, bool &isbegin, bool &isEnd)
{
	longlong ll1 = GetTickCount();
	TCHAR buffer[512];
	basestring tag_name;
	isEnd = false;

	longlong al0 = GetTickCount();
    do
    {
		ins.clear();
        ins.getline(buffer, sizeof(buffer), _T('<'));  // try to get tag's mark '<'
    }while(ins.fail() && !ins.eof());
	a0+=GetTickCount()-al0;
  

	longlong al1 = GetTickCount();
    do
	{
		buffer[0] = 0;
		ins.clear();
	    ins.getline(buffer, sizeof(buffer), _T('>'));     // try to get tag's mark '>'
		tag_name += buffer;
    }while(ins.fail() && !ins.eof());
	a1+=GetTickCount()-al1;

	int taglen = tag_name.size();

    if(ins.good() && taglen > 0)
    {
		if(taglen>4 && memcmp(tag_name.c_str(), _T("?xml"), 4)==0)    // check if it's XML header
        {
			t1+=GetTickCount()-ll1;
			return ReadTag(ins, name, isbegin, isEnd);     // skip the header and read the next node
        }
        else if(taglen>4 && memcmp(tag_name.c_str(), _T("!--"),3)==0)  // check if it's comment
        {
			longlong al2 = GetTickCount();
			while(tag_name[taglen-2]!='-' || tag_name[taglen-1]!='-')
			{
				do
				{
					buffer[0] = 0;
				    ins.clear();
				    ins.getline(buffer, sizeof(buffer), _T('>'));     // try to get tag's mark '>'
				    tag_name += buffer;
				}
				while(ins.fail() && !ins.eof());
				taglen = tag_name.size();
			}
			m_Comment.append(tag_name, 3, taglen-5);
			a2+=GetTickCount()-al2;
			t1+=GetTickCount()-ll1;			
			return ReadTag(ins, name, isbegin, isEnd); // read the next node
		}
        else if(tag_name[0]==_T('/'))        // check if it's node's end
        {
            name = tag_name.c_str()+1;
            isbegin = false;
        }
        else if(tag_name[taglen-1]==_T('/'))   // check if it's node's immediate end
        {
			longlong al3 = GetTickCount();
			char* endbuffer = new char[tag_name.size()+1];
			strcpy(endbuffer, tag_name.c_str());
			endbuffer[taglen-1] = 0;
			name = endbuffer;
			delete [] endbuffer;
			isbegin = true;
			isEnd = true;
			a3+=GetTickCount()-al3;
			//return ReadTag(ins, name, isbegin); // skip this node and read the next one
        }
        else                      // otherwise, it should be the node's begin
        {
			name = tag_name;
            isbegin = true;
        }
		t1+=GetTickCount()-ll1;
        return true;
	}
	t1+=GetTickCount()-ll1;
    return false;
}



//#include <windows.h>
// Read the node from stream                
bool XMLNode::LoadFromStream(InStream &ins)
{
    //char buff[512];
    //long long bt = 0;
    //long long et = 0;

	//longlong ll1 = GetTickCount();
	

    long long bn = 0;
    long long en = 0;

    //bn = GetTickCount();
    //sprintf(buff, "beginN %lld\n", bn);
    //OutputDebugString(buff);


	m_Name = m_Text = m_Comment = _T("");
    m_ChildNodes.clear();      // clear all members first
	m_Attriutes.clear();

    basestring name;
    bool begin_tag;
	bool end_tag;

    int prev_pos = ins.tellg();     // push the current stream's position

    //bt = GetTickCount();
    //sprintf(buff, "begin0 %lld\n", bt);
    //OutputDebugString(buff);
	//t1 += GetTickCount()-ll1;

	//ll1 = GetTickCount();
    if(ReadTag(ins, name, begin_tag, end_tag) && begin_tag)   // read tag's begin first
    {
        //et = GetTickCount();
        //sprintf(buff, "End0 %lld  => %lld\n", et, et-bt);
        //OutputDebugString(buff);
		int ll1 = GetTickCount();
		ReadAttribute(name);
		t3 += GetTickCount()-ll1;

		// single tag and immediate end
		if(end_tag)
			return true;

		XMLNode node;
		while(node.LoadFromStream(ins))     // try to retrive the child nodes
		{
			int ll1 = GetTickCount();
			m_ChildNodes.push_back(node);
			int sss = m_ChildNodes.size();
			t4 += GetTickCount()-ll1;
		}

		//ll1 = GetTickCount();

		int data_start = ins.tellg();

		//std::string endtag = '/'+m_Name;
		// find ending tag
		//bt = GetTickCount();
		//sprintf(buff, "begin3 %lld\n", bt);
		//OutputDebugString(buff);

		if(ReadTag(ins, name, begin_tag, end_tag) && name==m_Name && !begin_tag)
		{
			//t5 += GetTickCount()-ll1;

			if(m_ChildNodes.size()==0) // end node, so we should catch the node's text
			{
				//ll1 = GetTickCount();
				int cur_pos = ins.tellg();
				//int data_len = cur_pos - data_start - name.size() - 3;
				int data_len = cur_pos - data_start - m_Name.size() - 3; // '/'
				TCHAR* text = new TCHAR[data_len+1];

				ins.seekg(data_start);
				ins.read(text, data_len);     // read the node's text
				ins.seekg(cur_pos);

				text[data_len] = 0;
				m_Text = Decode(text);      // decode the text
				delete [] text;
				//t6 += GetTickCount()-ll1;
			}
		}

		//t4 += GetTickCount() - ll1;
		//t8 += GetTickCount() - ll2;
		return true;
        //et = GetTickCount();
        //sprintf(buff, "End3 %lld  => %lld\n", et, et-bt);
        //OutputDebugString(buff);
	}
        
	//ll1 = GetTickCount();
    ins.seekg(prev_pos);
	//t7 += GetTickCount()-ll1;

    //en = GetTickCount();
    //sprintf(buff, "EndN %lld  => %lld\n", en, en-bn);
    //OutputDebugString(buff);
    return false;
}

// Write the node to stream 
void XMLNode::SaveToStream(OutStream &outs, int indent) const
{
    // prepare the indent string  
	//TCHAR* indent_str = new TCHAR[indent+1];

    //for(int i=0; i< indent; i++) indent_str[i] = _T(' ');
    //indent_str[indent] = 0;

    // write out the comment first
    if(m_Comment.size()!=0)
    {
        std::string indstr;
        indstr = _T("<!--") + m_Comment + _T("-->\r\n");
        outs << indstr.c_str();
        //outs << indent_str << _T("<!--") + m_Comment + _T("-->\r\n");
    }

	basestring nodename;
	nodename = m_Name;
	for(unsigned int i=0; i<m_Attriutes.size(); i++)
	{
		// NodeA name="AAA"
		nodename = nodename +  _T(" ") + m_Attriutes[i].m_Name + _T("=") +  _T("\"") + m_Attriutes[i].m_Value + _T("\"");
	}

    if(m_ChildNodes.size()!=0)     // it's the way we output the node with child nodes
    {
    	std::string indstr;
    	indstr = _T("<") + nodename + _T(">\r\n");
    	outs << indstr.c_str();
        //outs << indent_str << _T("<") + nodename + _T(">\r\n");
        
        for(unsigned int i=0; i< m_ChildNodes.size(); i++)
            m_ChildNodes[i].SaveToStream(outs, indent + 2);

        indstr = _T("</") + m_Name + _T(">\r\n");
        outs << indstr.c_str();
        //outs << indent_str << _T("</") + m_Name + _T(">\r\n");
    }
    else 
	{
    	std::string indstr;
    	indstr = _T("<") + nodename + _T(">") + Encode(m_Text) + _T("</") + m_Name + _T(">\r\n");
    	outs << indstr.c_str();
		//outs << indent_str << _T("<") + nodename + _T(">") + Encode(m_Text) + _T("</") + m_Name + _T(">\r\n");
	}
           
    //delete [] indent_str;
}

// Decode the XML-style string into plain text
basestring XMLNode::Decode(const basestring &str) const
{
	unsigned int pos = 0;
    basestring text = str;
		 
    while((pos=text.find(_T("&lt;"), pos))!=text.npos)
        text.replace(pos, 4, _T("<"));

    pos = 0;
	while((pos=text.find(_T("&gt;"), pos))!=text.npos)
          text.replace(pos, 4, _T(">"));

	pos = 0;
	while((pos=text.find(_T("&amp;"), pos))!=text.npos)
        text.replace(pos, 5, _T("&"));

    return text;
}

// Encode the plain text into the XML-style string
basestring XMLNode::Encode(const basestring &str) const
{
    unsigned int pos = 0;
    basestring text = str;

    while((pos=text.find(_T("&"), pos))!=text.npos)
	{
		text.replace(pos, 1, _T("&amp;"));
		pos+=4;
	}
	  
    pos = 0;
    while((pos=text.find(_T("<"), pos))!=text.npos)
	text.replace(pos, 1, _T("&lt;"));

    pos = 0;
    while((pos=text.find(_T(">"), pos))!=text.npos)
	    text.replace(pos, 1, _T("&gt;"));

    return text;
}

// Load XML from file
bool XMLNode::LoadFromFile(const char *filename)
{
	longlong ll0 = GetTickCount();

	FILE *fp = fopen(filename, "rb");

    if(fp)
    {
		fseek(fp, 0, SEEK_END);
        int size = ftell(fp);
        TCHAR* buf = new TCHAR[size+1];

        fseek(fp, 0, SEEK_SET);
        fread(buf, 1, size, fp);
		buf[size] = 0;
        fclose(fp);

		InStrStream iss(buf);
	    LoadFromStream(iss);

        delete [] buf;
		t8 += GetTickCount() - ll0;
        return true;
    }
	t8 += GetTickCount() - ll0;
    return false;
}

// Save XML to file
bool XMLNode::SaveToFile(const char *filename) const
{
    FILE *fp = fopen(filename, "wb");

    if(fp)
    {
		OutStrStream outs;
             
        // write out the header first
        outs << _T("<?xml version='1.0'?>\r\n"); 
              
        SaveToStream(outs);

#ifdef _UNICODE
		// write out the header of unicoded file
		int header = 0xfeff;
		fwrite(&header, 1, 2, fp);
#endif

        fwrite(outs.str().c_str(), sizeof(TCHAR), outs.tellp(), fp);
        fclose(fp);
        return true;
    }
        
    return false;	
}

// Reference the child nodes by name
XMLNode& XMLNode::operator[](const basestring &name)
{
    // find the node by name

    for(unsigned int i=0; i< m_ChildNodes.size(); i++)
    {
		if(m_ChildNodes[i].m_Name==name)
            return m_ChildNodes[i];
    }
       
    // we should create a new node if no match node

    XMLNode node;
    node.m_Name = name;
    m_ChildNodes.push_back(node);
    return m_ChildNodes.back();
}


// Find child node by name. Return 0 if no match node found.
XMLNode* XMLNode::FindNode(const basestring &name)
{
	m_LastNodeIndex = -1;   // reset the index

    for(unsigned int i=0; i< m_ChildNodes.size(); i++)
    {
		if(m_ChildNodes[i].m_Name==name)
		{
			m_LastNodeIndex = i;
			return &m_ChildNodes[i];
		}
    }

	return FindFirstNode(name.c_str());
}

// Find child node by index. Return 0 if no match node found.
XMLNode* XMLNode::FindNode(int idx)
{
	m_LastNodeIndex = -1;   // reset the index
	if(idx<=m_ChildNodes.size())
		return NULL;

	m_LastNodeIndex = idx;
	return &m_ChildNodes[idx];
}

XMLNode* XMLNode::FindFirstNode(const char *name)
{
	m_LastNodeIndex = -1;   // reset the index
	if(name)
	{
		for(unsigned int i=0; i< m_ChildNodes.size(); i++)
		{
			//if(m_ChildNodes[i].m_Name==name)
			if(strcmp(m_ChildNodes[i].m_Name.c_str(), name)==0)
			{
				m_LastNodeIndex = i;
				return &m_ChildNodes[i];
			}
		}
	}
	else
	{
		if(!m_ChildNodes.empty())
		{
			m_LastNodeIndex=0;
			return &m_ChildNodes[m_LastNodeIndex];
		}
	}
	return NULL;
}


// Find next child node if there're more child nodes with the same name
XMLNode* XMLNode::FindNextNode(const char *name)
{
	if(m_LastNodeIndex>=0)
	{
		if(name)
		{
			// find the node by name
			for(unsigned int i=m_LastNodeIndex+1; i< m_ChildNodes.size(); i++)
			{
				if(strcmp(m_ChildNodes[i].m_Name.c_str(), name)==0)//==m_ChildNodes[m_LastNodeIndex].m_Name)
				{
					m_LastNodeIndex = i;
					return &m_ChildNodes[i];
				}
			}
			//m_LastNodeIndex = -1;
		}
		else
		{
			if((m_LastNodeIndex+1)<(int)m_ChildNodes.size())
			{
				m_LastNodeIndex++;
				return &m_ChildNodes[m_LastNodeIndex];
			}
		}
	}

	return 0;
}


XMLAttribute* XMLNode::FindAttribute(const basestring &name)
{
    return FindFirstAttribute(name.c_str());
}


XMLAttribute* XMLNode::FindFirstAttribute(const char *name)
{
	m_LastAttrbuteIndex = -1;   // reset the index
	if(name)
	{
		for(unsigned int i=0; i< m_Attriutes.size(); i++)
		{
			if(strcmp(m_Attriutes[i].m_Name.c_str(), name)==0)
			{
				m_LastAttrbuteIndex=i;
				return &m_Attriutes[i];
			}
		}
	}
	else
	{
		if(!m_ChildNodes.empty())
		{
			m_LastAttrbuteIndex=0;
			return &m_Attriutes[m_LastAttrbuteIndex];
		}
	}

	return NULL;

}


XMLAttribute* XMLNode::FindNextAttribute(const char *name)
{
	if(m_LastAttrbuteIndex>=0)
	{
		if(name)
		{
			// find the node by name
			for(unsigned int i=m_LastAttrbuteIndex+1; i<(int)m_Attriutes.size(); i++)
			{
				if(strcmp(m_Attriutes[i].m_Name.c_str(), name)==0)//==m_ChildNodes[m_LastNodeIndex].m_Name)
				{
					m_LastAttrbuteIndex = i;
					return &m_Attriutes[i];
				}
			}
			//m_LastNodeIndex = -1;
		}
		else
		{
			if((m_LastAttrbuteIndex+1)<(int)m_Attriutes.size())
			{
				m_LastAttrbuteIndex++;
				return &m_Attriutes[m_LastAttrbuteIndex];
			}
		}
	}

	return 0;
}