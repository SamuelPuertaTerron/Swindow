#ifndef SWINDOW_H
#define SWINDOW_H

#ifdef SWINDOW_EXPORTS
#define SWINDOW_API __declspec(dllexport)
#else
#define SWINDOW_API __declspec(dllimport)
#endif

void SWINDOW_API show(const char* title);

#endif // SWINDOW_H