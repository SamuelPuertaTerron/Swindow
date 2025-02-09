workspace "Swindow"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    startproject "Examples"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    include "../Examples"
