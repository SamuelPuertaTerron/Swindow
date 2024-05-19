workspace "Swindow"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    startproject "SampleC"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    include "Swindow"
    include "SampleC"
