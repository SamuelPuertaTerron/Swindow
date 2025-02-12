project "Examples"
    kind "ConsoleApp"
    language "C++"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
        "../Swindow.h",
    }

    includedirs
    {
        "../%{IncludeDir.imgui}",
    }

    links
    {
        "IMGUI",
    }

    filter "system:windows"
    systemversion "latest"
    defines 
    {
        "SW_PLATFORM_WINDOWS" 
    }

    filter "configurations:Debug"
        defines { "SWINDOW_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "SWINDOW_RELEASE" }
        optimize "On"
