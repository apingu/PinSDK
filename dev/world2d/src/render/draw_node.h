//
//
//
//
//
//
//
//
//
//                       if use 3D mode define _VMC3D
//
//
//
//
#ifndef DRAWNODE_H
#define DRAWNODE_H

#include <graphic3d.h>
//#include <icon.h>
#include <vector>

#define _RENDER_FULLALPHA  255
class CPIcon;

namespace LGB  //game entity render
{
    // Forward Declarations
    class VMCRender2d;
    class VMCText;

    /**
     * \class DrawNode
     * \brief 這個類別用來建立Scene Graph。
     * 請將你自定的類別繼承自這個類別，並實作以下的虛擬函式內容。
     */
    class DrawNode
    {
        friend class VMCRender2d;
    public:

        typedef std::vector<DrawNode*>   rendercontainer_t;

        // RSAlphaZ 有畫Z
        enum  DrawMethod  { RSNormal = 0, RSAlpha, RSLignten, RSDarken, RSClipAlpha, RSScaleAlpha, RSUser = 25, RSMax = 50 };

        // kLayerBG -> kLayerSurface -> kLayerRole -> kLayerRole -> kLayerFX -> kLayerOnAir -> kLayerUI -> kLayerTop
        enum  DrawLayer   { kLayerRole, kLayerBG, kLayerBld, kLayerUI, kLayerSurface, kLayerFX, kLayerOnAir, kLayerTop, MAX_DrawLayerS };

        enum  DrawTypeID  { DTNormal = 0, NtBillBoard = 8 };

        /**
         * 建構子
         * 初始化所有的成員變數。
         */
        DrawNode(): RenderLayer(kLayerRole), RenderAlpha(_RENDER_FULLALPHA), RenderMethod(RSNormal), RenderType(DTNormal), m_Transparent(false)    {}

        /**
         * 解構子
         */
        virtual ~DrawNode()        {}


        /**
         * 將自已推入待繪製的佇列中。
         * @param container 待繪製佇列。
         */
        virtual void             OnRenderTo(rendercontainer_t &container)    { container.push_back(this); }

        /**
         * 當系統要繪製元件上的字的時候會呼叫的函式。由系統呼叫，非用戶呼叫。這是一個純虛擬函式，請記得實作。
         * @param font 用來繪製用的物件。
         */
        //virtual void             OnBlitText(LGB::VMCText *font) = 0;
        // 客製化去取得畫字的物件
        virtual   void*            OnGetTextNode() {return NULL;}

        /**
         * 取得用來比較的位置。
         * @return 用來比較的位置。
         */
        virtual LG::Point3         OnGetComparisonPos() = 0;

        /**
         *
         */
        virtual bool               OnIsOverlap(const LG::Point& pos) { return false; }

        /**
         * 取得物件方向的函式。
         * @return 方向。
         */
        virtual int                OnGetDir() const { return 0; }

        /**
         * 取得繪製時用到的圖型元件。由系統呼叫，非用戶呼叫。
         * @return 繪製用到的圖型，有可能為NULL。
         */
        virtual const CPIcon*      OnGetRenderImage(){ return NULL; }


        /**
         * 取得繪製的位置。由系統呼叫，非用戶呼叫。
         * @return 繪製的位置。
         */
        virtual LG::Point3         OnGetRenderPos()  { return LG::Point3(); }

#ifdef _VMC3D
        // 3D模式下 取得繪圖的位置
        virtual const LG::Axis3f & OnGetRenderAxis() { return LG::Axis3f(); }
#endif
        //virtual int           OnGetLayer(void){ return 0; }
        //virtual const CPIcon* OnGetCurIcon(void){ return NULL; }


        DrawLayer           RenderLayer;        ///< 決定該物件會於哪一層繪製。
        DrawMethod          RenderMethod;        ///< ****記得要改變這個 才會使用半透FUNC
        DrawTypeID          RenderType;         /// Bit 8: BillBoard


        ///< 物件以半透明多少的程度來繪製。
        uint_t              RenderAlpha;
        uint_t              RenderReserver;

        // 如果是 RSScaleAlpha 或是其他支援放大縮小的
        long                RenderSizeCx;
        long                RenderSizeCy;

        // 如果是 RSClipAlpha 部分貼圖的
        ushort_t            RenderLeft;
        ushort_t            RenderTop;
        ushort_t            RenderRight;
        ushort_t            RenderBottom;

        short               m_Transparent;        ///< 有沒有被遮蓋

    };
};

#endif
