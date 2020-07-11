#if !defined (_PEEKMESSAGE_H_)
#define _PEEKMESSAGE_H_

/***************
// Esta función es borrada y sustituida por la macro
void APPPeekMessage(void)
{
   MSG  msg;

   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
      if (!AfxGetApp()->PreTranslateMessage(&msg))
         {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
         }
      }
}
***************/
/*
#define APPPeekMessage()		PEEKMESSAGE()

#define PEEKMESSAGE() {MSG msg;\
                      while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))\
                       {                                    \
                        if (!AfxGetApp()->PreTranslateMessage(&msg))\
                            { \
                           TranslateMessage(&msg);          \
                           DispatchMessage(&msg);           \
                            }  \
                       }}                                   
*/                       
//#endif
#endif
