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

#include "DataStream.h"
#include "PFTimer.h"
#include <algorithm>
////#include <assert.h>


namespace GSO
{
    /*
    int CompareNeighbor(const void* i0, const void* i1)
    {
        int ii0 = (*(int*)i0);
        int ii1 = (*(int*)i1);
        return (ii0-ii1);
    }
    */

    PFTimer g_StreamLoadTimer;

    class CellRealm : public StreamCell
    {
    public:
        CellRealm(cellid_t id): StreamCell(id) {}
        ~CellRealm(){ Neighborhood.clear(); }
        std::vector<cellid_t> Neighborhood;
        DWORD          LoadTime;
        //std::vector<int> Neighborhood;
    };

    int CompareRelease(const void* i0, const void* i1)
    {
        CellRealm* ii0 = (CellRealm*)i0;
        CellRealm* ii1 = (CellRealm*)i1;
        return (ii0->LoadTime - ii1->LoadTime);
    }


    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    DataStream::DataStream()
    : m_FocusedCellId(Invalid_CellId)//, m_CellWidth(0), m_CellHeight(0), m_CellOffset(1, 1)
    {
        m_WaitLoadSeq.clear();
        m_WaitReleaseSeq.clear();
        m_pLoadedCellVec.clear();
		m_LoadTimer.SetCycle(50);
		m_ReleaseTimer.SetCycle(5000);
    }


    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    DataStream::~DataStream()
    {
        ReleaseCells();
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    uint_t DataStream::GetWaitLoadSeqSize()
    {
        return m_WaitLoadSeq.size();
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    uint_t DataStream::GetWaitReleaseSeqSize()
    {
        return m_WaitReleaseSeq.size();
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    void DataStream::ReleaseCells()
    {
        uint_t i=0;
        m_WaitLoadSeq.clear();
        for(i=0; i<m_pLoadedCellVec.size(); ++i)
        {
            OnReleaseCell(m_pLoadedCellVec[i]->Id,
                           m_pLoadedCellVec[i]->Descriptor, //m_pLoadedCellVec[i]->Name,
                           m_pLoadedCellVec[i]->pData,
                           m_pLoadedCellVec[i]->pUserData);
            m_pLoadedCellVec[i]->pData = m_pLoadedCellVec[i]->pUserData = 0;
            
        }
        m_pLoadedCellVec.clear();

        for(i=0; i<m_WaitReleaseSeq.size(); ++i)
        {
            OnReleaseCell(m_WaitReleaseSeq[i]->Id,
                           m_WaitReleaseSeq[i]->Descriptor, //m_pLoadedCellVec[i]->Name,
                           m_WaitReleaseSeq[i]->pData,
                           m_WaitReleaseSeq[i]->pUserData);
            m_WaitReleaseSeq[i]->pData = m_WaitReleaseSeq[i]->pUserData = 0;
        }

        m_WaitReleaseSeq.clear();

        std::map< cellid_t, StreamCell* >::iterator iter = m_RealmBuf.begin();
        while(iter != m_RealmBuf.end())
        {
            CellRealm* cell = (CellRealm*)iter->second;
            OnDestroyDescriptor(cell->Descriptor);
            SAFE_DELETE(cell);
            iter++;
        }
        m_RealmBuf.clear();

        m_FocusedCellId = -1;
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
//    void DataStream::Create(uint_t cx, uint_t cy, uint_t w, uint_t h)
//    {
//        //m_pCellBuf.allot(cx, cy);
//        m_CellWidth = w;
//        m_CellHeight = h;
//        return;
//    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    void DataStream::GetLoadedCellIDs(std::vector<int>& idList) const
    {
        uint_t iiii=m_pLoadedCellVec.size();
        for(uint_t i=0; i<m_pLoadedCellVec.size(); i++)
        {
             idList.push_back(m_pLoadedCellVec[i]->Id);
        }

        return;
        /*

        if(m_pLoadedCellVec.empty())
        {
            idList.clear();
            return;
        }
        std::vector< StreamCell* >::const_iterator iLoadedCell = m_pLoadedCellVec.begin();
        for(; iLoadedCell != m_pLoadedCellVec.end(); ++iLoadedCell)
        {
            idList.push_back((*iLoadedCell)->Id);
        }
        */
    }

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	size32_t DataStream::GetLoadedCellCount(void)
	{
		return m_pLoadedCellVec.size();
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void* DataStream::GetLoadedCellData(uint_t i)
	{
		return m_pLoadedCellVec[i]->pData;
	}


    /*
    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    uint_t DataStream::GetCols(void)
    {
        return m_pCellBuf.col_size();
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    uint_t DataStream::GetRows(void)
    {
        return m_pCellBuf.row_size();
    }
    */

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
//    uint_t DataStream::GetCellWidth (void)
//    {
//        return m_CellWidth;
//    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
//    uint_t DataStream::GetCellHeight(void)
//    {
//        return m_CellHeight;
//    }
    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    StreamCell* DataStream::GetStreamCell(cellid_t Id)
    {
        std::map< cellid_t, StreamCell* >::iterator iter = m_RealmBuf.find(Id);
        if(iter != m_RealmBuf.end())
        {
            return iter->second;
        }

        /*
        for(uint_t i=0; i<m_RealmBuf.size(); i++)
        {
            if(Id ==  m_RealmBuf[i]->Id)
                return m_RealmBuf[i];
        }
        */
        return NULL;

    }
/*
    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    const char* DataStream::GetCellName(int Id)
    {
        CellRealm* cell = GetStreamCell(Id);
        if(cell == NULL)
            return NULL;
        return cell->Name;
        //return m_pCellBuf(x+m_CellOffset.x, y+m_CellOffset.y).Name;
    }
*/
    StreamCellDescriptor DataStream::GetCellDescriptor(cellid_t Id)
    {
        StreamCell* cell = GetStreamCell(Id);
        if(cell == NULL)
            return NULL;
        return cell->Descriptor;
    }
    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    uint_t DataStream::GetCellCount(void)
    {
        return m_RealmBuf.size();
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    void*  DataStream::GetCellData(cellid_t Id)
    {
        CellRealm* cell = (CellRealm*)GetStreamCell(Id);
        if(cell == NULL)
            return NULL;
        return cell->pData;
        //return m_pCellBuf(x+m_CellOffset.x, y+m_CellOffset.y).pData;
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    void* DataStream::GetCellUserData(cellid_t Id)
    {
        StreamCell* cell = GetStreamCell(Id);
        if(cell == NULL)
            return NULL;
        return cell->pUserData;
        //return m_pCellBuf(x+m_CellOffset.x, y+m_CellOffset.y).pUserData;
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    uint_t DataStream::ForAllCell(CellIdCallBack cellcb, void* pData)
    {
        std::map< cellid_t, StreamCell* >::iterator iter = m_RealmBuf.begin();
        unsigned int i=0;
        while(iter != m_RealmBuf.end())
        {
            if(!cellcb(iter->second->Id, iter->second->Descriptor, i, pData))
                break;
            iter++;
            i++;
        }
        /*
        for(uint_t i=0; i<m_RealmBuf.size(); i++)
        {
            if(!cellcb(m_RealmBuf[i], i, pData))
                break;
        }
        */
        return m_RealmBuf.size();
    }
    
    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    void DataStream::ForAllNeighborhood(cellid_t Id, CellIdCallBack func, void* data)
    {
        if(func==NULL)
            return;
        std::map< cellid_t, StreamCell* >::iterator iter = m_RealmBuf.find(Id);
        if(iter != m_RealmBuf.end())
        {
            CellRealm* cell=(CellRealm*)iter->second;
            for(uint_t i=0; i<cell->Neighborhood.size(); i++)
            {
                CellRealm* cellr = (CellRealm*)GetStreamCell(cell->Neighborhood[i]);
                if(cellr != NULL)
                {
                    if(!(*func)(cellr->Id, cellr->Descriptor, i, data))
                        return;
                }
                
            }
        }
    }

    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    /*
    uint_t DataStream::ForAllLoadedCell(CellCallBack cellcb, void* pData)
    {
        for(uint_t i=0; i<m_pLoadedCellVec.size(); i++)
        {
            if(!cellcb(m_pLoadedCellVec[i], i, pData))
                break;
        }
        return m_pLoadedCellVec.size();
    }
    */

    //=============================================================================
    //  Name : 
    //  Desc :  LOAD 的優先權  小 <---- 大
    //          Release        小 <---- 大
    //=============================================================================
	bool SortByLoadTime(StreamCell* cell1, StreamCell* cell2)
	{
		if(((CellRealm*)cell1)->LoadTime < ((CellRealm*)cell1)->LoadTime)
		{
			return true;
		}
		return false;

	}


    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
	void DataStream::ForceUpdateCenter(cellid_t Id)
	{
        if(Id ==  Invalid_CellId)
        {
            m_FocusedCellId=Id;
            return;
        }

		m_pLoadedCellVec.clear();
		m_WaitLoadSeq.clear();
		//m_WaitReleaseSeq.clear();
		
        CellRealm* cell = (CellRealm*)GetStreamCell(Id);
        if(cell==NULL)
        {
            m_FocusedCellId=Invalid_CellId;
            return;
        }

        m_WaitLoadSeq.clear();
        m_FocusedCellId = Id;
        
        
        for(uint_t lc=0; lc<m_pLoadedCellVec.size(); lc++)
        {
        	m_WaitReleaseSeq.push_back(m_pLoadedCellVec[lc]);
        }
        
        m_pLoadedCellVec.clear();


        //先將neighborhood取出放在一個vector裡 做為準備要載入的
        //std::vector< CellRealm* > centerneighborhood;
        for(uint_t cn=0; cn<cell->Neighborhood.size(); cn++)
        {
            CellRealm* celln = (CellRealm*)GetStreamCell(cell->Neighborhood[cn]);
            if(celln!=NULL)
            {
				int jump=false;
        	    for(uint_t cr=0; cr<m_WaitReleaseSeq.size(); cr++)
        	    {
        		    if(m_WaitReleaseSeq[cr]->Id == celln->Id)
        		    { 
        		    	m_pLoadedCellVec.push_back(celln);
        			    m_WaitReleaseSeq.erase(m_WaitReleaseSeq.begin()+cr);
        			    jump=true;
						break;
        		    }
        	    }
				if(!jump)
                    m_WaitLoadSeq.push_back(celln);
            }

        }


		std::stable_sort(m_WaitLoadSeq.begin(),
		                  m_WaitLoadSeq.end(),
			              SortByLoadTime);


        //1. compute new stand center

		for(uint_t l=0; l<m_WaitLoadSeq.size(); l++)
		{
            //CellRealm* celln = (CellRealm*)GetStreamCell(m_WaitLoadSeq.back());
            CellRealm* celln = (CellRealm*)m_WaitLoadSeq[l];
                
            //find first if it not be release already
         
            DWORD btime = g_StreamLoadTimer.GetTickMSTime();
            OnLoadCell(celln->Id,
                        celln->Descriptor, //region->Name,
                        &celln->pData,
                        &celln->pUserData);
            DWORD atime = g_StreamLoadTimer.GetTickMSTime();
            celln->LoadTime = atime - btime;
			
            m_pLoadedCellVec.push_back(celln);
            m_WaitLoadSeq.erase(m_WaitLoadSeq.begin()+l);
		}

		for(uint_t r=0; r<m_WaitReleaseSeq.size(); r++)
		{
            //CellRealm* celln = (CellRealm*)GetStreamCell(m_WaitReleaseSeq.back());
            CellRealm* celln = (CellRealm*)m_WaitReleaseSeq[r];

            OnReleaseCell(celln->Id,
                           celln->Descriptor,//m_pLoadedCellVec[i]->Name,
                           celln->pData,
                           celln->pUserData);

            celln->pData = celln->pUserData = 0;
            m_WaitReleaseSeq.erase(m_WaitReleaseSeq.begin()+r);
        }
        
        
	}

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void DataStream::UpdateAllCell(void)
	{
		std::map< cellid_t, StreamCell* >::iterator iter = m_RealmBuf.begin();
		while(iter!=m_RealmBuf.end())
		{
			StreamCell* celln = iter->second;
			OnLoadCell(celln->Id,
				        celln->Descriptor, //region->Name,
				        &celln->pData,
				        &celln->pUserData);
			m_pLoadedCellVec.push_back(iter->second);
			iter++;
		}
	}


    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    void DataStream::UpdateCenter(cellid_t Id)
    {
        if(Id ==  Invalid_CellId)
        {
            m_FocusedCellId=Id;
            return;
        }
        //1. compute new stand center
        if(m_FocusedCellId == Id)
        {
            if(!m_WaitLoadSeq.empty())
            {
                //CellRealm* celln = (CellRealm*)GetStreamCell(m_WaitLoadSeq.back());
                CellRealm* celln = (CellRealm*)m_WaitLoadSeq.back();
                
                //find first if it not be release already
                for(uint_t i=m_WaitReleaseSeq.size(); i>0; i--)
                {
                    uint_t index = i-1;
                    if(m_WaitReleaseSeq[i-1]->Id==celln->Id)
                    {
                        m_pLoadedCellVec.push_back(m_WaitReleaseSeq[index]);
                        m_WaitReleaseSeq.erase(m_WaitReleaseSeq.begin()+index);//find one
                        m_WaitLoadSeq.erase(m_WaitLoadSeq.begin()+index);//last one
						m_LoadTimer.Reset();
                        return;
                    }
                }

				if(m_LoadTimer.Interval() < TIMER_INTERRUPT)
					return;
                
                DWORD btime = g_StreamLoadTimer.GetTickMSTime();
                OnLoadCell(celln->Id,
                            celln->Descriptor, //region->Name,
                            &celln->pData,
                            &celln->pUserData);
                DWORD atime = g_StreamLoadTimer.GetTickMSTime();
                celln->LoadTime = atime - btime;
#ifdef _DEBUG
                char buff[255];
                sprintf(buff, "Load time: %d Size %d\n", celln->LoadTime, m_WaitLoadSeq.size());
                OutputDebugStringA(buff);
#endif
                m_pLoadedCellVec.push_back(celln);
                m_WaitLoadSeq.erase(m_WaitLoadSeq.end()-1);
				m_LoadTimer.Reset();
                return;
            }
            

            if(!m_WaitReleaseSeq.empty())
            {
                //CellRealm* celln = (CellRealm*)GetStreamCell(m_WaitReleaseSeq.back());
				if(m_ReleaseTimer.Interval() < TIMER_INTERRUPT)
					return;

                CellRealm* celln = (CellRealm*)m_WaitReleaseSeq.back();

                DWORD btime = g_StreamLoadTimer.GetTickMSTime();
                OnReleaseCell(celln->Id,
                               celln->Descriptor,//m_pLoadedCellVec[i]->Name,
                               celln->pData,
                               celln->pUserData);
                DWORD atime = g_StreamLoadTimer.GetTickMSTime();
                DWORD ReleaseTime = atime - btime;

                celln->pData = celln->pUserData = 0;
                m_WaitReleaseSeq.erase(m_WaitReleaseSeq.end()-1);
				m_ReleaseTimer.Reset();

#ifdef _DEBUG
                tchar_t buff[255];
                sprintf(buff, "Release time: %d Size %d\n", ReleaseTime, m_WaitReleaseSeq.size());
                OutputDebugStringA(buff);
#endif
                return;
            }
            return;
        }

        
#ifdef _DEBUG
      DWORD computertime=g_StreamLoadTimer.GetTickMSTime();
#endif
        CellRealm* cell = (CellRealm*)GetStreamCell(Id);
        if(cell==NULL)
        {
            m_FocusedCellId=Invalid_CellId;
            return;
        }

        m_WaitLoadSeq.clear();
        m_FocusedCellId = Id;
        
        
        for(uint_t lc=0; lc<m_pLoadedCellVec.size(); lc++)
        {
        	m_WaitReleaseSeq.push_back(m_pLoadedCellVec[lc]);
        }
        
        m_pLoadedCellVec.clear();


        //先將neighborhood取出放在一個vector裡 做為準備要載入的
        //std::vector< CellRealm* > centerneighborhood;
        for(uint_t cn=0; cn<cell->Neighborhood.size(); cn++)
        {
            CellRealm* celln = (CellRealm*)GetStreamCell(cell->Neighborhood[cn]);
            if(celln!=NULL)
            {
				int jump=false;
        	    for(uint_t cr=0; cr<m_WaitReleaseSeq.size(); cr++)
        	    {
        		    if(m_WaitReleaseSeq[cr]->Id == celln->Id)
        		    { 
        		    	m_pLoadedCellVec.push_back(celln);
        			    m_WaitReleaseSeq.erase(m_WaitReleaseSeq.begin()+cr);
        			    jump=true;
						break;
        		    }
        	    }
				if(!jump)
                    m_WaitLoadSeq.push_back(celln);
            }

        }


		std::stable_sort(m_WaitLoadSeq.begin(),
		                  m_WaitLoadSeq.end(),
			              SortByLoadTime);


#ifdef _DEBUG
        tchar_t buff[255];
        sprintf(buff, "Compute time: %d\n",  g_StreamLoadTimer.GetTickMSTime()-computertime);
        OutputDebugStringA(buff);
#endif

        
        /*
        //bool* loaded=new bool[centerneighborhood.size()];
        //memset(loaded, false, (centerneighborhood.size())*sizeof(bool));
        
        for(uint_t i=0; i<m_pLoadedCellVec.size(); i++)
        {
            bool find=false;
            uint_t j=0;
            for(j=0; j< m_WaitLoadSeq.size(); j++)
            {
                //CellRealm* cell = (CellRealm*)GetStreamCell(Id)//啥還要再抓一次 我也不知道位啥                 
                //have be load，not need to load again
                if(m_WaitLoadSeq[j]->Id == m_pLoadedCellVec[i]->Id)
                {
                    //cell->Neighborhood.erase(cell->Neighborhood.begin()+j);
                    m_WaitLoadSeq.erase(m_WaitLoadSeq.begin()+j);
                    //loaded[j]=true;
                    find=true;
                    break;
                }
            }

            //this cell will be release
            //因為沒有找到
            if(!find)
            {
                //m_WaitReleaseSeq.push_back(m_pLoadedCellVec[i]);
                
                m_WaitReleaseSeq.push_back(m_pLoadedCellVec[i]);
                m_pLoadedCellVec.erase(m_pLoadedCellVec.begin()+i);
                i--;
            }
        }
        */

        //qsort(&m_WaitReleaseSeq[0], m_WaitReleaseSeq.size(), sizeof(StreamCell*), CompareRelease);
        
        
                /*
        for(uint_t j=0; j< centerneighborhood.size(); j++)
        {
            //want to load
            if(loaded[j]==false)
            {
                ////m_WaitLoadSeq.push_back(centerneighborhood[j]);

                OnLoadCell(region->Id,
                            region->Descriptor, //region->Name,
                            &region->pData,
                            &region->pUserData);
                m_pLoadedCellVec.push_back(region);

            }
        }

        delete [] loaded;
                */
        return;
    }


    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
//    bool DataStream::InsertCell(Pcstr filename, int Id)
    bool DataStream::InsertCell(StreamCellDescriptor CellDescriptor,  cellid_t Id)
    {
        if(Id < 0)
        {
            return false;
        }

        std::map< cellid_t, StreamCell* >::iterator iter = m_RealmBuf.find(Id);
        if(iter != m_RealmBuf.end())
        {
            //had be loaded
            //m_RealmBuf[i]->Name = filename;
            //CellRealm* getcell = iter->second;
            return false;
            /*
            //正在被載入中
            if(iter->second->pData)
            {
                OnReleaseCell(getcell->Id,
                               getcell->Descriptor, //m_RealmBuf[i]->Name,
                               getcell->pData,
                               getcell->pUserData);
                getcell->pData = getcell->pUserData=0;
                OnDestroyDescriptor(getcell->Descriptor);
                getcell->Descriptor = CellDescriptor;
                OnLoadCell(getcell->Id,
                            getcell->Descriptor, //m_RealmBuf[i]->Name,
                            &getcell->pData,
                            &getcell->pUserData);
            }
            */
        }
        
        /*
        CellRealm* cell = GetStreamCell(Id);
        if(cell)
        {
            OnReleaseCell(cell->Id,
                           cell->Descriptor,//m_pLoadedCellVec[i]->Name,
                           cell->pData,
                           cell->pUserData);
            cell->pData = cell->pUserData=0;
            OnDestroyDescriptor(CellDescriptor);
            cell->Neighborhood.clear();

            cell->Descriptor = CellDescriptor;
        }
        else
        */
        //{
        CellRealm* cell = new CellRealm(Id);

        cell->Descriptor = CellDescriptor;
    //        cell->Name = filename;
        m_RealmBuf.insert(std::map< cellid_t, CellRealm* >::value_type(Id, cell)); 

        

        /*
        
        for(uint_t i=0; i<m_RealmBuf.size(); i++)
        {
            //had be loaded
            if(m_RealmBuf[i]->Id == Id)
            {
//                m_RealmBuf[i]->Name = filename;
                if(m_RealmBuf[i]->pData)
                {
                    OnReleaseCell(m_RealmBuf[i]->Id,
                                   m_RealmBuf[i]->m_pDescriptor, //m_RealmBuf[i]->Name,
                                   m_RealmBuf[i]->pData,
                                   m_RealmBuf[i]->pUserData);
                    m_RealmBuf[i]->pData = m_RealmBuf[i]->pUserData = 0;
                
                    m_RealmBuf[i]->m_pDescriptor = pCellDescriptor;

                    OnLoadCell(m_RealmBuf[i]->Id,
                                m_RealmBuf[i]->m_pDescriptor, //m_RealmBuf[i]->Name,
                                &m_RealmBuf[i]->pData,
                                &m_RealmBuf[i]->pUserData);
                }
                else
                {
                    m_RealmBuf[i]->m_pDescriptor = pCellDescriptor;
                }
            }
        }
        

        CellRealm* cell = GetStreamCell(Id);
        if(cell)
        {
            cell->m_pDescriptor = pCellDescriptor;
            cell->Neighborhood.clear();
        }
        else
        {
    //        CellRealm* cell = new CellRealm;
            cell = new CellRealm(pCellDescriptor);
            cell->Id = Id;
    //        cell->Name = filename;
            m_RealmBuf.push_back(cell);
        }
        */
        
        //std::vector<int> neighborhood;
        OnGetNeighbor(Id, cell->Neighborhood);
        
        //不做排序讓USER 做優先權  放在LIST 最後面的 將優先載入 也優先釋放                
        //qsort(&cell->Neighborhood[0], cell->Neighborhood.size(), sizeof(int), CompareNeighbor);
        /*
        for(unsigned int c=0; c<neighborhood.size(); c++)
        {
            CellRealm* ncell = GetStreamCell(neighborhood[c]);
            if(!ncell)
            {
                ncell = new CellRealm(neighborhood[c]);
                m_RealmBuf.insert(std::map< cellid_t, CellRealm* >::value_type(ncell->Id, ncell)); 
            }
            cell->Neighborhood.push_back(ncell);
        }
        */
        cell->Neighborhood.push_back(Id);
        
        // Insert neighborhood
        //if(m_RealmBuf[i]->Neighborhood.empty())
        //{

        //}
        return true;
    }


    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
    bool DataStream::RemoveCell(cellid_t Id)
    {
        std::map< cellid_t, StreamCell* >::iterator iter = m_RealmBuf.find(Id);
        if(iter != m_RealmBuf.end())
        {
            CellRealm* cell = (CellRealm*)iter->second;
            if((cell->pData != NULL) ||
                (cell->pUserData != NULL))
            {
                OnReleaseCell(cell->Id,
                               cell->Descriptor, //cell->Name,
                               cell->pData,
                               cell->pUserData);
                cell->pData = cell->pUserData = 0;
                OnDestroyDescriptor(cell->Descriptor);
                
            }

            cell->Neighborhood.clear();
            delete cell;
            m_RealmBuf.erase(iter);
            return true;
        }

        /*
        for(uint_t i=0; i<m_RealmBuf.size(); i++)
        {
            if(m_RealmBuf[i]->Id == Id)
            {
                CellRealm* cell = m_RealmBuf[i];
                if((cell->pData != NULL) ||
                    (cell->pUserData != NULL))
                {
                    OnReleaseCell(cell->Id,
                                   cell->m_pDescriptor, //cell->Name,
                                   cell->pData,
                                   cell->pUserData);
                    cell->pData = cell->pUserData = 0;
                }

                cell->Neighborhood.clear();
                delete cell;
                m_RealmBuf.erase(m_RealmBuf.begin()+i);
            }
        }
        */
        return false;
    }

	void DataStream::SetLoadTime(mstimen_t t)
	{
		m_LoadTimer.SetCycle(t);
	}


	void DataStream::SetReleaseTime(mstimen_t t)
	{
		m_ReleaseTimer.SetCycle(t);
	}

	bool DataStream::IsLoadingFinish(void)
	{
		return m_WaitLoadSeq.empty();
	}

    

};