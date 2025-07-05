#ifndef BJ_MAIN_IMPL_H
#define BJ_MAIN_IMPL_H

#ifndef BJ_MAIN_H
#   error "Incorrect header inclusion" 
#endif

#if !defined(BJ_MAIN_HANDLED) && !defined(BJ_MAIN_NOIMPL)
#   ifdef main
#       undef main
#   endif
#   ifdef BJ_MAIN_USE_CALLBACKS
#       define BJ_MAIN_CALLBACK_STANDARD 1
        int bj_main(int argc, char* argv[]) {
            return bj_enter_app_main_callbacks(argc, argv, bj_app_begin, bj_app_iterate, bj_app_end);
        }
#   endif

#   if (!defined(BJ_MAIN_USE_CALLBACKS) || defined(BJ_MAIN_CALLBACK_STANDARD))
#       if defined(BJ_OS_WINDOWS)

#           ifndef WINAPI
#               define WINAPI __stdcall
#           endif

            typedef struct HINSTANCE__* HINSTANCE;
            typedef char*               LPSTR;
            typedef wchar_t*            PWSTR;

#           ifdef BJ_COMPILER_MSVC
#               if defined(UNICODE) && UNICODE
                    int wmain(int argc, wchar_t* wargv[], wchar_t* wenvp) {
                        (void)argc;(void)wargv;(void)wenvp;
                        return bj_run_app(0, NULL, bj_main);
                    }
#               else
                    int main(int argc, char* argv[]) {
                        (void)argc;(void)argv;
                        return bj_run_app(0, NULL, bj_main);
                    }
#               endif
#           endif
        
#           ifdef __cplusplus
            extern "C" {
#           endif

#           if defined(UNICODE) && UNICODE
                int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR szCmdLine, int sw) {
#           else
                int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {
#           endif
                (void)hInst; (void)hPrev; (void)szCmdLine; (void)sw;
                return bj_run_app(0, NULL, bj_main);
            }

#           ifdef __cplusplus
            extern }
#           endif
#       else
            int main(int argc, char *argv[]) {
                return bj_run_app(argc, argv, bj_main);
            }
#       endif
#   endif


#   define main bj_main

#endif

#endif



