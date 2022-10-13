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
//
//
//
#ifndef RESOURCEMGR_H
#define RESOURCEMGR_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <PfDef.h>

///////////////////////////////////////////////////////////////////////////////
//
class CMyCurrentDirectory
{
public:

	tchar_t* m_CurrentDirectory;

	CMyCurrentDirectory()
	{
		m_CurrentDirectory=NULL;
	}

	~CMyCurrentDirectory()
	{
		delete [] m_CurrentDirectory;
	}

	const tchar_t*  GetCurrentDirectory(void)
	{
		return m_CurrentDirectory;
	}

	void SetCurrentDirectory(const tchar_t* wdir)
	{
		if(m_CurrentDirectory != NULL)
		{
			delete [] m_CurrentDirectory;
		    m_CurrentDirectory = NULL;
		}
		m_CurrentDirectory = new tchar_t[strlen(wdir)+1];
		strcmp(m_CurrentDirectory, wdir);
	}

};


///////////////////////////////////////////////////////////////////////////////
//
class ResourceMgr
{
public:

    struct Record
    {
        tchar_t* Name;
        void*   pData;  
		unsigned int CloneCount;
    };

    std::vector<Record*> ResourceList;
    
    ResourceMgr(){}
    ~ResourceMgr()
    {
        for(unsigned int i=0; i<ResourceList.size(); i++)
        {
            delete [] ResourceList[i]->Name;
            delete ResourceList[i];
        }
		ResourceList.clear();
    }

	bool Emtpy(void)
	{
		return ResourceList.empty();
	}
   
    void* Create(const tchar_t* name)
    {
        for(unsigned int i=0; i<ResourceList.size(); i++)
        {
			assert(ResourceList[i]->Name != NULL);
            if(strcmp(name, ResourceList[i]->Name) == 0)
			{
				ResourceList[i]->CloneCount++;
                return ResourceList[i]->pData;
			}
        }
        return NULL;
    }

	unsigned int GetReferSum(const tchar_t* name)
	{
        for(unsigned int i=0; i<ResourceList.size(); i++)
        {
			assert(ResourceList[i]->Name != NULL);
            if(strcmp(name, ResourceList[i]->Name) == 0)
			{
                return ResourceList[i]->CloneCount;
			}
        }
        return 0;
	}

    void Insert(const tchar_t* name, void* data)
    {
        for(unsigned int i=0; i<ResourceList.size(); i++)
        {
			assert(ResourceList[i]->Name != NULL);
            if(strcmp(name, ResourceList[i]->Name) == 0)
                return;
        }
        Record* record = new Record;
        record->Name = new tchar_t[ strlen(name)+1 ];
        strcpy(record->Name, name);
        record->pData = data;
		record->CloneCount = 0;
        ResourceList.push_back(record);
    }
    
    void* Destroy(const tchar_t* name)
    {
        for(unsigned int i=0; i<ResourceList.size(); i++)
        {
			assert(ResourceList[i]->Name != NULL);
            if(strcmp(name, ResourceList[i]->Name) == 0)
            {
				//could be delete
				if(ResourceList[i]->CloneCount == 0)
				{
                    void* data = ResourceList[i]->pData;
					ResourceList[i]->pData=NULL;
                    delete [] ResourceList[i]->Name;
			        ResourceList[i]->Name=NULL;
                    delete ResourceList[i];
                    ResourceList.erase(ResourceList.begin()+i);
                    return data;
				}
				else
					ResourceList[i]->CloneCount--;
            }
        }
        return NULL;
    }
};

#endif//RESOURCEMGR_H
