    void Server::OnReceive(Net::TCPSocket* s)
    {
        OutputDebugString("http begin\n");

        unsigned long nread=0;
        int recvret = s->Ioctl(FIONREAD, (unsigned long *)&nread);
        char* buff=new char[nread];
        //char buff[1024];
        s->Recv(buff, nread);

        std::string httpdata;
        httpdata.append(buff, nread);

        HttpParser httpparser;
        int needrecv=httpparser.addBytes(buff, recvret);
        SAFE_DELETE_ARRAY(buff);       

     
        // 要確定資料流已經收完了        
        if(needrecv>0)
        {
            char* needbuff=new char[needrecv];
            s->Recvn(needbuff, needrecv);
            httpdata.append(needbuff, needrecv);

            httpparser.addBytes(needbuff, needrecv);
            SAFE_DELETE_ARRAY(needbuff);
        }

        BYTE* zipbuf=NULL;
        int   ziplen=0;

        HttpCodingType context_coding=HttpEncType_Normal;
        HttpCodingType accept_coding =HttpEncType_Normal;
        {
            const char* Context_Encoding = httpparser.getValue("content-encoding");
            if(Context_Encoding)
            {
                if(strcmp(Context_Encoding, "gzip")==0)
                {
                    context_coding = HttpEncType_GZip;
                }
            }

            const char* Accept_Encoding = httpparser.getValue("accept-encoding");
            if(Accept_Encoding)
            {
                if(strcmp(Accept_Encoding, "gzip")==0)
                {
                    accept_coding = HttpEncType_GZip;
                }
            }
        }



        std::string response_str="{\"ACK\":9}";
        {
            const char* uri = httpparser.getUri();
            const char* qur = httpparser.getQueryString();
            char* dbuf=url_decode((char*)uri);
            //printf("%s\n", dbuf);
            // 分析ㄈ

            {
                char logbuf[512];
                sprintf(logbuf, "%s\n", dbuf);
                OutputDebugString(logbuf);
            }


            if(m_pDelegate)
            {
                char* pdbuf=dbuf;
                char* actstr=strtok(pdbuf, "?");
                char* tmpbuf01=NULL;
                //BYTE* tmpbuf02=NULL;
                const char* comstr=NULL;
                int comlen=0;
                HttpEncType    content_type=HttpEncType_Application;

                {
                    const char* str = httpparser.getValue("content-type");
                    if(str)
                    {
                        if(strstr(str, "multipart/form-data")!=NULL)
                            content_type=HttpEncType_Multipart;
                    }
                }

                if(strcmp(httpparser.getMethod(), "GET")==0)
                {
                    comstr=strtok(NULL, "\0");
                    if(comstr)
                        comlen=strlen(comstr)+1;
                }
                else if(strcmp(httpparser.getMethod(), "POST")==0)
                {
                    const BYTE* httpbody = (BYTE*)httpparser.getBody();
                    int         httplen  = httpparser.getContentLength();

                    if(context_coding==HttpEncType_GZip)
                    {
                        int maxlen = IO::GZip::GetMaxUncompressedLen(httpbody, httplen);
                        zipbuf = (BYTE*)s_BufferStorage.Borrow(maxlen);// new BYTE[maxlen];
                        //zipbuf = new BYTE[maxlen];
                        ziplen = IO::GZip::UncompressBuffer(httpbody, httplen, zipbuf, maxlen);
                        httpbody = zipbuf;
                        httplen  = ziplen;
                        zipbuf[ziplen]=0;
                    }

                    if(content_type==HttpEncType_Application)
                    {
                        comstr=tmpbuf01=url_decode((const char*)httpbody);
                    }
                    else
                    {
                        comstr=(const char*)httpbody;
                    }
                    comlen=httplen;
                }

                if((comstr)&&(comlen>0))
                {
                    // command validate
                    // command SelectSphere
                    if(strcmp(actstr, "/selectsphere")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectSphere(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/login")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvLogin(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/logout")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvLogout(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/insertpiece")==0)
                    {
                        //if(content_type==HttpEncType_Multipart)
                        response_str = OnRecvInsertPiece(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/updatepiece")==0)
                    {
                        //if(content_type==HttpEncType_Multipart)
                        response_str = OnRecvUpdatePiece(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/deletepiece")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvDeletePiece(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/selectpiece")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectPiece(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/selectsheet")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectSheet(s, content_type, comstr, comlen);
                    }
                    // 評價
                    else if(strcmp(actstr, "/evaluatepiece")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvEvaluatePiece(s, content_type, comstr, comlen);
                    }
                    // 評價
                    else if(strcmp(actstr, "/selectmyevaluation")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectMyEvaluation(s, content_type, comstr, comlen);
                    }
                    // 評價
                    else if(strcmp(actstr, "/selectallevaluations")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectAllEvaluations(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/insertmember")==0)
                    {
                        //if(content_type==HttpEncType_Multipart)
                        response_str = OnRecvInsertMember(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/updatemember")==0)
                    {
                        //if(content_type==HttpEncType_Multipart)
                        response_str = OnRecvUpdateMember(s, content_type, comstr, comlen);
                    }
                    else if(strcmp(actstr, "/selectmember")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectMember(s, content_type, comstr, comlen);
                    }
                    // 加入朋友
                    else if(strcmp(actstr, "/insertfriend")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvInsertFriend(s, content_type, comstr, comlen);
                    }
                    // 允許成為朋友
                    else if(strcmp(actstr, "/permitfriend")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvPermitFriend(s, content_type, comstr, comlen);
                    }
                    // 封鎖朋友
                    else if(strcmp(actstr, "/forbidfriend")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvForbidFriend(s, content_type, comstr, comlen);
                    }
                    // 刪除朋友
                    else if(strcmp(actstr, "/deletefriend")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvDeleteFriend(s, content_type, comstr, comlen);
                    }
                    // 取得朋友列表
                    else if(strcmp(actstr, "/selectfriend")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectFriend(s, content_type, comstr, comlen);
                    }
                    // 取得朋友與我的關西
                    else if(strcmp(actstr, "/selectrelation")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectRelation(s, content_type, comstr, comlen);
                    }
                    // 發表近況
                    else if(strcmp(actstr, "/insertstatus")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvInsertStatus(s, content_type, comstr, comlen);
                    }
                    // 刪除近況
                    else if(strcmp(actstr, "/updatestatus")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvUpdateStatus(s, content_type, comstr, comlen);
                    }
                    // 刪除近況
                    else if(strcmp(actstr, "/deletestatus")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvDeleteStatus(s, content_type, comstr, comlen);
                    }
                    // 取得朋友近況
                    else if(strcmp(actstr, "/selectstatus")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectStatus(s, content_type, comstr, comlen);
                    }
                    // 取得朋友近況
                    else if(strcmp(actstr, "/selectrecent")==0)
                    {
                        //if(content_type==HttpEncType_Application)
                        response_str = OnRecvSelectRecent(s, content_type, comstr, comlen);
                    }
                    // 發表近況回應
                    else if(strcmp(actstr, "/insertresponse")==0)
                    {
                        //if(content_type==)
                        response_str = OnRecvInsertResponse(s, content_type, comstr, comlen);
                    }
                    // 更新近況回應
                    else if(strcmp(actstr, "/updateresponse")==0)
                    {
                        //if(content_type==)
                        response_str = OnRecvUpdateResponse(s, content_type, comstr, comlen);
                    }
                    // 取得近況回應
                    else if(strcmp(actstr, "/deleteresponse")==0)
                    {
                        //if(content_type==)
                        response_str = OnRecvDeleteResponse(s, content_type, comstr, comlen);
                    }
                    // 取得近況回應
                    else if(strcmp(actstr, "/selectresponse")==0)
                    {
                        //if(content_type==)
                        response_str = OnRecvSelectResponse(s, content_type, comstr, comlen);
                    }
                    // 加入密語
                    else if(strcmp(actstr, "/insertwhisperone")==0)
                    {
                        //if(content_type==)
                        response_str = OnRecvInsertWhisperOne(s, content_type, comstr, comlen);
                    }
                    // 刪除密語
                    else if(strcmp(actstr, "/deletetwhisperone")==0)
                    {
                        //if(content_type==)
                        response_str = OnRecvDeleteWhisperOne(s, content_type, comstr, comlen);
                    }
                    // 取得密語
                    else if(strcmp(actstr, "/selectwhisperone")==0)
                    {
                        //if(content_type==)
                        response_str = OnRecvSelectWhisperOne(s, content_type, comstr, comlen);
                    }
                    // 取得密語
                    else if(strcmp(actstr, "/selectwhisperset")==0)
                    {
                        //if(content_type==)
                        response_str = OnRecvSelectWhisperSet(s, content_type, comstr, comlen);
                    }

                }
                
                SAFE_DELETE_ARRAY(tmpbuf01);
                //if(zipbuf!=NULL)
                //{
                //    delete [] zipbuf;
                //    zipbuf=NULL;
                //    ziplen=0;
                //}
            }
            SAFE_DELETE_ARRAY(dbuf);            
        }
        

        HttpWriter httpwriter;
        if(response_str=="")
            response_str="{\"ACK\":9}";

        {
            char logbuf[512];
            sprintf(logbuf, "%s\n", response_str);
            OutputDebugString(logbuf);
        }
                
        {
            BYTE* sendbuf=(BYTE*)response_str.c_str();
            int   sendlen=response_str.size()+1;

            if(accept_coding == HttpEncType_GZip)
            {
                httpwriter.SetContentCoding("gzip");
                httpwriter.SetAcceptCoding("gzip");

                int compress_len = IO::GZip::GetMaxCompressedLen(sendlen);
                if(ziplen<compress_len)
                {
                    s_BufferStorage.Return((char*)zipbuf);
                    zipbuf=(BYTE*)s_BufferStorage.Borrow(compress_len);
                    ziplen=compress_len;
                }
                sendlen = IO::GZip::CompressBuffer(sendbuf, sendlen, zipbuf, ziplen);
                sendbuf = zipbuf;
            }

            httpwriter.SetContent((const char*)sendbuf, sendlen);
            size_t len=0;
            const char* response=httpwriter.GetBuffer(len);      
            int send_size = s->Send(response, len);
            if(send_size!=len)
            {
                OutputDebugString(response);
                OutputDebugString("\n");
            }

            s_BufferStorage.Return((char*)zipbuf);
                //SAFE_DELETE_ARRAY(compress_send_buf);
        }

        Kickout(s);
        OutputDebugString("http end");

    }