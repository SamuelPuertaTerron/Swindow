workspace "Swindow"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    startproject "Examples"

    IncludeDir = {}
    IncludeDir["imgui"] = "Third-Party/imgui"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    include "../Examples"
    include "../Third-Party/imgui"

    