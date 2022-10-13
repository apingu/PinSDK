#ifndef _MSGBOX2D_H
#define _MSGBOX2D_H


#include "Dialog2d.h"
#include <string>

namespace GSO
{
    namespace UI
    {
        enum UIMSG_TYPEID
        {
            UIMB_OKCANCEL  = 0,
            UIMB_OK        = 1,
            UIMB_CANCEL    = 2,
        };

        class MsgBoxReply
        {
        public :

            MsgBoxReply(){}
            ~MsgBoxReply(){}

            virtual void OnReply(int t) = 0;
        };


        class MsgBox2d
        {
            static  GSO::UI::Dialog2d                    s_Dialog;
            static  GSO::UI::Static2d*                   s_pContent;
            static  const MsgBoxReply*                   s_pRelpy;
            static  std::map<UIMSG_TYPEID, std::string>  s_typelist;
            static  longlong_t                             s_ContentId;

        public:

            MsgBox2d();
            ~MsgBox2d();

            static const GSO::UI::Dialog2d*  Create(GSO::UI::IWidget2d* pParent);
            static void                      Destroy(void);

            static bool                      AddType(UIMSG_TYPEID type, const tchar_t* filename);
            static void                      DelType(UIMSG_TYPEID type);
            static void                      SetTextContentId(longlong_t id);
            static longlong_t                  GetTextContentId(void);


            // 0: 只有確定
            // 1: 確定取消
            static bool                      Show(const tchar_t* text, GSO::UI::UIMSG_TYPEID type=UIMB_OK, const GSO::UI::MsgBoxReply* func=NULL);
            static void                      Close(void);
        };


    };
};




#endif


