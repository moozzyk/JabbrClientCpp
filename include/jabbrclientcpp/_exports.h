#ifdef JABBRCLIENTCPP_EXPORTS
#define JABBRCLIENTCPP_API __declspec(dllexport)
#else
#define JABBRCLIENTCPP_API __declspec(dllimport)
#endif